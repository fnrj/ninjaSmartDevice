//-------------------------------------------------------------------

#include "SSLocationReporter.h"
#include <AssetTracker.h>

//-------------------------------------------------------------------

SSLocationReporter::SSLocationReporter(AssetTracker &theTracker,
                                 UVSensor &theUV) :
    gpsSensor(theTracker), uvSensor(theUV) {

    tick = 0;
    state = S_Wait;
    led = D7;
    pinMode(led, OUTPUT);
}

//-------------------------------------------------------------------

void SSLocationReporter::execute() {
    String postData;

    switch (state) {
        case SSLocationReporter::S_Wait:
            tick = 0;
            digitalWrite(led, LOW);

            if (uvSensor.isDetected()) {
                state = SSLocationReporter::S_Publish;
            }
            else {
                state = SSLocationReporter::S_Wait;
            }
            break;

        case SSLocationReporter::S_Publish:
            if (gpsSensor.gpsFix()) {
                postData = String::format("{ \"longitude\": \"%f\", \"latitude\": \"%f\", \"uv\": \"%f\" }",
                                          gpsSensor.readLatDeg(), gpsSensor.readLonDeg(),uvSensor.uvIndex);
            }
            else {
                Serial.println("DEBUG: No GPS fix");
                postData = String::format("{ \"longitude\": \"%f\", \"latitude\": \"%f\" }",
                                          32.248814, -110.987419);
            }

            Serial.println(postData);
            Particle.publish("putStatus", postData);
            uvSensor.setReported();

            state = SSLocationReporter::S_LedNotify;
            break;

        case SSLocationReporter::S_LedNotify:
            digitalWrite(led, HIGH);
            ++tick;

            // Keep LED on for 2 seconds
            if (tick == 200) {
                state = SSLocationReporter::S_Wait;
            }
            else {
                state = SSLocationReporter::S_LedNotify;
            }
            break;
    }
    Serial.println("gps_state"+String(state));
}

//-------------------------------------------------------------------
