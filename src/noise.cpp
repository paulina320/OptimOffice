#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <noise.h>

Adafruit_ADS1115 ads1115;

void Noise::initNoise(void)
{
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV)");
  ads1115.begin();
}

void Noise::readAudio()
{
  startMillis = millis(); // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;    // minimum value
  unsigned int signalMin = 1024; // maximum value

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = ads1115.readADC_SingleEnded(0); // get reading from microphone

    if (sample > signalMax)
      signalMax = sample; // save just the max levels
    else if (sample < signalMin)
      signalMin = sample; // save just the min levels
  }
  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
  audioSignalAVG += peakToPeak;
  audioSignalCount++;
}

bool Noise::interpretNoise(void)
{
  if ((millis() - noiseSignalMillis) > NOISEAVG_FREQUENCY) // triggers the routine every 5 seconds
  {
    audioSignalAVG = audioSignalAVG / audioSignalCount;
    lastNoiseValue = map(audioSignalAVG, 400, 1200, 20, 93); // calibrate for deciBels
    audioSignalAVG = 0;
    audioSignalCount = 0;
    noiseSignalMillis = millis();
    return true;
  }
  return false;
}

Noise noise;