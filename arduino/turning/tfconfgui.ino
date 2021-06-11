//<File !Start!>
// FILE: [tf-conf-gui.ino]
// Created by GUIslice Builder version: [0.13.0.3]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<File !End!>
//
// (c) pir 2019
//
// ARDUINO NOTES:
// - GUIslice_config.h must be edited to match the pinout connections
//   between the Arduino CPU and the display controller (see ADAGFX_PIN_*).
//

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIslice.h"
#include "GUIslice_drv.h"

// Use fonts from TFT_eSPI instead of adafruit
#include <TFT_eSPI.h>

#ifdef USE_ZPT_SERIAL
#include "zpt_serial.h"
#endif //USE_ZPT_SERIAL

// Include any extended elements
//<Includes !Start!>
// Include extended elements
#include "elem/XCheckbox.h"
#include "elem/XListbox.h"
#include "elem/XRingGauge.h"
#include "elem/XSlider.h"
//<Includes !End!>

// ------------------------------------------------
// Headers and Defines for fonts
// Note that font files are located within the Adafruit-GFX library folder:
// ------------------------------------------------
//<Fonts !Start!>
//<Fonts !End!>

// ------------------------------------------------
// Defines for resources
// ------------------------------------------------
//<Resources !Start!>
//<Resources !End!>

// ------------------------------------------------
// Enumerations for pages, elements, fonts, images
// ------------------------------------------------
//<Enum !Start!>
enum {E_PG_MAIN,E_PG_SIGNAL,E_PG_WIFI_SELECT,E_PG_WIFI_CONF
      ,E_PG_CONFIGS};
enum {CONFIGS_BTN,CONF_RESTART_BTN,E_ELEM_BOX1,E_ELEM_CONFIGS_EXIT_BTN
      ,E_ELEM_CONFIGS_TITLE,E_ELEM_CONF_TITLE,E_ELEM_LISTBOX_CONFIGS
      ,E_ELEM_RINGGAUGE1,E_ELEM_TEXT3,E_ELEM_TEXT_RB_1,E_ELEM_TEXT_RB_2
      ,E_ELEM_TEXT_RB_3,E_ELEM_WIFI_CHOOSE_BTN,E_ELEM_WIFI_CONF_EXIT
      ,E_ELEM_WIFI_CONF_R1,E_ELEM_WIFI_CONF_R2,E_ELEM_WIFI_CONF_R3
      ,E_ELEM_WIFI_CONF_TITLE,E_ELEM_WIFI_NET_CHOOSE
      ,E_ELEM_WIFI_SEL_EXIT,E_ELEM_WIFI_SEL_TITLE
      ,E_ELEM_WIFI_SIG_EXIT_BTN,E_LISTSCROLL1,E_LISTSCROLL4,SIGNAL_BTN
      ,WIFI_BTN};
enum {GSLC_GROUP_ID_WIFI};
// Must use separate enum for fonts with MAX_FONT at end to use gslc_FontSet.
enum {E_FONT_SANS12,E_FONT_SANS24,E_FONT_SANS9,MAX_FONT};
//<Enum !End!>

// ------------------------------------------------
// Instantiate the GUI
// ------------------------------------------------

// ------------------------------------------------
// Define the maximum number of elements and pages
// ------------------------------------------------
//<ElementDefines !Start!>
#define MAX_PAGE                5

#define MAX_ELEM_PG_MAIN 6                                          // # Elems total on page
#define MAX_ELEM_PG_MAIN_RAM MAX_ELEM_PG_MAIN // # Elems in RAM

#define MAX_ELEM_PG_SIGNAL 3                                          // # Elems total on page
#define MAX_ELEM_PG_SIGNAL_RAM MAX_ELEM_PG_SIGNAL // # Elems in RAM

#define MAX_ELEM_PG_WIFI_SELECT 5                                          // # Elems total on page
#define MAX_ELEM_PG_WIFI_SELECT_RAM MAX_ELEM_PG_WIFI_SELECT // # Elems in RAM

#define MAX_ELEM_PG_WIFI_CONF 9                                          // # Elems total on page
#define MAX_ELEM_PG_WIFI_CONF_RAM MAX_ELEM_PG_WIFI_CONF // # Elems in RAM

