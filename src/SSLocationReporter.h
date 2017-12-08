#ifndef SSLREPORTER_H
#define SSLREPORTER_H

//-------------------------------------------------------------------

#include <AssetTracker.h>
//#include <queue>
#include "UVSensor.h"

//-------------------------------------------------------------------

class SSLocationReporter {
   enum State { S_Wait, S_DataPush, S_Publish, S_LedNotify };

private:
    int rate;

private:
    State state;
    int tick;
    int led;
    AssetTracker& gpsSensor;
    UVSensor& uvSensor;

public:
    vector <String>* queueData;
    String* postData;
    SSLocationReporter(AssetTracker &theTracker, UVSensor &theDetector, vector <String>* queueData, String* postData);
    void execute();
};

//-------------------------------------------------------------------

#endif
