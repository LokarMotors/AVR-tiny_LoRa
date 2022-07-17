# AVR-tiny_LoRa
# LoRaWAN node based on ATTiny87/167 and RFM95 #

Small LoRaWAN node with a temperature, air pressure and soil moisture sensor. Code includes modified Adafruit BMP180, LoRaWAN, RFM95 and custom Cayenne libraries to save flash. Circuit in made in EAGLE and includes a few avr circuit design considerations. ATTiny chip is programmed via ISP pins. Measured current consumption in deep sleep is 5μA at 3.3V and drops down to 3.9μA at 1.8V. Wake current consumption is 96.4mA at 3.3V and 79.5mA at 1.8V. Because of the RFM95 transmitter the maximum voltage is capped at 3.7V and the system is supplied with two AA batteries. According to simulations i made in Matlab the system could for 8 years in normal temperature conditions and uplinks every 30min. If there is a possibility to switch the RFM95 for a transmitter that is 5V tolerant the life expectancy of the device would be around 12 years with the same conditions with three AA batteries. Battery life is also effected by the spreading factor, table below shows wake time and battery life vs spreading factor.  


## Spread factor table (125k bandwith)##

| Spreading factor  | Wake time | Battery life |
| ------------- | ------------- | ------------- |
| SF7 | 61 ms | 8.9 years |
| SF8 | 61 ms | 8.9 years |
| SF9 | 208 ms | 8.6 years |
| SF10 | 411 ms | 7.8 years |
| SF11 | 823 ms | 6.2 years |
| SF12 | 1483 ms | 4.2 years |
