#include <Arduino.h>
// data structure for storing colors

#define LDR A5
#define RIPENESS_LED 8
#define LED_R 6
#define LED_G 5
#define LED_B 7
struct Rgb
{
  float r;
  float g;
  float b;
};
Rgb measure_rgb();
Rgb white;
Rgb black;
bool is_ripe(Rgb color);
void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  white = Rgb{1, 1, 1};
  black = Rgb{0, 0, 0};
  pinMode(LDR, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(RIPENESS_LED, OUTPUT);

  Rgb temp_white = measure_rgb(false);
  delay(2000);
  Rgb temp_black = measure_rgb(false);
  delay(2000);
  white = temp_white;
  black = temp_black;
}

void loop()
{
  Rgb color = measure_rgb(true);
  digitalWrite(RIPENESS_LED, is_ripe(color) ? HIGH : LOW);
}

Rgb measure_rgb(bool normalise)
{
  int led_pins[3] = {LED_R, LED_G, LED_B};
  float rgb[3];
  for (int color = 0; color < 3; color++)
  {
    digitalWrite(led_pins[color], HIGH);
    long long int measurement = 0;
    int n = 100;
    for (int i = 0; i < n; i++)
    {
      measurement += analogRead(LDR);
      delay(10);
    }
    digitalWrite(led_pins[color], LOW);
    rgb[color] = measurement / n;
    delay(100);
  }
  Rgb normalised = Rgb{
      (rgb[0] - black.r) / (white.r - black.r),
      (rgb[1] - black.g) / (white.g - black.g),
      (rgb[2] - black.b) / (white.b - black.b),
  };
  Rgb raw{
      rgb[0],
      rgb[1],
      rgb[2],
  };

  Serial.println(
      (String) "NormalisedBlue:" + normalised.b + "," +
      "NormalisedRed:" + normalised.r + "," +
      "NormalisedGreen:" + normalised.g + "," +
      "RawBlue:" + raw.b + "," +
      "RawRed:" + raw.r + "," +
      "RawGreen:" + raw.g);

  if (normalise)
  {
    return normalised;
  }
  return raw;
}

bool is_ripe(Rgb color)
{
  if (color.r > (abs(color.b) + abs(color.g)) * 0.8)
  {
    return true;
  }
  else
  {
    return false;
  }
}