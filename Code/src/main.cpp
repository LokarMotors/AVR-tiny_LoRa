#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <LoRaWAN.h>
#include <CAY_DIY.h>
#include <Adafruit_BMP085.h>
#include "config.h"

// RFM95 pins
#define DIO0 9
#define NSS 11

#define SLEEP_TIME 180 // ~10s per cycle 180 ----> 30min

// SOIL MOISTURE pins
#define V_CLK 7 // CLOCK pin
#define AVLA 14 // ANALOG pin ADC9

// SOIL MOISTURE SENSOR SETTINGS:
#define MOIST_W 179 // WET
#define MOIST_D 222 // DRY
#define MAX_3V3 228 // MAX ANALOG VALUE AT 3.3 volts

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

RFM95 rfm(DIO0, NSS);
LoRaWAN lora = LoRaWAN(rfm);
uint16_t Frame_Counter_Tx = 0x0000;

// Global Variable to Track Deep Sleep
volatile uint16_t sleep_count = SLEEP_TIME; // Send first message at power on

// Custom CAYENNE library
CAYDIY cayenne(3);

// Modified Adafruit BMP180 library
Adafruit_BMP085 sensor;
int16_t dry, wet;

void goToSleep();
int16_t GetMoisture();

void setup()
{
  lora.setKeys(NwkSkey, AppSkey, DevAddr);
  rfm.init();
  rfm.RFM_Set_Datarate(SF10BW125);
}

void loop()
{
  if (sleep_count >= SLEEP_TIME)
  {
    ADCSRA |= (1 << ADEN); // Enable ADC
    pinMode(V_CLK, OUTPUT);
    digitalWrite(V_CLK, LOW); // Discharging soil moisture capacitor
    if (sensor.begin(BMP085_ULTRALOWPOWER))
    {
      cayenne.Index = 0;
      cayenne.Add(0x01, 0x73, sensor.readPressure() / 10);
      // Saving space instead of calling sensor.readTemperature();
      int32_t X1 = (sensor.readRawTemperature() - (int32_t)sensor.ac6) * (int32_t)sensor.ac5 >> 15;
      int32_t X2 = ((int32_t)sensor.mc << 11) / (X1 + (int32_t)sensor.md);
      ///////////////////////////////////
      cayenne.Add(0x02, 0x67, (((X1 + X2 + 8) >> 4)));
      cayenne.Add(0x03, 0x02, map(GetMoisture(), MOIST_D, MOIST_W, 0, 10000));
    }
    lora.Send_Data(cayenne.Buffer, cayenne.Index, ++Frame_Counter_Tx);
    sleep_count = 0;
  }
  goToSleep();
}

void goToSleep()
{
  // Disable ADC, saves ~230uA
  ADCSRA &= ~(1 << ADEN);

  // setup watchdog "9" -> 8 sec
  MCUSR &= ~(1 << WDRF);
  WDTCR |= (1 << WDCE) | (1 << WDE);
  WDTCR = (9 & 7) | (1 << 5) | (1 << WDCE);
  WDTCR |= _BV(WDIE);
  ///////////////////////////////////

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
  wdt_disable();
}

ISR(WDT_vect)
{
  sleep_count++; // keep track of how many sleep cycles have been completed.
}

int16_t GetMoisture()
{
  ADMUX &= 0b11000000; // clear last six bits
  ADMUX |= 0b00101001; // ADC9 and left adjust resaults for only 8-bit reading

  for (uint8_t i = 0; i < 255; i++)
  {
    ADCSRA |= (1 << ADSC); // first few measurements are not reliable
    delayMicroseconds(1);
    digitalWrite(V_CLK, HIGH);
    delayMicroseconds(1);
    digitalWrite(V_CLK, LOW);
  }
  ADCSRA |= (1 << ADSC);        // start conversion
  while (!(ADCSRA & 1 << ADIF)) // wait for conversion to finish
    ;
  int16_t out = (int16_t)ADCH;

  // compensation for battery discharge
  digitalWrite(V_CLK, HIGH);
  delay(15);
  ADCSRA |= (1 << ADSC);        // measure high voltage
  while (!(ADCSRA & 1 << ADIF)) // wait for conversion to finish
    ;
  out += (int16_t)(MAX_3V3 - ADCH); // compensate battery discharge
  digitalWrite(V_CLK, LOW);
  ///////////////////////////////////

  return out;
}
