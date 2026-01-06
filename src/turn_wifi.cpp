/*
 * Wifi setup/utils for TurningFeather
 */

/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-wi-fi-manager-asyncwebserver/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include "turn_wifi.h"
#ifdef TURN_WIFI_ENABLE

// Store wifi settings in Preferences
Preferences prefs;


// Variables to save values from HTML form
// ssid to connect to as a client
String ssid="";
// Password for auth for client ssid
String pass="";
// ssid to use when being an AP
String ap_ssid="";
// Password for auth for AP ssid
String ap_pass="";
// Fixed local IP for client mode
String ip="";
// Fixed local subnet mask for client mode
String subnet="";
// Fixed local gateway for client mode
String gateway="";
// Local MAC
String MAC="";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// These are used for local IP when connecting/connected
IPAddress localIP;
IPAddress localGateway;
// Default for netmask
IPAddress localSubnet(255, 255, 255, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// Set LED GPIO
const int ledPin = LED_BUILTIN;
// LED state
String ledState;

// Local functions
bool startWiFiClient();
void stopWifi();
String processor(const String& var);
String getStringIf(char *name);
void removeIf(char *name);
void initLittleFS();
String readFile(fs::FS &fs, const char * path);
void writeFile(fs::FS &fs, const char * path, const char * message);


// Initialize LittleFS, needed for serving files
void initLittleFS() {
  if (! LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  } else {
    Serial.println("LittleFS mounted successfully for wifi");
  }
}

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path) {
#ifdef DEBUG
  Serial.printf("Reading file: %s\r\n", path);
#endif //DEBUG

  File file = fs.open(path);
  if (! file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while (file.available()) {
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message) {
#ifdef DEBUG
  Serial.printf("Writing file: %s\r\n", path);
#endif //DEBUG

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (! file.print(message)) {
    Serial.println("- write failed");
#ifdef DEBUG
  } else {
    Serial.println("- file written");
#endif //DEBUG
  }
}

String getStringIf(char *name) {
  // Return a string if it exists otherwise empty
  if (prefs.isKey(name))
    return prefs.getString(name);
  return String();
}

// Remove pref if it exists
void removeIf(char *name) {
  if (prefs.isKey(name))
    prefs.remove(name);
}

// Reset all WiFi Preferences
void resetWifi(void) {
  // Reset settings in prefs to empty.
  // Open prefs R/W
  prefs.begin(TURN_WIFI_PREFS, false);
  prefs.clear();
  prefs.end();
}

// Replaces placeholders with values
String processor(const String& var) {
  if (var == "STATE") {
    if (digitalRead(ledPin)) {
      ledState = "ON";
    } else {
      ledState = "OFF";
    }
    return ledState;
  } else if (var == "MAC") {
    return MAC;
  } else if (var == "SSID") {
    return ssid;
  } else if (var == "PASS") {
    return pass;
  } else if (var == "AP_SSID") {
    return ap_ssid;
  } else if (var == "PASS") {
    return pass;
  } else if (var == "AP_PASS") {
    return ap_pass;
  } else if (var == "IP") {
    // If a static IP has been set
    if (ip != "")
      return "value=\"" + ip + "\"";
    return "placeholder=\"" + localIP.toString() + "\"";
  } else if (var == "SUBNET") {
    if (subnet != "")
      // If a static subnet has been set
      return "value=\"" + subnet + "\"";
    return "placeholder=\"" + localSubnet.toString() + "\"";
  } else if (var == "GATEWAY") {
    if (gateway != "")
      // If a static gateway has been set
      return "value=\"" + gateway + "\"";
    return "placeholder=\"" + localGateway.toString() + "\"";
  } else if (var == "PASSLEN") {
    return String(TURN_WIFI_PASS_MIN);
  }
  // Default is empty string
  return String();
}

// Shut down web and wifi
void stopWifi() {
  server.end();
  WiFi.disconnect();
  Serial.println("WiFi stopped until reboot");
}

// Try to initialize WiFi as a station/client to a WiFi network
bool startWiFiClient() {
  if (ssid == "") {
    Serial.println("No remote SSID set so not connecting as client");
    return false;
  }

  // Connect as a station/client to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(ap_ssid.c_str());
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());
  localSubnet.fromString(subnet.c_str());

  //WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)
  if (!WiFi.config(localIP, localGateway, localSubnet)) {
    Serial.println("WiFi station Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.print("Connecting to remote WiFi ");
  Serial.println(ssid.c_str());
#ifdef DEBUG
  Serial.print("pass ");
  Serial.println(pass);
#endif //DEBUG

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  localIP=WiFi.localIP();
  localSubnet=WiFi.subnetMask();
  localGateway=WiFi.gatewayIP();
  Serial.print("Client WiFi ip: ");
  Serial.println(localIP);
  Serial.print("Client WiFi subnet: ");
  Serial.println(localSubnet);
  Serial.print("Client WiFi gateway: ");
  Serial.println(localGateway);  
  return true;
}

void initWifi() {
  // Work out how we are setting up, AP or client, and do it.

  // Set LED pin as an OUTPUT
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  initLittleFS();


  // Init Preferences R/O
  prefs.begin(TURN_WIFI_PREFS, true);

  ssid=getStringIf("ssid");
  pass=getStringIf("pass");
  ap_pass=getStringIf("ap_pass");
  ip=getStringIf("ip");
  subnet=getStringIf("subnet");
  gateway=getStringIf("gateway");

  if (prefs.isKey("ap_ssid")) {
    ap_ssid=prefs.getString("ap_ssid");
  } else {
#ifdef DEBUG    
    Serial.println("Using default AP SSID");
#endif //DEBUG
    ap_ssid=TURN_WIFI_DEFAULT_AP;
  }
  // Close prefs
  prefs.end();

#ifdef DEBUG  
  Serial.println("Values from Prefs: ");
  Serial.println("##");
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ap_ssid);
  Serial.println(ap_pass);
  Serial.println(ip);
  Serial.println(subnet);
  Serial.println(gateway);
  Serial.println("##");
#endif //DEBUG

  // If we don't have a configured wifi network to join
  // then start up as an access point.
  if(! startWiFiClient()) {
    // No configured wifi to join or joining failed.
    Serial.print("Setting AP as ");
    Serial.println(ap_ssid);
    WiFi.softAPsetHostname(ap_ssid.c_str());
    // If ap_pass is empty then open access point
    if (ap_pass == "") {
      WiFi.softAP(ap_ssid);
    } else {
      WiFi.softAP(ap_ssid, ap_pass);
    }
    localIP=WiFi.softAPIP();
    localSubnet=WiFi.softAPSubnetMask();
    localGateway=WiFi.softAPBroadcastIP();
    Serial.print("AP WiFi ip: ");
    Serial.println(localIP);
    Serial.print("AP WiFi subnet: ");
    Serial.println(localSubnet);
    Serial.print("AP WiFi gateway: ");
    Serial.println(localGateway);
  }
  MAC=WiFi.macAddress();
  Serial.print("WiFi MAC: ");
  Serial.println(MAC);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(ledPin, HIGH);
    request->send(LittleFS, "/www/index.html", "text/html", false, processor);
  });

  // Route to set GPIO state to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(ledPin, LOW);
    request->send(LittleFS, "/www/index.html", "text/html", false, processor);
  });

  // Reset all WiFi configs
  server.on("/WIFIRESET", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Resetting WiFi configuration and restarting.");
    resetWifi();
    request->send(200, "text/plain", "Done. Restart controller then connect to default WiFi.");
    stopWifi();
  });

  // Setup page with values to save
  server.on("/wifisetup.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/wifisetup.html", "text/html", false, processor);
  });

  server.on("/wifisetup.html", HTTP_POST, [](AsyncWebServerRequest *request) {
    // Set WiFi Preferences
    int params = request->params();
    for(int i=0;i<params;i++){
      const AsyncWebParameter* p = request->getParam(i);
      if(p->isPost()){
        // HTTP POST values
        // Open prefs R/W
        prefs.begin(TURN_WIFI_PREFS, false);

        if (p->name() == "ssid") {
          ssid = p->value().c_str();
          ssid.trim();
#ifdef DEBUG
          Serial.print("SSID set to: ");
          Serial.println(ssid);
#endif //DEBUG
          if (ssid != "") {
            prefs.putString("ssid", ssid);
          } else {
            removeIf("ssid");
          }
        }
        // HTTP POST pass value
        if (p->name() == "pass") {
          pass = p->value().c_str();
          pass.trim();
#ifdef DEBUG
          Serial.print("Password set to: ");
          Serial.println(pass);
#endif //DEBUG
          if (pass == "") {
            removeIf("pass");
          } else if (pass.length() >= TURN_WIFI_PASS_MIN) {
            prefs.putString("pass", pass);
          }
        }
        if (p->name() == "ap_ssid") {
          ap_ssid = p->value().c_str();
          ap_ssid.trim();
#ifdef DEBUG
          Serial.print("AP SSID set to: ");
          Serial.println(ap_ssid);
#endif //DEBUG
          if (ap_ssid != "" && ap_ssid != TURN_WIFI_DEFAULT_AP) {
            prefs.putString("ap_ssid", ap_ssid);
          } else {
            removeIf("ap_ssid");
          }
        }
        // HTTP POST ap_pass value
        if (p->name() == "ap_pass") {
          ap_pass = p->value().c_str();
          ap_pass.trim();
#ifdef DEBUG
          Serial.print("AP password set to: ");
          Serial.println(ap_pass);
#endif //DEBUG
          if (ap_pass == "") {
            removeIf("ap_pass");
          } else if (ap_pass.length() >= TURN_WIFI_PASS_MIN) {
            prefs.putString("ap_pass", ap_pass);
          }
        }
        // HTTP POST ip value
        if (p->name() == "ip") {
          ip = p->value().c_str();
          ip.replace(" ", "");
          // TODO ip needs more validation
#ifdef DEBUG
          Serial.print("IP Address set to: ");
          Serial.println(ip);
#endif //DEBUG
          if (ip != "") {
            prefs.putString("ip", ip);
          } else {
            removeIf("ip");
          }
        }
        // HTTP POST gateway value
        if (p->name() == "subnet") {
          subnet = p->value().c_str();
          subnet.replace(" ", "");
          // TODO subnet needs more validation
#ifdef DEBUG
          Serial.print("Subnet set to: ");
          Serial.println(subnet);
#endif //DEBUG
          if (subnet != "") {
            prefs.putString("subnet", subnet);
          } else {
            removeIf("subnet");
          }
        }
        // HTTP POST gateway value
        if (p->name() == "gateway") {
          gateway = p->value().c_str();
          gateway.replace(" ", "");
          // TODO gateway needs more validation
#ifdef DEBUG
          Serial.print("Gateway set to: ");
          Serial.println(gateway);
#endif //DEBUG
          if (gateway != "") {
            prefs.putString("gateway", gateway);
          } else {
            removeIf("gateway");
          }
        }
#ifdef DEBUG_POST
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
#endif //DEBUG_POST
      }
    }
    // Close Preferences
    prefs.end();


    if (pass != "" && pass.length() < TURN_WIFI_PASS_MIN) {
      request->send(200, "text/plain", "Password too short. Minimum length=" + String(TURN_WIFI_PASS_MIN));
    } else if (ap_pass != "" && ap_pass.length() < TURN_WIFI_PASS_MIN) {
      request->send(200, "text/plain", "AP password too short. Minimum length=" + String(TURN_WIFI_PASS_MIN));
    } else {
      request->send(200, "text/plain", "Done. Restart controller to use new settings.");
      //stopWifi();
    }
  });
  // Serve a subdirectory to make sure we do not expose anything else
  server.serveStatic("/", LittleFS, "/www/");
  server.begin();
}
#endif //TURN_WIFI_ENABLE