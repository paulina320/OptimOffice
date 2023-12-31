#include <Arduino.h>
#include <noise.h>

void Noise::readAudio()
{
  startMillis = millis(); // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;    // minimum value
  unsigned int signalMin = 1024; // maximum value

  // collect data for sampleWindow mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(micPin); // get reading from microphone

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
  if (((millis() - noiseSignalMillis) > NOISEAVG_FREQUENCY) || isboot) // triggers the routine every 5 seconds
  {
    if (isboot)
      isboot = false;
    audioSignalAVG = audioSignalAVG / audioSignalCount;
    //lastNoiseValue = map(audioSignalAVG, 400, 1200, 20, 93); // calibrate for deciBels
    if(audioSignalAVG < 410)
      lastNoiseValue = map(audioSignalAVG,405,410,32,42);
    else if(audioSignalAVG < 422)
      lastNoiseValue = map(audioSignalAVG,410,422,42,50);
    else if(audioSignalAVG < 537)
      lastNoiseValue = map(audioSignalAVG,422,537,50,60);
    else if(audioSignalAVG < 790)
      lastNoiseValue = map(audioSignalAVG,537,790,60,70);
    else
      lastNoiseValue = map(audioSignalAVG,790,1100,70,79);
    
    log_d("Noise: %.2f\n", lastNoiseValue);
    audioSignalAVG = 0;
    audioSignalCount = 0;
    noiseSignalMillis = millis();
    return true;
  }
  return false;
}

Noise noise;