#define MAX_ELEM_PG_CONFIGS 5                                          // # Elems total on page
#define MAX_ELEM_PG_CONFIGS_RAM MAX_ELEM_PG_CONFIGS // # Elems in RAM
//<ElementDefines !End!>

// ------------------------------------------------
// Create element storage
// ------------------------------------------------
gslc_tsGui                      m_gui;
gslc_tsDriver                   m_drv;
gslc_tsFont                     m_asFont[MAX_FONT];
gslc_tsPage                     m_asPage[MAX_PAGE];

//<GUI_Extra_Elements !Start!>
gslc_tsElem                     m_asPage1Elem[MAX_ELEM_PG_MAIN_RAM];
gslc_tsElemRef                  m_asPage1ElemRef[MAX_ELEM_PG_MAIN];
gslc_tsElem                     m_asPage2Elem[MAX_ELEM_PG_SIGNAL_RAM];
gslc_tsElemRef                  m_asPage2ElemRef[MAX_ELEM_PG_SIGNAL];
gslc_tsElem                     m_asPage3Elem[MAX_ELEM_PG_WIFI_SELECT_RAM];
gslc_tsElemRef                  m_asPage3ElemRef[MAX_ELEM_PG_WIFI_SELECT];
gslc_tsElem                     m_asPage4Elem[MAX_ELEM_PG_WIFI_CONF_RAM];
gslc_tsElemRef                  m_asPage4ElemRef[MAX_ELEM_PG_WIFI_CONF];
gslc_tsElem                     m_asPage5Elem[MAX_ELEM_PG_CONFIGS_RAM];
gslc_tsElemRef                  m_asPage5ElemRef[MAX_ELEM_PG_CONFIGS];
gslc_tsXRingGauge               m_sXRingGauge1;
gslc_tsXListbox                 m_sListbox1;
// - Note that XLISTBOX_BUF_OH_R is extra required per item
char                            m_acListboxBuf1[1024 + XLISTBOX_BUF_OH_R];
gslc_tsXSlider                  m_sListScroll1;
gslc_tsXCheckbox                m_asXRadio1;
gslc_tsXCheckbox                m_asXRadio2;
gslc_tsXCheckbox                m_asXRadio3;
gslc_tsXListbox                 m_sListbox4;
// - Note that XLISTBOX_BUF_OH_R is extra required per item
char                            m_acListboxBuf4[2048 + XLISTBOX_BUF_OH_R];
gslc_tsXSlider                  m_sListScroll4;

#define MAX_STR                 100

//<GUI_Extra_Elements !End!>

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
gslc_tsElemRef*  m_pElemListbox1   = NULL;
gslc_tsElemRef*  m_pElemListbox4   = NULL;
gslc_tsElemRef*  m_pElemXRingGauge1= NULL;
gslc_tsElemRef*  m_pListSlider1    = NULL;
gslc_tsElemRef*  m_pListSlider4    = NULL;
//<Save_References !End!>

// Variables for me
char tfguibuff[128];
// standin for an actual config file
char config_file[128]="/turnconf.txt";

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// My functions

/*
 * Config file
 * {
 *    "wifi": { "current": 1, // index in the array below
 *              "type": 1, // 0=client, 1=server, 2=off
 *              "nets": [["foo", "bar"], // ssid/password
 *                       ["baz", "qux"]]
 *            },
 *    "turnconf": {"sd": true, "name": "/turnconf.txt"}
 * }
 */

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------

// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem* pElem = pElemRef->pElem;

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case WIFI_BTN:
        gslc_SetPageCur(&m_gui,E_PG_WIFI_CONF);
        break;
      case SIGNAL_BTN:
        gslc_SetPageCur(&m_gui,E_PG_SIGNAL);
        break;
      case CONF_RESTART_BTN:
        do_restart();
        break;
      case CONFIGS_BTN:
        gslc_SetPageCur(&m_gui,E_PG_CONFIGS);
        break;
      case E_ELEM_WIFI_SIG_EXIT_BTN:
        gslc_SetPageCur(&m_gui,E_PG_MAIN);
        break;
      case E_ELEM_WIFI_SEL_EXIT:
        gslc_SetPageCur(&m_gui,E_PG_WIFI_CONF);
        break;
      case E_ELEM_WIFI_CHOOSE_BTN:
        gslc_SetPageCur(&m_gui,E_PG_WIFI_SELECT);
        break;
      case E_ELEM_WIFI_CONF_EXIT:
        gslc_SetPageCur(&m_gui,E_PG_MAIN);
        break;
      case E_ELEM_CONFIGS_EXIT_BTN:
        gslc_SetPageCur(&m_gui,E_PG_MAIN);
        break;

