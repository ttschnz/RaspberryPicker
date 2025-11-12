#include <Arduino.h>

const int buttonPin = 4;  // the number of the pushbutton pin


class LimitSwitch{
    public:
        LimitSwitch(int pin);

        /**
         * returns true if the limit-switch's reading is HIGH
         */
        bool is_touching();
    private:
        int pin;
};


LimitSwitch::LimitSwitch(int pin){
    this->pin = pin;
    pinMode(this->pin, INPUT_PULLUP);
}

bool LimitSwitch::is_touching(){
    int reading = digitalRead(this->pin);
    bool is_high = reading == HIGH;
    return is_high;
}

LimitSwitch *limit_switch;
void setup() {
  // initialize the pushbutton pin as an input:
  limit_switch = new LimitSwitch(buttonPin);
  Serial.begin(9600);
}

void loop() {
  if (limit_switch->is_touching()){
    Serial.println("touching");
  }else{
    Serial.println("not touching");
  }
  delay(200);
}
