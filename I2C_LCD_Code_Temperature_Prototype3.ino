#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int ThermistorPins[] = {A0, A1, A2};
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 0.7634002931e-03, c2 = 2.728979140e-04, c3 = 0.9938429021e-07;

const int numSamples = 10; // Number of samples for the moving average
int samples[3][numSamples];
int sampleIndex[3]= {0,0,0};
float averageVo[3];

int buzzPin = 11; // Define the pin for the buzzer
int buttonPin = 12; // Define the pin for the push button

const int baby1blue = 2;      // baby 1 blue
const int baby1green = 3;     // baby 1 green
const int baby1red = 4;       // baby 1 red
const int baby2blue = 5;      // baby 2 blue
const int baby2green = 6;     // baby 2 green
const int baby2red = 7;       // baby 2 red
const int baby3blue = 8;    // baby 3 blue
const int baby3green = 9;    // baby 3 green
const int baby3red = 10;   // baby 3 red

float low1 = 99.9;
float low2 = 99.9;
float low3 = 99.9;
float high1 = 11.1;
float high2 = 11.1;
float high3 = 11.1;

double hot = 37.5; // set hot parameter
double cold = 36.0; // set cold parameter

const int numReadings = 12; // 12 readings for 1 hour (5-minute intervals)
float MinTemperatureReadings1[numReadings]; // Array to store temperature readings
float MinTemperatureReadings2[numReadings]; // Array to store temperature readings
float MinTemperatureReadings3[numReadings]; // Array to store temperature readings
float MaxTemperatureReadings1[numReadings]; // Array to store temperature readings
float MaxTemperatureReadings2[numReadings]; // Array to store temperature readings
float MaxTemperatureReadings3[numReadings]; // Array to store temperature readings
int currentIndex = 0; // Current index in the array
unsigned long previousMillis = 0;
const long interval = 300000; // 5 minutes in milliseconds

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

bool buzzerMuted = false; // Buzzer mute state
unsigned long muteStartTime = 0; // Start time of buzzer mute
const unsigned long muteDuration = 5 * 60 * 1000; // 5 minutes in milliseconds

void setup() {
  Serial.begin(9600);
  
  // LCD setup
  lcd.init();         // initialize the lcd
  lcd.backlight();    // Turn on the LCD screen backlight

  // LED setup baby 1
  pinMode(baby1blue, OUTPUT); // blue LED for baby 1
  pinMode(baby1green, OUTPUT); // green LED for baby 1
  pinMode(baby1red, OUTPUT); // red LED for baby 1

  // LED setup baby 2
  pinMode(baby2blue, OUTPUT); // blue LED for baby 2
  pinMode(baby2green, OUTPUT); // green LED for baby 2
  pinMode(baby2red, OUTPUT); // red LED for baby 2

  // LED setup baby 3
  pinMode(baby3blue, OUTPUT); // blue LED for baby 3
  pinMode(baby3green, OUTPUT); // green LED for baby 3
  pinMode(baby3red, OUTPUT); // red LED for baby 3

  pinMode(buzzPin, OUTPUT); // Set the buzzPin as an output
  pinMode(buttonPin, INPUT_PULLUP); // Set the buttonPin as an input with an internal pull-up resistor

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < numSamples; j++) {
      samples[i][j] = analogRead(thermistorPins[i]);
    }

   for (int i = 0; i < numReadings; i++) {
    MinTemperatureReadings1[i] = 99.9;
  }

  for (int i = 0; i < numReadings; i++) {
    MaxTemperatureReadings1[i] = 11.1;
  }

}

void loop() {
  unsigned long currentMillis = millis();

  double currentTemperature1 = readThermistor(0);
  double currentTemperature2 = readThermistor(1);
  double currentTemperature3 = readThermistor(2);

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Get the latest temperature reading (replace this with your temperature reading logic)
        MinTemperatureReadings1[currentIndex] = currentTemperature1;
        MaxTemperatureReadings1[currentIndex] = currentTemperature1;
        MinTemperatureReadings2[currentIndex] = currentTemperature2;
        MaxTemperatureReadings2[currentIndex] = currentTemperature2;
        MinTemperatureReadings3[currentIndex] = currentTemperature3;
        MaxTemperatureReadings3[currentIndex] = currentTemperature3;
    

    currentIndex++;
    if (currentIndex >= numReadings) {
      currentIndex = 0; // Wrap around to the beginning
    }

    // Find the minimum temperature in the past hour
    low1 = findMinTemperature(1);
    low2 = findMinTemperature(2);
    low3 = findMinTemperature(3);
    high1 = findMaxTemperature(1);
    high2 = findMaxTemperature(2);
    high3 = findMaxTemperature(3);


  printTemperature();
  delay(500);
  displayLCD();
  delay(500); // Add a delay to make the updates readable

  // baby 1
     if (currentTemperature1 < cold) {
    digitalWrite(baby1blue, HIGH);
    digitalWrite(baby1green, LOW);
    digitalWrite(baby1red, LOW);
  }
  if (currentTemperature1 >= cold && currentTemperature1 <= hot) {
    digitalWrite(baby1blue, LOW);
    digitalWrite(baby1green, HIGH);
    digitalWrite(baby1red, LOW);
  }
  if (currentTemperature1 > hot) {
    digitalWrite(baby1blue, LOW);
    digitalWrite(baby1green, LOW);
    digitalWrite(baby1red, HIGH);
  }

  // baby 2
    if (currentTemperature2 < cold) {
    digitalWrite(baby2blue, HIGH);
    digitalWrite(baby2green, LOW);
    digitalWrite(baby2red, LOW);
  }
  if (currentTemperature2 >= cold && currentTemperature2 <= hot) {
    digitalWrite(baby2blue, LOW);
    digitalWrite(baby2green, HIGH);
    digitalWrite(baby2red, LOW);
  }
  if (currentTemperature2 > hot) {
    digitalWrite(baby2blue, LOW);
    digitalWrite(baby2green, LOW);
    digitalWrite(baby2red, HIGH);
  }

  // baby 3
    if (currentTemperature3 < cold) {
    digitalWrite(baby3blue, HIGH);
    digitalWrite(baby3green, LOW);
    digitalWrite(baby3red, LOW);
  }
  if (currentTemperature3 >= cold && currentTemperature3 <= hot) {
    digitalWrite(baby3blue, LOW);
    digitalWrite(baby3green, HIGH);
    digitalWrite(baby3red, LOW);
  }
  if (currentTemperature3 > hot) {
    digitalWrite(baby3blue, LOW);
    digitalWrite(baby3green, LOW);
    digitalWrite(baby3red, HIGH);
  }

int baby1cold = digitalRead(baby1blue); 
int baby2cold = digitalRead(baby2blue); 
int baby3cold = digitalRead(baby3blue); 

int baby1hot = digitalRead(baby1red);
int baby2hot = digitalRead(baby2red); 
int baby3hot = digitalRead(baby3red); 

  if (baby1cold == HIGH || baby2cold == HIGH || baby3cold == HIGH || baby1hot == HIGH || baby2hot == HIGH || baby3hot == HIGH){
      if (!buzzerMuted) {
      buzzerBeep(10); // Change the number of beeps as needed
      delay(5000);
    }
  }

  stopBeep();

}

