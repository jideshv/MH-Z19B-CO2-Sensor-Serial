// Copyright (c) 2019 Jidesh Veeramachaneni
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).

#pragma once

/**
 * The MHZ19BCO2SensorSerial class reads and controls
 * the MH-Z19B CO2 module by Winsen.
 *
 * Datasheet: https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf
 *
 * @tparam MHZ19UARTSerial Use USARTSerial on Particle platforms. See example.
 */
template <typename MHZ19UARTSerial>
class MHZ19BCO2SensorSerial
{
public:

  /**
   * Construct the MHZ19BCO2SensorSerial with the Serial object you want to use.
   *
   * @param serial the Serial object you want to use.
   */
  MHZ19BCO2SensorSerial(MHZ19UARTSerial& serial) : m_serial(serial) {
    m_serial.template begin(9600,SERIAL_8N1);
  }

  ~MHZ19BCO2SensorSerial() {
    m_serial.template end();
  }

  /**
   * Reads the current CO2 level from the sensor.
   *
   * Returns -1 if either a timeout or checksum didn't match.
   */
  int Read() {
    unsigned long startTime = millis();

    int reading = -1;
    const uint8_t CMD86[9] = {0xFF,0x01,0x86,0,0,0,0,0,0x79};
    uint8_t checksum = 0;
    int recieved[9] = {0};
    int recieved_bytes = 0;

    m_serial.template write(CMD86, 9);

    while ((recieved_bytes < 9) && ((millis() - startTime) < 500)) {
      if (m_serial.template available() > 0) {
        recieved[recieved_bytes] = m_serial.template read();
        if (recieved_bytes == 8) {
          checksum = 0xFF - checksum;
          checksum += 1;
          if (checksum == recieved[recieved_bytes]) {
            reading = ((recieved[2]*256) +recieved[3]);
          }
        } else if (recieved_bytes > 0){
          checksum += recieved[recieved_bytes];
        }
        recieved_bytes++;
      }
    }

    return reading;
  }

  /**
   * Allows you to calibrate the "Zero Point".
   *
   * Leave the sensor in outdoor air (~400ppm) for at least 20 minutes,
   * then call SetZeroPoint. For most cases you can just keep ABC on
   * and therefore this is not necessary.
   */
  void SetZeroPoint() {
    const uint8_t CMD87[9] = {0xFF,0x01,0x87,0,0,0,0,0,0x78};

    m_serial.template write(CMD87, 9);
  }

  /**
   * Enable ABC (refer to the datasheet)
   *
   * Default when power on. So only need to call this if you called SetABCOff().
   */
  void SetABCOn() {
    const uint8_t CMD79[9] = {0xFF,0x01,0x79,0xA0,0,0,0,0,0xE6};

    m_serial.template write(CMD79, 9);
  }

  /**
   * Disable ABC (refer to the datasheet)
   */
  void SetABCOff() {
    const uint8_t CMD79[9] = {0xFF,0x01,0x79,0,0,0,0,0,0x86};

    m_serial.template write(CMD79, 9);
  }

private:
  MHZ19UARTSerial& m_serial;
};
