#include <Arduino.h>

// Pin definitions
const int ecgPin = A0;   // ECG sensor connected to A0
const int pulsePin = A1; // Pulse sensor connected to A1
const int lm35Pin = A2;  // LM35 temperature sensor connected to A2

// Moving average filter for ECG
const int numEcgReadings = 10; // Number of readings for moving average
int ecgReadings[numEcgReadings];
int ecgReadIndex = 0;
int ecgTotal = 0;
int ecgAverage = 0;

// Low-pass filter parameters
float alpha = 0.1; // Adjust alpha for smoothing (0 < alpha < 1)
float previousPulseOutput = 0;
float previousTemperatureOutput = 0;

// Calibration values (adjust if necessary)
const float temperatureCalibrationOffset = 0.0;
const float pulseCalibrationOffset = 0.0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting Pulse, Temperature, and ECG Sensor monitoring...");

  // Initialize the ECG readings array
  for (int i = 0; i < numEcgReadings; i++)
  {
    ecgReadings[i] = 0; // Set initial ECG readings to zero
  }
}

// Function for low-pass filtering
float lowPassFilter(float input, float previousOutput, float alpha)
{
  return alpha * input + (1 - alpha) * previousOutput;
}

void loop()
{
  // Read the ECG sensor value
  int ecgRawValue = analogRead(ecgPin);

  // Update moving average filter for ECG
  ecgTotal -= ecgReadings[ecgReadIndex];
  ecgReadings[ecgReadIndex] = ecgRawValue;
  ecgTotal += ecgReadings[ecgReadIndex];
  ecgReadIndex = (ecgReadIndex + 1) % numEcgReadings;
  ecgAverage = ecgTotal / numEcgReadings;

  // Read the Pulse sensor value
  int pulseRawValue = analogRead(pulsePin);
  float calibratedPulseValue = pulseRawValue + pulseCalibrationOffset;
  float filteredPulseValue = lowPassFilter(calibratedPulseValue, previousPulseOutput, alpha);
  previousPulseOutput = filteredPulseValue;

  // Read the LM35 temperature sensor value
  int lm35RawValue = analogRead(lm35Pin);
  float voltage = lm35RawValue * (3.3 / 1023.0); // Convert to voltage
  float calibratedTemperatureC = (voltage * 100.0) + temperatureCalibrationOffset;
  float filteredTemperatureC = lowPassFilter(calibratedTemperatureC, previousTemperatureOutput, alpha);
  previousTemperatureOutput = filteredTemperatureC;

  // Print the sensor values
  Serial.print("Raw ECG Value: ");
  Serial.print(ecgRawValue);
  Serial.print(" | Filtered ECG Value: ");
  Serial.print(ecgAverage);
  Serial.print(" | Filtered Pulse Value: ");
  Serial.print(filteredPulseValue);
  Serial.print(" BPM | Filtered Temperature: ");
  Serial.print(filteredTemperatureC);
  Serial.println(" °C");

  delay(1000); // Delay before the next reading
}