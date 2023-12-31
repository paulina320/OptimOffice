#ifndef __NOISE__
#define __NOISE__

#define sampleWindow 1000
#define NOISEAVG_FREQUENCY 2 * 60 * 1000
#define micPin 36 // connect mic sensor to Analog 0

class Noise
{
public:
    void readAudio(void);
    bool interpretNoise(void);
    float lastNoiseValue = 0;

private:
    unsigned int audioSignalAVG = 0;
    unsigned int audioSignalCount = 0;
    unsigned int startMillis = 0;
    unsigned long noiseSignalMillis = 0;
    unsigned int sample = 0;
    bool isboot = 1;
};

extern Noise noise;

#endif