#include "Lpf2Hub.h"
#include "M5StickCPlus.h"
#include "JoyC.h"

JoyC joyc;

double x_double;
int y_left;
int y_right;
int y_left_zero;
int y_right_zero;
int marked_menu_entry;
int selected_profile;
char text_buff[100];

Lpf2Hub myHub;
byte portA = (byte)ControlPlusHubPort::A;
byte portB = (byte)ControlPlusHubPort::B;
byte portC = (byte)ControlPlusHubPort::C;
byte portD = (byte)ControlPlusHubPort::D;

bool isCalibrated;
bool reload_menu;

uint8_t show_flag = 0;

void setup() {
  Serial.begin(115200);
  M5.begin();
  Wire.begin(0, 26, (uint32_t)400000);
  y_left_zero = joyc.GetY(0) - 100;
  y_right_zero = joyc.GetY(1) - 100;
  isCalibrated = false;
  reload_menu = true;
  marked_menu_entry = 1;
  selected_profile = 0;
}

void loop() {
  M5.update();

if (selected_profile == 0) {
  // Show Menu
  
  // joyc.GetY(0)-100 == Middle Position
  y_left = joyc.GetY(0) - 100 - y_left_zero;

  if (reload_menu == true) {
    M5.Lcd.fillScreen(BLUE);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextSize(3);
    M5.Lcd.fillRect(0, 10 + 30*(marked_menu_entry-1), 135, 26, BLACK);
    M5.Lcd.println("Buggy");
    M5.Lcd.setCursor(10, 40);
    M5.Lcd.println("Tank");
    reload_menu = false;
  } else {
    int previous_menu_selection = marked_menu_entry;
    if (y_left >= 90) {
      marked_menu_entry = 1;
    } else if (y_left <= -90) {
      marked_menu_entry = 2;
    } else if (joyc.GetPress(0) == 1) {
      //If Button is pressed then select profile
      selected_profile = marked_menu_entry;
      reload_menu = true;
    }
    if (previous_menu_selection != marked_menu_entry) {
      reload_menu = true;
    }

  }
}

if (selected_profile != 0) {


  if (!myHub.isConnected() && !myHub.isConnecting()) {
    myHub.init();
    if (reload_menu) {
        M5.Lcd.fillScreen(BLUE);
        M5.Lcd.setCursor(10, 50);
        M5.Lcd.setTextColor(WHITE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.printf("Connecting");
        reload_menu = false;

      } else {
        M5.Lcd.setCursor(10, 70);
        for (int i = 0; i < 7; i++) {
          delay(200);
          M5.Lcd.printf(".");
        }
        reload_menu = true;
      }
  }

  if (myHub.isConnecting()) {
    myHub.connectHub();
    if (myHub.isConnected()) {
      M5.Lcd.fillScreen(BLUE);
      M5.Lcd.setCursor(10, 50);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("Connected");
    } else {
      Serial.println("Failed to connect to HUB");
    }
  }
}
// *************************************************************************************
// *************************************************************************************
if (selected_profile == 1) {
// Buggy

  if (myHub.isConnected()) {

    // if calibration is not yet complete, reset steering center (assumes motor is physically restricted at +/- 90 degrees)
    if (!isCalibrated) {

      // Move to the minimum endstop (i.e. -90)
      myHub.setTachoMotorSpeedForDegrees(portC, -90, -180);
      delay(1000);

      myHub.setTachoMotorSpeedForDegrees(portC, 90, 180);
      delay(1000);

      // Reset the encoded position to 90
      myHub.setAbsoluteMotorEncoderPosition(portC, 70);
      delay(1000);

      // Move to center
      myHub.setAbsoluteMotorPosition(portC, 100, 0);

      delay(1000);
      // Do not repeat calibration
      isCalibrated = true;
    }

      //Steering
  if (x_double != (180 * joyc.GetX(1) / 200) - 90) {
    x_double = (180 * joyc.GetX(1) / 200) - 90;
    myHub.setAbsoluteMotorPosition(portC, 100, -(int)x_double);
  }

  //Velocity Up and Down
  // joyc.GetY(0)-100 == Middle Position
  y_left = joyc.GetY(0) - 100 - y_left_zero;

  if (y_left > 100) {
    y_left = 100;
  } else if (y_left < -100) {
    y_left = -100;
  }

    myHub.setTachoMotorSpeed(portA, -y_left);
    myHub.setTachoMotorSpeed(portB, -y_left);

    delay(100);

  } 
  }

  //buggy end
// *************************************************************************************
// *************************************************************************************


if (selected_profile == 2) {
//Profile 2

  if (myHub.isConnected()) {


  //Velocity Up and Down
  // joyc.GetY(0)-100 == Middle Position
  y_left = joyc.GetY(0) - 100 - y_left_zero;
  y_right = joyc.GetY(1) - 100 - y_right_zero;

  if (y_left > 100) {
    y_left = 100;
  } else if (y_left < -100) {
    y_left = -100;
  }

  
  if (y_right > 100) {
    y_right = 100;
  } else if (y_right < -100) {
    y_right = -100;
  }

    myHub.setTachoMotorSpeed(portC, y_left);
    myHub.setTachoMotorSpeed(portA, -y_right);

    delay(100);

  } 
  }

//Profile 2 end
// *************************************************************************************
// *************************************************************************************


  if (M5.BtnA.wasPressed()) {
    myHub.shutDownHub();
    reload_menu = true;
    marked_menu_entry = 0;
    selected_profile = 0;
    show_flag = 1 - show_flag;
  }
}