//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
bool CbListbox(void* pvGui, void* pvElemRef, int16_t nSelId)
{
  gslc_tsGui*     pGui = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem = gslc_GetElemFromRef(pGui, pElemRef);
  char            acTxt[MAX_STR + 1];
  
  if (pElemRef == NULL) {
    return false;
  }

  // From the element's ID we can determine which listbox was active.
  switch (pElem->nId) {
//<Listbox Enums !Start!>
    case E_ELEM_WIFI_NET_CHOOSE:
      //TODO: Add your listbox selection code here
      if (nSelId != XLISTBOX_SEL_NONE) {
        gslc_ElemXListboxGetItem(&m_gui, pElemRef, nSelId, acTxt, MAX_STR);
      }
      break;
    case E_ELEM_LISTBOX_CONFIGS:
      //TODO: Add your listbox selection code here
      if (nSelId != XLISTBOX_SEL_NONE) {
        gslc_ElemXListboxGetItem(&m_gui, pElemRef, nSelId, config_file, MAX_STR);
        Serial.print("Config file selected: ");
        Serial.println(config_file);
      }
      break;

//<Listbox Enums !End!>
    default:
      break;
  }
  return true;
}
//<Draw Callback !Start!>
//<Draw Callback !End!>

// Callback function for when a slider's position has been updated
bool CbSlidePos(void* pvGui,void* pvElemRef,int16_t nPos)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = pElemRef->pElem;
  int16_t         nVal;

  // From the element's ID we can determine which slider was updated.
  switch (pElem->nId) {
//<Slider Enums !Start!>
    case E_LISTSCROLL1:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pListSlider1);
      // Update the textbox scroll position
      gslc_ElemXListboxSetScrollPos(pGui, m_pElemListbox1, nVal);
      break;
    case E_LISTSCROLL4:
      // Fetch the slider position
      nVal = gslc_ElemXSliderGetPos(pGui,m_pListSlider4);
      // Update the textbox scroll position
      gslc_ElemXListboxSetScrollPos(pGui, m_pElemListbox4, nVal);
      break;

//<Slider Enums !End!>
    default:
      break;
  }

  return true;
}
//<Tick Callback !Start!>
//<Tick Callback !End!>

