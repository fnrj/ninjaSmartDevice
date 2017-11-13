// This #include statement was automatically added by the Particle IDE.
#include <AssetTracker.h>

// Author: Kyle Kuan
#include "SSLocationReporter.h"
#include "UVSensor.h"
// VEML define
#include <application.h>
//#include <spark_wiring_i2c.h>
//-------------------------------------------------------------------

using namespace std;

//-------------------------------------------------------------------

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

//-------------------------------------------------------------------

AssetTracker locationTracker = AssetTracker();
UVSensor uvSensor(/*veml6070,*/ 5, 3, 0);
SSLocationReporter sslocationReporter(locationTracker, uvSensor);

//-------------------------------------------------------------------

void executeStateMachines() {
    locationTracker.updateGPS();
    uvSensor.execute();
    sslocationReporter.execute();
}

Timer stateMachineTimer(10, executeStateMachines);

//-------------------------------------------------------------------

void responseHandler(const char *event, const char *data) {
    // Formatting output
    String output = String::format("POST Response:\n  %s\n  %s\n", event, data);
    // Log to serial console
    Serial.println(output);
}

//-------------------------------------------------------------------

void setup() {
    Serial.begin(9600);

    // Initialize the gps and turn it on
    locationTracker.begin();
    locationTracker.gpsOn();


    // Initialize I2C and VEML6070 exposure time initialize with 0,1,2,3
    // Set variable
    //Particle.variable("i2cdevice", "VEML6070");
    //Particle.variable("uvlight", uvlight);

    // Initialise I2C communication as MASTER
    Wire.begin();
    // Initialise serial communication, set baud rate = 9600
    //Serial.begin(9600);

    // Start I2C Transmission
    Wire.beginTransmission(VEML6070_ADDR_L);
    // Select command register
    // Integration time = 4T, shutdown mode disable
    Wire.write((0x03)<<2|0x02);
    // Stop I2C Transmission
    Wire.endTransmission();
    delay(300);
    // Handler for response from POSTing location to server
    Particle.subscribe("hook-response/holz", responseHandler, MY_DEVICES);

    stateMachineTimer.start();
}

//-------------------------------------------------------------------

void loop() {

    // Request time synchronization from the Particle Cloud once per day
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }

    delay(10);
}

//-------------------------------------------------------------------
