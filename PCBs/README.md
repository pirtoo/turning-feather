# turning feather PCB and build

## Parts list to assemble with PCB

| Part | Quantity | Source |
| ---- | -------- | ------ |
| Capacitors |
| 22uF | x1 | |
| 0.1uF | x1 | |
| 470uF | x1 | |
| Resistors |
| 10kR | x4 | |
| 560R | x1 | |
| 2.2kR | x1 | |
| Diodes |
| 3mm LED | x2 | |
| 1N5817 | x4 | https://uk.farnell.com/stmicroelectronics/1n5817/diode-schottky-1a-20v-do-41/dp/9801197 |
| Connectors and cables |
| Socket headers | x1 pack | https://thepihut.com/products/36-pin-swiss-female-socket-headers-pack-of-5 |
| JST-PH 3 way PCB 90 degree | x1 | https://thepihut.com/products/jst-ph-jumper-assembly-3-wire |
| JST-PH 3 way plug and cable | x1 | Included in above |
| JST-PH 4 way PCB 90 degree | x1 | https://thepihut.com/products/jst-ph-jumper-assembly-4-wire |
| JST-PH 4 way plug and cable | x1 | Included in above |
| RJ45 PCB Shielded | x1 | https://uk.farnell.com/wurth-elektronik/615008142621/rj45-conn-r-a-jack-8p8c-1port/dp/2827822?gross_price=true&CMP=KNC-GUK-GEN-SHOPPING-PMAX-HIGH-ROAS&gad_source=1&gad_campaignid=17568744370&gclid=CjwKCAiA09jKBhB9EiwAgB8l-Keu6t1KMnuOvlgZ3LdBT3_u35CNMEyAGO-oGcBRlu94OLG5bAtN4hoCo3kQAvD_BwE |
| Panel Mount Extension USB Cable | x1 | https://thepihut.com/products/panel-mount-extension-usb-cable-micro-b-male-to-micro-b-female |
| SMA to uFL cable | x1 | https://thepihut.com/products/sma-to-ufl-u-fl-ipx-ipex-rf-adapter-cable or included with antenna |
| SMA antenna | x1 | https://thepihut.com/products/lora-antenna-with-pigtail-868mhz-black |
| Adafruit Stacking Headers for Feather | x1 | https://thepihut.com/products/stacking-headers-for-feather-12-pin-and-16-pin-female-headers |
| Switches |
| Panel mount DPDT toggle switch | x1 | https://www.adafruit.com/product/3220?srsltid=AfmBOopktvlaYSxf00hTX3HSPP8zWhvR285GFbeFXXPOa2BU2tYauQCs |
| 6mm PCB button | x1 | https://thepihut.com/products/metal-ball-tactile-button-6mm-x-10-pack |
| Panel mount push buttons | x2 | https://thepihut.com/products/12mm-domed-push-button-6-pack |
| Boards and ICs | | |
| Feather HUZZAH32 | x1 | https://thepihut.com/products/adafruit-huzzah32-esp32-feather-board-pre-soldered-ada3591 |
| TFT FeatherWing - 2.4" 320x240 | x1 | https://thepihut.com/products/adafruit-tft-featherwing-2-4-320x240-touchscreen-for-all-feathers |
| ZPT receiver DIP 868Mhz | x1 | https://www.rfsolutions.co.uk/radio-modules/zpt-433-868mhz-radio-telemetry-modules/?searchid=35864&search_query=zpt |
| Remote control 868Mhz 4 buttons | x1 | https://www.rfsolutions.co.uk/remote-control-systems/fobber-433mhz-868mhz-remote-control-key-fob-transmitter/ or https://www.rfsolutions.co.uk/remote-control-systems/trap-handheld-transmitters-868mhz-or-918mhz-versions/ |
| TSR 1-2450 | x1 | https://uk.farnell.com/tracopower/tsr-1-2450/converter-dc-dc-5v-1a-sip/dp/1696320 |
| 4427 mosfet driver DIP-8 | x2 | https://uk.farnell.com/maxim-integrated-products/max4427epa/mosfet-driver-high-low-side-dip/dp/2516370 |
| Other |
| PCB | x1 | https://www.pcbway.com/project/shareproject/Turning_Feather.html |
| 5v buzzer | x1 | https://thepihut.com/products/buzzer-5v-breadboard-friendly |
| 2.5mm plastic standoff kit | x1 | https://thepihut.com/products/adafruit-black-nylon-screw-and-stand-off-set-m2-5-thread |
| Case Grey ABS Box Clear Lid 150x80x76 | x1 | https://thepihut.com/products/small-plastic-project-enclosure-weatherproof-with-clear-top |
| LiPo backup battery | x1 | https://thepihut.com/products/150mah-3-7v-lipo-battery |
| MicroSD card 1Gb-32Gb | x1 | |

One of the 1N5817 diodes should be inserted into the 5v supply line in the panel mount extension cable so when the unit has 12v supply, converting to 5V via the TSR 1-2450, this does not feed back to a USB connected device. 5v can be supplied either through the converter or from USB.

Input voltage is stated at 12v since this is what the original system uses. Minimum input voltage via the RJ45 is 6.5v and maxium input voltage is 36v to the TSR 1-2450. Maximum input voltage to the MAX4427EPA+ is 18v. Voltage output to the turn/away signals will be the input voltage.

The DPDT toggle switch turns the unit on and off and connects to J1 via the JST-PH 4 way cable. Pins 2 and 3 to each common on the switch, pin 1 to pin 2 in one position and pin 3 to pin 4 in the other position. In the off position 12v from the PCB RJ45 is not supplied back to the board and the enable pin of the Feather is grounded.

The two panel switches and the buzzer connect to J3 via the JST-PH 3 way cable. Pin 1 to the buzzer positive pin, pin 3 to the buzzer ground. Pin 2 to the first button and to a resistor to the second button and ground to the other button pins. This way when the button is pressed pin 2 is grounded directly, when the other button is pressed pin 2 is grounded via the resistor.

With the HUZZAH32 you cannot read analog inputs on ADC #2 once WiFi has started as it is shared with the WiFi module. This pin is used as a digital input from the radio receiver so this should not cause problems. Maximum current output from each output pin is 1.1 mA, hence running the buzzer through a mosfet driver.

## Options

The LiPo battery is optional, a small battery is useful for using the controller when not connected to the target system and in case power is temporarily interrupted. A small battery will fit under the Feather board and connect to it.

Instead of using a PCB mounted RJ45 connector the optional J2 connector can be used to connect to 12v, ground, and turn/away signals. The 12v will need to be wired via a switch if needed. If you are not using J2 there is no need to fit a connector.

If you do not need a buzzer then U3 can be left off. The output will still flash the onboard LED by default. J3 pin 2 can be left unconnected.

# Assembly

Be careful to orient diodes, LEDs and tantalum/electrolytic capacitors correctly.

Note that some components (RJ45, LEDs and learn button) are placed on the rear side of the PCB.

The ZPT antenna is connected via the uFL connector on the ZPT board.

Plastic M2.5 standoffs should be used to mount the Featherwing display and HUZZAH32 to the IO board. Note that two of the mounting holes on the HUZZAH32 are partially obscured by the onboard 2.4GHz antenna so cannot be used.