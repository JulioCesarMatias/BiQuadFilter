#include "BIQUADFILTER.h"

static BiquadFilter_Struct Smooth_AnalogRead;

#define THIS_LOOP_FREQUENCY 1000 //FREQUENCIA DO LOOP INFINITO
#define FILTER_CUTOFF_FREQ 20 //20HZ

void setup() {
  Serial.begin(115200);
  BIQUADFILTER.Settings(&Smooth_AnalogRead, FILTER_CUTOFF_FREQ, 0, BIQUAD_SET_FREQUENCY(THIS_LOOP_FREQUENCY, "KHZ"), LPF);
}

void loop() {
  int16_t GetAnalogReadValue = analogRead(0); //PINO ANALOGICO A0
  Serial.print("RealAnalog:");
  Serial.print(GetAnalogReadValue);
  Serial.print(" AnalogFiltered:");
  Serial.println(BIQUADFILTER.FilterApplyAndGet(&Smooth_AnalogRead, GetAnalogReadValue));
  delay(1); //1KHZ LOOP
}
