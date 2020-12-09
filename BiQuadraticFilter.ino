#include "BIQUADFILTER.h"

BiQuadFilter IIR;

void setup() {
  Serial.begin(115200);
  IIR.Settings(15, 50, LPF); //15HZ DE CORTE / 50HZ DO LOOP / LPF:LOW-PASS-FILTER
}

void loop() {
  delay(20); //50HZ LOOP
  Serial.print("Analog: ");    Serial.print(analogRead(0));
  Serial.print(" Filtered: "); Serial.print(IIR.FilterOutput(analogRead(0)));
  Serial.print("\n\t\r");
}