// ------------------------------------------------
// Create page elements
// ------------------------------------------------
bool InitGUI()
{
  gslc_tsElemRef* pElemRef = NULL;

//<InitGUI !Start!>
  gslc_PageAdd(&m_gui,E_PG_MAIN,m_asPage1Elem,MAX_ELEM_PG_MAIN_RAM,m_asPage1ElemRef,MAX_ELEM_PG_MAIN);
  gslc_PageAdd(&m_gui,E_PG_SIGNAL,m_asPage2Elem,MAX_ELEM_PG_SIGNAL_RAM,m_asPage2ElemRef,MAX_ELEM_PG_SIGNAL);
  gslc_PageAdd(&m_gui,E_PG_WIFI_SELECT,m_asPage3Elem,MAX_ELEM_PG_WIFI_SELECT_RAM,m_asPage3ElemRef,MAX_ELEM_PG_WIFI_SELECT);
  gslc_PageAdd(&m_gui,E_PG_WIFI_CONF,m_asPage4Elem,MAX_ELEM_PG_WIFI_CONF_RAM,m_asPage4ElemRef,MAX_ELEM_PG_WIFI_CONF);
  gslc_PageAdd(&m_gui,E_PG_CONFIGS,m_asPage5Elem,MAX_ELEM_PG_CONFIGS_RAM,m_asPage5ElemRef,MAX_ELEM_PG_CONFIGS);

  // NOTE: The current page defaults to the first page added. Here we explicitly
  //       ensure that the main page is the correct page no matter the add order.
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
  
  // Set Background to a flat color
  gslc_SetBkgndColor(&m_gui,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_MAIN
  
   
  // Create E_ELEM_BOX1 box
  pElemRef = gslc_ElemCreateBox(&m_gui,E_ELEM_BOX1,E_PG_MAIN,(gslc_tsRect){10,40,50,180});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLACK,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // create WIFI_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,WIFI_BTN,E_PG_MAIN,
    (gslc_tsRect){30,50,100,80},(char*)"WiFi setup",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create SIGNAL_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,SIGNAL_BTN,E_PG_MAIN,
    (gslc_tsRect){30,140,100,80},(char*)"RF signal",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create CONF_RESTART_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,CONF_RESTART_BTN,E_PG_MAIN,
    (gslc_tsRect){190,140,100,80},(char*)"Restart",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create CONFIGS_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,CONFIGS_BTN,E_PG_MAIN,
    (gslc_tsRect){190,50,100,80},(char*)"Configs",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // Create E_ELEM_CONF_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_CONF_TITLE,E_PG_MAIN,(gslc_tsRect){79,10,162,30},
    (char*)"Configuration",0,E_FONT_SANS12);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_SIGNAL
  

  // Create ring gauge E_ELEM_RINGGAUGE1 
  static char m_sRingText1[11] = "";
  pElemRef = gslc_ElemXRingGaugeCreate(&m_gui,E_ELEM_RINGGAUGE1,E_PG_SIGNAL,&m_sXRingGauge1,
          (gslc_tsRect){10,50,180,180},
          (char*)m_sRingText1,11,E_FONT_SANS24);
  gslc_ElemXRingGaugeSetValRange(&m_gui, pElemRef, 0, 255);
  gslc_ElemXRingGaugeSetVal(&m_gui, pElemRef, 0); // Set initial value
  gslc_ElemXRingGaugeSetThickness(&m_gui,pElemRef, 20);
  gslc_ElemXRingGaugeSetColorActiveGradient(&m_gui, pElemRef, GSLC_COL_BLUE_LT4, GSLC_COL_RED);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_GREEN_LT4);
  m_pElemXRingGauge1 = pElemRef;
  
  // create E_ELEM_WIFI_SIG_EXIT_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_WIFI_SIG_EXIT_BTN,E_PG_SIGNAL,
    (gslc_tsRect){230,190,80,40},(char*)"Exit",0,E_FONT_SANS12,&CbBtnCommon);
  
  // Create E_ELEM_TEXT3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT3,E_PG_SIGNAL,(gslc_tsRect){43,10,234,22},
    (char*)"RF recieved signal strength",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);

  // -----------------------------------
  // PAGE: E_PG_WIFI_SELECT
  
  
  // Create E_ELEM_WIFI_SEL_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_WIFI_SEL_TITLE,E_PG_WIFI_SELECT,(gslc_tsRect){77,10,165,22},
    (char*)"Select wifi network",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
   
  // Create wrapping box for listbox E_ELEM_WIFI_NET_CHOOSE and scrollbar
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_WIFI_SELECT,(gslc_tsRect){10,40,300,130});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create listbox
  pElemRef = gslc_ElemXListboxCreate(&m_gui,E_ELEM_WIFI_NET_CHOOSE,E_PG_WIFI_SELECT,&m_sListbox1,
    (gslc_tsRect){10+2,40+4,300-23,130-7},E_FONT_SANS9,
    (char*)&m_acListboxBuf1,sizeof(m_acListboxBuf1),0);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 5, 1); // 5 rows, 1 columns
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT3);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLUE_DK3);
  gslc_ElemSetGlowCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLUE_DK3, GSLC_COL_WHITE);
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListbox);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemListbox1 = pElemRef;

  // Create vertical scrollbar for listbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_LISTSCROLL1,E_PG_WIFI_SELECT,&m_sListScroll1,
          (gslc_tsRect){10+300-21,40+4,20,130-8},0,100,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLUE);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pListSlider1 = pElemRef;
  
  // create E_ELEM_WIFI_SEL_EXIT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_WIFI_SEL_EXIT,E_PG_WIFI_SELECT,
    (gslc_tsRect){230,190,80,40},(char*)"Exit",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);

  // -----------------------------------
  // PAGE: E_PG_WIFI_CONF
  
  
  // Create E_ELEM_WIFI_CONF_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_WIFI_CONF_TITLE,E_PG_WIFI_CONF,(gslc_tsRect){112,10,95,22},
    (char*)"WIFI config",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create radio button E_ELEM_WIFI_CONF_R1
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_WIFI_CONF_R1,E_PG_WIFI_CONF,&m_asXRadio1,
    (gslc_tsRect){30,40,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_BLUE_LT4,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,GSLC_GROUP_ID_WIFI);
  
  // Create radio button E_ELEM_WIFI_CONF_R2
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_WIFI_CONF_R2,E_PG_WIFI_CONF,&m_asXRadio2,
    (gslc_tsRect){30,70,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_BLUE_LT4,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,GSLC_GROUP_ID_WIFI);
  
  // Create radio button E_ELEM_WIFI_CONF_R3
  pElemRef = gslc_ElemXCheckboxCreate(&m_gui,E_ELEM_WIFI_CONF_R3,E_PG_WIFI_CONF,&m_asXRadio3,
    (gslc_tsRect){30,100,20,20},true,GSLCX_CHECKBOX_STYLE_ROUND,GSLC_COL_BLUE_LT4,false);
  gslc_ElemSetGroup(&m_gui,pElemRef,GSLC_GROUP_ID_WIFI);
  
  // Create E_ELEM_TEXT_RB_1 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_RB_1,E_PG_WIFI_CONF,(gslc_tsRect){60,40,228,22},
    (char*)"Wifi client (join a network)",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_TEXT_RB_2 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_RB_2,E_PG_WIFI_CONF,(gslc_tsRect){60,70,130,22},
    (char*)"Wifi server (AP)",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create E_ELEM_TEXT_RB_3 text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_TEXT_RB_3,E_PG_WIFI_CONF,(gslc_tsRect){60,100,63,22},
    (char*)"Wifi off",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // create E_ELEM_WIFI_CHOOSE_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_WIFI_CHOOSE_BTN,E_PG_WIFI_CONF,
    (gslc_tsRect){230,140,80,40},(char*)"Config",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
  
  // create E_ELEM_WIFI_CONF_EXIT button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_WIFI_CONF_EXIT,E_PG_WIFI_CONF,
    (gslc_tsRect){230,190,80,40},(char*)"Exit",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);

  // -----------------------------------
  // PAGE: E_PG_CONFIGS
  
  
  // Create E_ELEM_CONFIGS_TITLE text label
  pElemRef = gslc_ElemCreateTxt(&m_gui,E_ELEM_CONFIGS_TITLE,E_PG_CONFIGS,(gslc_tsRect){93,10,157,22},
    (char*)"Choose config file",0,E_FONT_SANS9);
  gslc_ElemSetTxtCol(&m_gui,pElemRef,GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_GRAY_DK3,GSLC_COL_BLACK,GSLC_COL_BLACK);
   
  // Create wrapping box for listbox E_ELEM_LISTBOX_CONFIGS and scrollbar
  pElemRef = gslc_ElemCreateBox(&m_gui,GSLC_ID_AUTO,E_PG_CONFIGS,(gslc_tsRect){10,40,300,140});
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLACK);
  
  // Create listbox
  pElemRef = gslc_ElemXListboxCreate(&m_gui,E_ELEM_LISTBOX_CONFIGS,E_PG_CONFIGS,&m_sListbox4,
    (gslc_tsRect){10+2,40+4,300-23,140-7},E_FONT_SANS9,
    (char*)&m_acListboxBuf4,sizeof(m_acListboxBuf4),0);
  gslc_ElemXListboxSetSize(&m_gui, pElemRef, 6, 1);
  gslc_ElemXListboxItemsSetSize(&m_gui, pElemRef, XLISTBOX_SIZE_AUTO, XLISTBOX_SIZE_AUTO);
  gslc_ElemSetTxtMarginXY(&m_gui, pElemRef, 5, 0);
  gslc_ElemSetTxtCol(&m_gui, pElemRef, GSLC_COL_BLUE_LT3);
  gslc_ElemSetGlowCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLUE_DK3, GSLC_COL_WHITE);
  gslc_ElemSetCol(&m_gui, pElemRef, GSLC_COL_GRAY, GSLC_COL_BLACK, GSLC_COL_BLUE_DK3);
  gslc_ElemXListboxSetSelFunc(&m_gui, pElemRef, &CbListbox);
  gslc_ElemSetFrameEn(&m_gui,pElemRef,true);
  m_pElemListbox4 = pElemRef;

  // Create vertical scrollbar for listbox
  pElemRef = gslc_ElemXSliderCreate(&m_gui,E_LISTSCROLL4,E_PG_CONFIGS,&m_sListScroll4,
          (gslc_tsRect){10+300-21,40+4,20,140-8},0,100,0,5,true);
  gslc_ElemSetCol(&m_gui,pElemRef,GSLC_COL_BLUE,GSLC_COL_BLACK,GSLC_COL_BLUE);
  gslc_ElemXSliderSetPosFunc(&m_gui,pElemRef,&CbSlidePos);
  m_pListSlider4 = pElemRef;
  
  // create E_ELEM_CONFIGS_EXIT_BTN button with text label
  pElemRef = gslc_ElemCreateBtnTxt(&m_gui,E_ELEM_CONFIGS_EXIT_BTN,E_PG_CONFIGS,
    (gslc_tsRect){230,190,80,40},(char*)"Exit",0,E_FONT_SANS9,&CbBtnCommon);
  gslc_ElemSetRoundEn(&m_gui, pElemRef, true);
