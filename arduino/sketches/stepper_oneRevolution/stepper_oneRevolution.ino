#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::HALF4WIRE, 8, 10, 9, 11);
int printCounter = 0;

void setup()
{
  // stepper.setMaxSpeed(680);
  stepper.setMaxSpeed(1360);
  stepper.setAcceleration(300);
  stepper.setSpeed(200);
  Serial.begin(9600);
  stepper.moveTo(2048 * 5); // one revolution forward
}

void loop()
{
  if (stepper.distanceToGo() == 0)
  {
    stepper.moveTo(-stepper.currentPosition()); // reverse direction
  }
  printCounter++;
  if (printCounter % 1000 == 0)
  {
    // Serial.println((String)"current speed:" + stepper.speed());
  }
  stepper.run(); // run with acceleration
}