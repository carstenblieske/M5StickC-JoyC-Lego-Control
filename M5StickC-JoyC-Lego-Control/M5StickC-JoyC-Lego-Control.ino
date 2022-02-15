#include "Lpf2Hub.h"
#include "M5StickCPlus.h"
#include "JoyC.h"

JoyC joyc;

double x_double;
int y_left;
int y_left_zero;
char text_buff[100];

Lpf2Hub myHub;
byte portA = (byte)ControlPlusHubPort::A;
byte portB = (byte)ControlPlusHubPort::B;
byte portC = (byte)ControlPlusHubPort::C;
byte portD = (byte)ControlPlusHubPort::D;

  bool isCalibrated;
  
  uint8_t show_flag = 0;
  
  void setup()
  {
      Serial.begin(115200);
      isCalibrated = false;
      M5.begin();
      myHub.init(); 
      Wire.begin(0, 26, 400000);
      M5.Lcd.fillScreen(BLUE);
      M5.Lcd.setCursor(10, 50);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("Connecting");
      y_left_zero = joyc.GetY(0)-100;

}

void loop()
{
  if (!myHub.isConnected() && !myHub.isConnecting()) 
  {
    myHub.init(); 
  }
  
  if (myHub.isConnecting()) {
    myHub.connectHub();
    if (myHub.isConnected()) {
      M5.Lcd.fillScreen(BLUE);
      M5.Lcd.setCursor(10, 50);
      M5.Lcd.printf("Connected");
    } else {
      Serial.println("Failed to connect to HUB");
    }
  }
  M5.update();
  // if connected, you can set the name of the hub, the led color and shut it down
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
    if (x_double != (180*joyc.GetX(1)/200)-90) {
      x_double = (180*joyc.GetX(1)/200)-90;
      myHub.setAbsoluteMotorPosition(portC, 100, -(int) x_double);

    }
    y_left = joyc.GetY(0)-100 - y_left_zero;

    if (y_left > 100) {
      y_left = 100;
    } else if (y_left < -100) {
      y_left = -100;
    }

    //Serial.println(y_left);

    myHub.setTachoMotorSpeed(portA, -y_left);
    myHub.setTachoMotorSpeed(portB, -y_left);
    myHub.setTachoMotorSpeed(portD, -y_left);
    
    delay(100);

  } else {
    Serial.println("ControlPlus hub is disconnected");
  }

    if(M5.BtnA.wasPressed())
    {
      myHub.shutDownHub();
        joyc.SetLedColor(random(0xFFFFFF));    
        show_flag = 1 - show_flag;
    }
}
