#include "BIQUADFILTER.h"

void BiQuadFilter::Settings(int16_t CutOffFreq, int16_t SampleFreq, uint8_t FilterType) {
  float COF    = min(float(CutOffFreq), float((SampleFreq / 2) - 1)); //CALCULA A FREQUENCIA DE CORTE
  //GERA OS COEFICIENTES DO FILTRO BIQUADRATICO
  float Omega  = tan(3.1415927f * COF / SampleFreq);
  float Normal = 1.0f / (1.0f + Omega / 0.7071f + Omega * Omega);
  switch (FilterType) {

    case LPF:
      //CALCULO DE COEFICIENTE PARA FILTRO DO TIPO LOW PASS
      Coeff1   = int16_t(Omega * Omega * Normal * 16384.0f);
      Coeff2   = 2 * Coeff1;
      Coeff3   = Coeff1;
      Coeff4   = int16_t( 2.0f * (Omega * Omega - 1.0f) * Normal * 16384.0f);
      Coeff5   = int16_t((1.0f - Omega / 0.7071f + Omega * Omega) * Normal * 16384.0f);
      break;

    case HPF:
      //CALCULO DE COEFICIENTE PARA FILTRO DO TIPO HIGH PASS
      Coeff1 = 1 * Normal * 16384.0f;
      Coeff2 = -2 * Coeff1;
      Coeff3 = Coeff1;
      Coeff4 = 2 * (Omega * Omega - 1) * Normal * 16384.0f;
      Coeff5 = (1 - Omega / 0.7071f + Omega * Omega) * Normal * 16384.0f;
      break;

    case NOTCH:
      //CALCULO DE COEFICIENTE PARA FILTRO DO TIPO NOTCH
      Coeff1 = (1 + Omega * Omega) * Normal * 16384.0f;
      Coeff2 = 2 * (Omega * Omega - 1) * Normal * 16384.0f;
      Coeff3 = Coeff1;
      Coeff4 = Coeff2;
      Coeff5 = (1 - Omega / 0.7071f + Omega * Omega) * Normal * 16384.0f;
      break;
  }
}

int16_t BiQuadFilter::FilterOutput(int16_t DeviveToFilter) { //ENTRADA DO DISPOSITIVO PARA O FILTRO
  //UTILIZA A MULTIPLICAÇÃO AVR EM ASSEMBLY PARA PREVENIR ERROS DE ARREDONDAMENTO DE VALORES
  Result.LongValue =	mul(DeviveToFilter, Coeff1) + mul(GuardInput1, Coeff2) + mul(GuardInput2, Coeff3) -
                      mul(GuardOutput1,   Coeff4) - mul(GuardOutput2, Coeff5);
  Result.LongValue = Result.LongValue << 2;
  GuardInput2      = GuardInput1;
  GuardInput1      = DeviveToFilter;
  GuardOutput2     = GuardOutput1;
  GuardOutput1     = Result.ShortValue[1];
  return Result.ShortValue[1];
}

//http://mekonik.wordpress.com/2009/03/18/arduino-avr-gcc-multiplication/
#define MultiS16X16to32(longRes, intIn1, intIn2) \
  asm volatile ( \
                 "clr r26 \n\t" \
                 "mul %A1, %A2 \n\t" \
                 "movw %A0, r0 \n\t" \
                 "muls %B1, %B2 \n\t" \
                 "movw %C0, r0 \n\t" \
                 "mulsu %B2, %A1 \n\t" \
                 "sbc %D0, r26 \n\t" \
                 "add %B0, r0 \n\t" \
                 "adc %C0, r1 \n\t" \
                 "adc %D0, r26 \n\t" \
                 "mulsu %B1, %A2 \n\t" \
                 "sbc %D0, r26 \n\t" \
                 "add %B0, r0 \n\t" \
                 "adc %C0, r1 \n\t" \
                 "adc %D0, r26 \n\t" \
                 "clr r1 \n\t" \
                 : \
                 "=&r" (longRes) \
                 : \
                 "a" (intIn1), \
                 "a" (intIn2) \
                 : \
                 "r26" \
               )
int32_t inline BiQuadFilter::mul(int16_t a, int16_t b) {
  int32_t r;
  MultiS16X16to32(r, a, b);
  return r;
}
