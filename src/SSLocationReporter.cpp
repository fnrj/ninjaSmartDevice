//-------------------------------------------------------------------

#include "SSLocationReporter.h"
#include <AssetTracker.h>
//-------------------------------------------------------------------

SSLocationReporter::SSLocationReporter(AssetTracker &theTracker,
                                 UVSensor &theUV, vector<String>* queueData, String* postData) :
    gpsSensor(theTracker), uvSensor(theUV) {

    tick = 0;
    state = S_Wait;
    led = D7;
    pinMode(led, OUTPUT);
    this->queueData = queueData;
    this->postData = postData;
}

//-------------------------------------------------------------------
using namespace std;
void SSLocationReporter::execute() {
    //String postData;

    switch (state) {
        case SSLocationReporter::S_Wait:
            tick = 0;
            digitalWrite(led, LOW);

            if (uvSensor.isDetected()) {
                state = SSLocationReporter::S_DataPush;
            }
            else {
                state = SSLocationReporter::S_Publish;
            }
            break;
        case SSLocationReporter::S_DataPush:
            if (gpsSensor.gpsFix()) {
                *postData = String::format("{ \"longitude\": \"%f\", \"latitude\": \"%f\", \"uv\": \"%f\", \"loggedTime\": \"%s\"}",
                                      gpsSensor.readLatDeg(), gpsSensor.readLonDeg(),uvSensor.uvIndex,Time.format(TIME_FORMAT_ISO8601_FULL).c_str());
                //Serial.println("Push Time:");
                //Serial.println(Time.format(TIME_FORMAT_ISO8601_FULL).c_str());
                Serial.println(*postData);
                queueData->push_back(*postData);
            }
            else {
                Serial.println("DEBUG: No GPS fix");
                *postData = String::format("{ \"longitude\": \"%f\", \"latitude\": \"%f\"}",
                                      32.248814, -110.987419);
            }

            state = SSLocationReporter::S_Publish;
            break;
        case SSLocationReporter::S_Publish:
            if(WiFi.ready()&&Particle.connected()){
              Serial.println("Wifi is OK.");
              if(queueData->size()>0){
                Particle.publish("putStatus", queueData->front());
                Serial.println("Post data online...");
                queueData->erase(queueData->begin());
              }
            }
            else  {
                Serial.println("Wifi is down... Store in Photon super small storage...");
                if(queueData->size()>5000){ // no connection but buffer can't hold and it's over 24hrs. 24*60*3times/minute
                  queueData->erase(queueData->begin()); // clear the oldest
                }
            }
            Serial.println("Current queue size: "+String(queueData->size()));
            //Particle.publish("putStatus", postData);
            uvSensor.setReported();

            state = SSLocationReporter::S_LedNotify;
            break;

        case SSLocationReporter::S_LedNotify:
            if (uvSensor.uvIndex>2240){
              digitalWrite(led, HIGH);
            }
            ++tick;

            // Keep LED on for 2 seconds
            //if (tick == 200) {
            // Keep LED on for 10 seconds decrease request integration rate
            if (tick == 1000) {
                state = SSLocationReporter::S_Wait;
            }
            else {
                state = SSLocationReporter::S_LedNotify;
            }
            break;
    }
    //Serial.println("gps_state"+String(state));
}

//-------------------------------------------------------------------
