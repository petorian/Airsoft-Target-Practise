/* Airsoft Target to test reaction

  Starts with blue leds on which indicates stand-by.
  First hit detected activates the target; the leds turn off and wait a random time after which they turn yellow for 2 seconds (or other timeout value)
  If hit too early, it will count as a miss
  if not hit at all, it will count as a miss
  if hit within the 2 second timeout, it will count as a hit.

  After the number of cycles have passed, the leds will flash white one time and then it will show the score:
  it will flash green for every hit and after that flash red for every miss.
  Then it will return to stand-by waiting for another initial hit to activate the target again.

  this sketch is based on the sketches in this topic on the arduino forum: http://forum.arduino.cc/index.php?topic=373655.0
 
*/
#include <Adafruit_NeoPixel.h>

int TIMEOUT_MILLISECONDS = 2000;  // Set time to hit target
int indicatorTime = 1000; // Set how long the leds flash during a hit or miss.
int targetCycles = 10; // Set the number of rounds.

const char* const stateText[] = { "Stand By", "Active Target", "Early Hit", "Hit", "No Hit", "End Game"};

enum GameState {
  STAND_BY,
  ACTIVE_TARGET,
  EARLY_HIT,
  GOOD_HIT,
  LATE_HIT,
  END_GAME,
};

GameState gameState = STAND_BY;
GameState lastState;

#define HITPIN A0
#define LEDPIN D2
#define NUMPIXELS 6
const int threshold = 100;
int cycleCounter = 0;
int hitCounter = 0;
int missCounter = 0;
int sensorReading = 0;
bool ledState = false;
bool yellowLed = false;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

unsigned long lastMillis = 0;
unsigned long randomTimerDelay = 0;

void setup()
{
  Serial.begin(112500);
  pinMode(HITPIN, INPUT);
  pixels.begin();
  randomSeed(analogRead(A0));
  Serial.println(stateText[gameState]);
}

void loop()
{

  bool pressed = checkButton();
  if (gameState == STAND_BY)
  {
    ledOn(0, 0, 10);
    if (pressed)
    {
      ledOff();
      randomTimerDelay = 3000 + random(7000);  // random time
      gameState = ACTIVE_TARGET;
    }
  }
  else if (gameState == ACTIVE_TARGET)
  {
    //Serial.println(cycleCounter);
    unsigned long elapsedMillis = millis() - lastMillis;
    if (elapsedMillis >= randomTimerDelay && yellowLed == false && cycleCounter < targetCycles)
    {
      ledOn(180, 100, 0);
      yellowLed = true;
      Serial.println(F("Lite On!!"));
    }
    else if (elapsedMillis > randomTimerDelay + TIMEOUT_MILLISECONDS && cycleCounter < targetCycles)
    {
      Serial.println(F("TimeOut!!"));
      ledOff();
      yellowLed = false;
      cycleCounter++;
      missCounter++;
      gameState = LATE_HIT;
      lastMillis = millis();
    }
    else if (cycleCounter >= targetCycles)
    {
      ledOff();
      gameState = END_GAME;
    }
    if (pressed)
    {
      if (yellowLed)
      {
        cycleCounter++;
        hitCounter++;
        gameState = GOOD_HIT;
        lastMillis = millis();
      }
      else
      {
        cycleCounter++;
        missCounter++;
        gameState = EARLY_HIT;
        lastMillis = millis();
      }
    }
  }
  else if (gameState == EARLY_HIT)
  {
    unsigned long elapsedMillis = millis() - lastMillis;
    if (elapsedMillis >= indicatorTime)
    {
      ledOff();
      randomTimerDelay = 3000 + random(7000);  // random time
      gameState = ACTIVE_TARGET;
    }
    else
    {
      ledFlash(50, 0, 0);
    }
  }
  else if (gameState == GOOD_HIT)
  {
    unsigned long elapsedMillis = millis() - lastMillis;
    if (elapsedMillis >= indicatorTime)
    {
      ledOff();
      yellowLed = false;
      randomTimerDelay = 3000 + random(7000);  // random time
      gameState = ACTIVE_TARGET;
    }
    else
    {
      ledFlash(0, 50, 0);
    }
  }
  else if (gameState == LATE_HIT)
  {
    unsigned long elapsedMillis = millis() - lastMillis;
    if (elapsedMillis >= indicatorTime)
    {
      ledOff();
      randomTimerDelay = 3000 + random(7000);  // random time
      gameState = ACTIVE_TARGET;
    }
    else
    {
      ledFlash(255, 0, 0);
    }
  }
  else if (gameState == END_GAME)
  {
    ledOff();
    showCount(hitCounter, missCounter);
    delay(1000);
    cycleCounter = 0;
    hitCounter = 0;
    missCounter = 0;
    gameState = STAND_BY;
  }
  if (lastState != gameState)
  {
    Serial.println(stateText[gameState]);
    lastState = gameState;
  }

}

bool checkButton()
{
  static byte lastButtonState = 0;
  
    sensorReading = analogRead(HITPIN);
    Serial.println(sensorReading);
  if (sensorReading >= threshold)
  {
    delay(100);
    Serial.println(F("!"));
    Serial.println(sensorReading);
    lastMillis = millis();
    return true;
  }
  lastButtonState = 1;
  return false;
}

void ledFlash(int r, int g, int b ) {

  static unsigned long lastFlashMillis = 0;
  if (millis() - lastFlashMillis > 200UL) {
    if (ledState == false) {
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(r, g, b));
        pixels.show();
      }
    } else {
      for (int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();
      }
    }
    ledState = !ledState;
    lastFlashMillis = millis();
  }
}

void ledOn(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
    pixels.show();
  }
}

void ledOff() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
  }
}

void showCount(int hit, int miss) {
  ledOn(100,100,100);
  delay(1000);
  ledOff();
  delay(2000);
  
  for (int h = 0; h < hit; h++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 255, 0));
      pixels.show();
    }
    delay(1000);
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
    delay(1000);
  }
  for (int m = 0; m < miss; m++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(255, 0, 0));
      pixels.show();
    }
    delay(1000);
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      pixels.show();
    }
    delay(1000);
  }
}
