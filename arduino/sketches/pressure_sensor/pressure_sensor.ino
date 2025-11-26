#include <Arduino.h>
// data structure for storing colors

#define SENSOR A5

float measure_val();

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  pinMode(SENSOR, INPUT);
}

void loop()
{
  float value = measure_val();
  Serial.println(
      (String) "Value:" + value);
}

float measure_val()
{
  return analogRead(SENSOR);
}