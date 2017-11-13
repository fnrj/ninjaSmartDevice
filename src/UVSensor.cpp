//-------------------------------------------------------------------

#include "UVSensor.h"
//#include <AssetTracker.h>

//-------------------------------------------------------------------

UVSensor::UVSensor(int rate,
                   int samples,
                   float threshold) : uvSamples(samples) {

   this->rate = rate;
   this->samples = samples;
   this->threshold = threshold;
   tick = 0;
   sampleIndex = 0;
   UVDetected = false;
   state = S_Wait;
}
//#define Addr 0x38
//-------------------------------------------------------------------
void UVSensor::execute() {
   switch (state) {
      case UVSensor::S_Wait:
         ++tick;
         sampleIndex = 0;

         if (tick == rate) {
            state = UVSensor::S_Sample;
         }
         else {
            state = UVSensor::S_Wait;
         }
         break;

      case UVSensor::S_Sample:
         tick = 0;
         //uvSamples.at(sampleIndex) = readUVmagnitude();
         uvSamples.at(sampleIndex) = readUV();
         sampleIndex++;

         if (sampleIndex == samples) {
            state = UVSensor::S_Filter;
         }
         else {
            state = UVSensor::S_Sample;
         }
         break;

      case UVSensor::S_Filter:
         avgMagnitude = 0.0;
         for (int i = 0; i < samples; ++i) {
            avgMagnitude += static_cast<float>(uvSamples.at(i));
         }
         avgMagnitude = avgMagnitude / samples;

         if (avgMagnitude > threshold) {
            state = UVSensor::S_Detected;
         }
         else {
            state = UVSensor::S_Wait;
         }
         break;

      case UVSensor::S_Detected:
         UVDetected = true;
         state = UVSensor::S_WaitUntilReported;
         break;

      case UVSensor::S_WaitUntilReported:
         if (UVDetected == true) {
            state = UVSensor::S_WaitUntilReported;
         }
         else {
            state = UVSensor::S_Wait;
         }
         break;
   }
   Serial.println("UV Sensor state"+String(state));
}

//-------------------------------------------------------------------

bool UVSensor::isDetected() {
   return UVDetected;
}

//-------------------------------------------------------------------

void UVSensor::setReported() {
   UVDetected = false;
}

//-------------------------------------------------------------------
 // uint16_t UVSensor::readUVmagnitude() {
 //
 //   unsigned int data[2];
 //   double uvlight = 0.0;
 //
 //   // Start I2C Transmission
 //   Wire.beginTransmission(Addr);
 //   // Select data msb register
 //   Wire.write(0x73);
 //   // Stop I2C Transmission
 //   Wire.endTransmission();
 //
 //   // Request 1 byte of data
 //   Wire.requestFrom(Addr, 1);
 //   if(Wire.available() == 1)
 //   {
 //     data[0] = Wire.read();
 //   }
 //
 //   // Start I2C Transmission
 //   Wire.beginTransmission(Addr);
 //   // Select data lsb register
 //   Wire.write(0x71);
 //   // Stop I2C Transmission
 //   Wire.endTransmission();
 //   // Request 1 byte of data
 //   Wire.requestFrom(Addr, 1);
 //   if(Wire.available() == 1)
 //   {
 //     data[1] = Wire.read();
 //   }
 //
 //   // Convert the data
 //   uvlight = data[0] * 256.0 + data[1];
 //
 //   // Output data to dashboard
 //   Serial.println("UV Light of The Device (everything) : "+String(uvlight));
 //   return uvlight;
 //   delay(1000);
 //
 // }

 uint16_t UVSensor::readUV() {
  if (Wire.requestFrom(VEML6070_ADDR_H, 1) != 1) {Serial.println("UV Light of The Device (library) : Read high byte error"); return -1;}
  uint16_t uvi = Wire.read();
  uvi <<= 8;
  if (Wire.requestFrom(VEML6070_ADDR_L, 1) != 1) {Serial.println("UV Light of The Device (library) : Read low byte error"); return -1;}
  uvi |= Wire.read();
  Serial.println("UV Light of The Device (library) : "+String(uvi));
  return uvi;
  delay(1000);
}