//<InitGUI !End!>

  return true;
}

void tf_conf_gui_setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  gslc_InitDebug(&DebugOut);

  if (!gslc_Init(&m_gui,&m_drv,m_asPage,MAX_PAGE,m_asFont,MAX_FONT)) { return; }

  // ------------------------------------------------
  // Load Fonts
  // ------------------------------------------------
//<Load_Fonts !Start!>
    if (!gslc_FontSet(&m_gui,E_FONT_SANS12,GSLC_FONTREF_PTR,&FreeSans12pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SANS24,GSLC_FONTREF_PTR,&FreeSans24pt7b,1)) { return; }
    if (!gslc_FontSet(&m_gui,E_FONT_SANS9,GSLC_FONTREF_PTR,&FreeSans9pt7b,1)) { return; }
//<Load_Fonts !End!>

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUI();

//<Startup !Start!>
  gslc_GuiRotate(&m_gui, 3);
//<Startup !End!>

  // Setup file list in listbox
  uint16_t i=0;
  if (!gslc_ElemXListboxAddItem(&m_gui, m_pElemListbox4, "SPIFFS backup file")) {
    Serial.println("Adding base entry to listbox failed");    
  }
  if (sd_init_ok) {
    // Set no entry selected
    gslc_ElemXListboxSetSel(&m_gui, m_pElemListbox4, -1);
    File root=SD.open("/");
    File entry;
    while (true) {
      entry=root.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      if (! entry.isDirectory()) {
        // Only want root directory files
        i++;
        Serial.print(F("Found file: "));
        Serial.println(entry.name());
        if (!gslc_ElemXListboxAddItem(&m_gui, m_pElemListbox4, entry.name())) {
          Serial.println("Adding entry to listbox failed");
        }
        if (strncmp(entry.name(), config_file, sizeof(config_file)) == 0) {
          // If the file we just found matches the config file, set it as selected
          Serial.println("Found current config file");
          gslc_ElemXListboxSetSel(&m_gui, m_pElemListbox4, i);
        }
      }
      entry.close();
    }
  }
}

// -----------------------------------
// Main event loop
// -----------------------------------
void tf_conf_gui_loop()
{
  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
#ifdef USE_ZPT_SERIAL
  zpt_serial_loop();
  if (zpt_serialpacket_ready) {
    gslc_ElemXRingGaugeSetVal(&m_gui, m_pElemXRingGauge1, packetin.rssi);
    snprintf(tfguibuff, sizeof(tfguibuff), "%03d", packetin.rssi);
    gslc_ElemSetTxtStr(&m_gui, m_pElemXRingGauge1, tfguibuff);    
    // Signal ready for another packet.
    zpt_serialpacket_ready=false;
  }
#endif //USE_ZPT_SERIAL
  
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
  // Slow down updates
  delay(20);
}
