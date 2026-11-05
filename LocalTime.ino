#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

int servoMin = 150;
int servoMax = 600;

// Eigen waarden voor servo 5
int servo5Min = 475;
int servo5Max = 800;

void setup() {

  Wire.begin(1, 2);

  pwm.begin();

  pwm.setPWMFreq(50);

  delay(500);
}

void loop() {

  // Alle 7 servo's naar minimum
  pwm.setPWM(0, 0, servoMin);
  pwm.setPWM(1, 0, servoMin);
  pwm.setPWM(2, 0, servoMin);
  pwm.setPWM(3, 0, servoMin);
  pwm.setPWM(4, 0, servoMin);

  // Servo 5 heeft zijn eigen minimum
  pwm.setPWM(5, 0, servo5Min);

  pwm.setPWM(6, 0, servoMin);

  delay(3000);


  // Alle 7 servo's naar het midden
  pwm.setPWM(0, 0, (servoMin + servoMax) / 2);
  pwm.setPWM(1, 0, (servoMin + servoMax) / 2);
  pwm.setPWM(2, 0, (servoMin + servoMax) / 2);
  pwm.setPWM(3, 0, (servoMin + servoMax) / 2);
  pwm.setPWM(4, 0, (servoMin + servoMax) / 2);

  // Servo 5 heeft zijn eigen waarden
  pwm.setPWM(5, 0, (servo5Min + servo5Max) / 2);

  pwm.setPWM(6, 0, (servoMin + servoMax) / 2);

  delay(3000);
}