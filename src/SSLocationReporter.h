#ifndef SSLREPORTER_H
#define SSLREPORTER_H

//-------------------------------------------------------------------

#include <AssetTracker.h>
#include "UVSensor.h"

//-------------------------------------------------------------------

class SSLocationReporter {
   enum State { S_Wait, S_Publish, S_LedNotify };

private:
    int rate;

private:
    State state;
    int tick;
    int led;
    AssetTracker& gpsSensor;
    UVSensor& uvSensor;

public:
    SSLocationReporter(AssetTracker &theTracker, UVSensor &theDetector);
    void execute();
};

//-------------------------------------------------------------------

#endif