double readThermistor(int thermistorIndex) {
  int thermistorPin = thermistorPins[thermistorIndex];

  // Read the sensor value and update the samples array
  samples[thermistorIndex][sampleIndex[thermistorIndex]] = analogRead(thermistorPin);
  sampleIndex[thermistorIndex] = (sampleIndex[thermistorIndex] + 1) % numSamples;


  // Calculate the average of the samples
  int sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += samples[thermistorIndex][i];
  }
  averageVo[thermistorIndex] = sum / (float)numSamples;

  // Calculate resistance and temperature
  R2 = R1 * (1023.0 / averageVo[thermistorIndex] - 1.0);
  logR2 = log(R2);
  T = 1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2);
  Tc = T - 273.15;
  return Tc; // Return temperature in Celsius
}

void printTemperature() {
  // Print the temperature values to the Serial Monitor
  Serial.print("Temperature 1: "); Serial.print(currentTemperature1); Serial.println(" C");

  Serial.print("Temperature 2: "); Serial.print(currentTemperature2); Serial.println(" C");

  Serial.print("Temperature 3: "); Serial.print(currentTemperature3); Serial.println(" C");

  delay(500);
}

void buzzerBeep(int beeps) {
  for(int x = 0; x < beeps; x++){
    digitalWrite(buzzPin, HIGH);
    delay(1000); // Wait for 1 second with the buzzer ON
    digitalWrite(buzzPin, LOW);
    delay(1000); // Wait for 1 second with the buzzer OFF
  }
}

void stopBeep() {
  if (digitalRead(buttonPin) == LOW) { // If the button is pressed
    buzzerMuted = true;
    muteStartTime = millis(); // Record the time the buzzer was muted
  }

  if (buzzerMuted && (millis() - muteStartTime >= muteDuration)) {
    buzzerMuted = false; // Unmute the buzzer after 5 minutes
  }
}

void displayLCD(){
  lcd.setCursor(0,0);
  lcd.print("NO. NOW   LOW   HIGH");

  // row 2
  lcd.setCursor(0, 1); lcd.print("T1: "); lcd.print(currentTemperature1, 1);
  lcd.print("  "); lcd.print(low1, 1); lcd.print("  "); lcd.print(high1, 1);

  // row 3
  lcd.setCursor(0, 2); lcd.print("T2: "); lcd.print(currentTemperature2, 1);
  lcd.print("  "); lcd.print(low2, 1); lcd.print("  "); lcd.print(high2, 1);

  // row 4
  lcd.setCursor(0, 3); lcd.print("T3: "); lcd.print(currentTemperature3, 1); lcd.print("  ");
  lcd.print(low3, 1); lcd.print("  "); lcd.print(high3, 1);
}

float findMinTemperature(int num) {
  float* mintemperatureReadings;

  // Select the appropriate array based on the num parameter
  switch (num) {
  case 1:
    mintemperatureReadings = MinTemperatureReadings1;
    break;
  case 2:
    mintemperatureReadings = MinTemperatureReadings2;
    break;
  case 3:
    mintemperatureReadings = MinTemperatureReadings1;
    break;
  default:
    break;
}

  float minTemp = mintemperatureReadings[0];

  for (int i = 1; i < numReadings; i++) {
    if (mintemperatureReadings[i] < minTemp) {
      minTemp = mintemperatureReadings[i];
    }
  }
  return minTemp;
}

float findMaxTemperature(int num) {
  float* maxtemperatureReadings;

  // Select the appropriate array based on the num parameter
  switch (num) {
    case 1:
      maxtemperatureReadings = MaxTemperatureReadings1;
      break;
    case 2:
      maxtemperatureReadings = MaxTemperatureReadings2;
      break;
    case 3:
      maxtemperatureReadings = MaxTemperatureReadings3;
      break;
    default:
      // Return a sentinel value or handle the error appropriately
      return -1;
  }
  // Initialize maxTemp with the first value in the selected array
  float maxTemp = maxtemperatureReadings[0];

  // Iterate through the array to find the maximum temperature
  for (int i = 1; i < numReadings; i++) {
    if (maxtemperatureReadings[i] > maxTemp) {
      maxTemp = maxtemperatureReadings[i];
    }
  }
  return maxTemp;
}
