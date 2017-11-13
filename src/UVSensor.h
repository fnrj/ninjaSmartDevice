#ifndef UVSENSOR_H
#define UVSENSOR_H

//-------------------------------------------------------------------

#include <vector>
#include <application.h>
//#include <spark_wiring_i2c.h>
#define VEML6070_ADDR_H 0x39
#define VEML6070_ADDR_L 0x38
//#include <AssetTracker.h>
//#include <Adafruit_VEML6070.h>
//-------------------------------------------------------------------

using namespace std;

//-------------------------------------------------------------------

class UVSensor {
   enum State { S_Wait, S_Sample, S_Filter, S_Detected, S_WaitUntilReported };

private:
    int rate;
    int samples;
    float threshold;

private:
    State state;
    int tick;
    int numSamples;
    vector<uint16_t> uvSamples;
    int sampleIndex;
    bool UVDetected;
    float avgMagnitude;
//    Adafruit_VEML6070& veml6070;

public:
    UVSensor(/*Adafruit_VEML6070 &thesensor,*/ int rate, int samples, float threshold);
    bool isDetected();
    void setReported();
    void execute();
    //uint16_t readUVmagnitude();
    uint16_t readUV(); //uv function from particle library
};

//-------------------------------------------------------------------

#endif
