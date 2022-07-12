# ATTNode Firmware

Example Sensor Node Firmware with a BMP/E280 or SHT21 Sensor. Used Sensor has to be set via #define in the src/secconfig.h (HAS_BME280 or HAS_SHT21). Please change the values in src/secconfig.h to your Keys from https://console.thethingsnetwork.org. Device has to be set to ABP Mode and the Frame Counter has to be set to 16 Bit.

For a complete explanation of the Parameters possible in the secconfig.h see https://www.attno.de/sites/attnode/20-firmware

The code also supports the RFM69W wireless module instead of the RFM95W LoRa module. For this purpose you need a receiver. Example code for a receiver in python (tested on a Raspberry Pi, RFM69 connected via SPI) can be found at https://www.seiichiro0185.org/git/IOT/sensord

Project was created using PlatformIO Atmel-AVR Framework

## Decoder for the TTN Application

```
function Decoder(bytes, port) {
  var decoded = {};

  if (bytes.length == 16) {
    // Old Payload Format
    decoded.t = ((bytes[0]) | (bytes[1] << 8 ) | (bytes[2] << 16 ) | (bytes[3] << 24)) / 100.0;
    decoded.p = ((bytes[4]) | (bytes[5] << 8 ) | (bytes[6] << 16 ) | (bytes[7] << 24)) / 100.0;
    decoded.h = ((bytes[8]) | (bytes[9] << 8 ) | (bytes[10] << 16 ) | (bytes[11] << 24)) / 100.0;
    decoded.v = ((bytes[12]) | (bytes[13] << 8 ) | (bytes[14] << 16 ) | (bytes[15] << 24)) / 1000.0;
  } else {
    // New Payload Format
    // We always have Battery Voltage (uint8_t)
    decoded.v = (bytes[0] * 20) / 1000.0;

    // Alarm Triggered (uint8_t)
    if (bytes.length == 2)
      decoded.a = bytes[1];

    // Temperature 2 * DS18B20
    if (bytes.length == 6){
      decoded.t1 = ((bytes[2]) | (bytes[3] << 8 )) / 100.0;
      decoded.t2 = ((bytes[4]) | (bytes[5] << 8 )) / 100.0;
      return decoded;
    }

    // Temperature (int32_t)
    if (bytes.length >= 5)
      decoded.t = ((bytes[1]) | (bytes[2] << 8 ) | (bytes[3] << 16 ) | (bytes[4] << 24)) / 100.0;
  
    // Humidity (int32_t)
    if (bytes.length >= 9)
      decoded.h = ((bytes[5]) | (bytes[6] << 8 ) | (bytes[7] << 16 ) | (bytes[8] << 24)) / 100.0;

    // Alarm Triggered (uint8_t)
    if (bytes.length == 10)
      decoded.a = bytes[9];

    // SHT21 + Brightness (int16_t)
    if (bytes.length == 11)
      decoded.b = ((bytes[9]) | (bytes[10] << 8 ));

    // Atmospheric Pressure (int32_t)
    if (bytes.length >= 13)
      decoded.p = ((bytes[9]) | (bytes[10] << 8 ) | (bytes[11] << 16 ) | (bytes[12] << 24)) / 100.0;

    // Alarm Triggered (uint8_t)
    if (bytes.length == 14)
      decoded.a = bytes[13];

    // BME280 + Brightness (int16_t)
    if (bytes.length == 15)
      decoded.b = ((bytes[13]) | (bytes[14] << 8 ));
  }

  return decoded;
}
```

## License
The firmware-code in this repository is licensed under the 3-clause BSD License (see LICENSE-File)
