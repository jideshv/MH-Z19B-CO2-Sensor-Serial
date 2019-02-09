//
//  read_tests.cpp
//  gmock
//
//  Created by Jidesh V on 7/23/18.
//

#include <chrono>
#define SERIAL_8N1 1314
auto start = std::chrono::system_clock::now();

unsigned long millis () {
  return (unsigned long)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count());
}


#include "gtest/gtest.h"
#include "MockSerialInterface.h"
#include "MH-Z19B-CO2-Sensor-Serial.h"

using ::testing::Sequence;
using ::testing::Return;

TEST(MHZ19, construct_destruct) {
  MockSerial serial;
  EXPECT_CALL(serial, begin(9600,SERIAL_8N1)).Times(1);
  EXPECT_CALL(serial, end()).Times(1);
  MHZ19BCO2SensorSerial<MockSerial> mhz19b(serial);
}

class MHZ19_Base : public ::testing::Test {
protected:
  MHZ19_Base() {
    EXPECT_CALL(m_serial, begin(9600,SERIAL_8N1)).Times(1);
    m_mhz19b = std::make_shared<MHZ19BCO2SensorSerial<MockSerial>>(m_serial);
  }
  
  virtual ~MHZ19_Base() {
    EXPECT_CALL(m_serial, end()).Times(1);
  }
  
  uint8_t CheckSum (const uint8_t* data) {
    // direct copy from Winsen data sheet
    uint8_t checksum = 0;
    
    for (uint8_t i = 1; i < 8; i++) {
      checksum += data[i];
    }
    checksum = 0xFF - checksum;
    checksum += 1;
    return checksum;
  }

  MockSerial m_serial;
  std::shared_ptr<MHZ19BCO2SensorSerial<MockSerial>> m_mhz19b;
};

TEST_F(MHZ19_Base, Test_CheckSum) {
  const uint8_t TEST_DATA[9] = {0xFF,0x01,0x86,0,0,0,0,0,0x79};
  EXPECT_EQ(TEST_DATA[8], CheckSum(TEST_DATA));
}

class MHZ19_Read : public MHZ19_Base {
protected:
  const uint8_t CMD86[9] = {0xFF,0x01,0x86,0,0,0,0,0,0x79};

  static const int Cmd86_1500 (int index) {
    static const int a[] = {0xFF,0x86,0x05,0xDC,0,0,0,0,0x99};
    return a[index];
  }
};

MATCHER_P2(HasBytes, bytes, size, "") {
  const uint8_t * dataToCheck = arg;
  bool isMatch = (memcmp(dataToCheck, bytes, size) == 0);
  return isMatch;
}

TEST_F(MHZ19_Read, CMD86_TimeOut) {
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD86,9),9));
  EXPECT_CALL(m_serial, available()).WillRepeatedly(Return(0));
  EXPECT_EQ(-1, m_mhz19b->Read());
}

TEST_F(MHZ19_Read, CMD86_TimeOut_AllButOne) {
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD86,9),9));
  for (int i = 0; i < 8; i++) {
    EXPECT_CALL(m_serial, available()).InSequence(s).WillOnce(Return(8-i));
    EXPECT_CALL(m_serial, read()).InSequence(s).WillOnce(Return(Cmd86_1500(i)));
  }
  EXPECT_CALL(m_serial, available()).InSequence(s).WillRepeatedly(Return(0));
  EXPECT_EQ(-1, m_mhz19b->Read());
}

TEST_F(MHZ19_Read, CMD86_ChecksumFail) {
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD86,9),9));
  for (int i = 0; i < 8; i++) {
    EXPECT_CALL(m_serial, available()).InSequence(s).WillOnce(Return(8-i));
    EXPECT_CALL(m_serial, read()).InSequence(s).WillOnce(Return(Cmd86_1500(i)));
  }
  EXPECT_CALL(m_serial, available()).InSequence(s).WillOnce(Return(1));
  EXPECT_CALL(m_serial, read()).InSequence(s).WillOnce(Return(0xFF));
  EXPECT_EQ(-1, m_mhz19b->Read());
}

TEST_F(MHZ19_Read, CMD86_1500) {
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD86,9),9));
  for (int i = 0; i < 9; i++) {
    EXPECT_CALL(m_serial, available()).InSequence(s).WillOnce(Return(9-i));
    EXPECT_CALL(m_serial, read()).InSequence(s).WillOnce(Return(Cmd86_1500(i)));
  }
  EXPECT_EQ(1500,m_mhz19b->Read());
}

class MHZ19_Settings : public MHZ19_Base {};

TEST_F(MHZ19_Settings, SetZeroPoint) {
  uint8_t CMD87[9] = {0xFF,0x01,0x87,0,0,0,0,0,0};
  CMD87[8] = CheckSum(&CMD87[0]);
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD87,9),9));
  m_mhz19b->SetZeroPoint();
}

TEST_F(MHZ19_Settings, SetABCOn) {
  uint8_t CMD79[9] = {0xFF,0x01,0x79,0xA0,0,0,0,0,0};
  CMD79[8] = CheckSum(&CMD79[0]);
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD79,9),9));
  m_mhz19b->SetABCOn();
}

TEST_F(MHZ19_Settings, SetABCOff) {
  uint8_t CMD79[9] = {0xFF,0x01,0x79,0,0,0,0,0,0};
  CMD79[8] = CheckSum(&CMD79[0]);
  Sequence s;
  EXPECT_CALL(m_serial, write(HasBytes(CMD79,9),9));
  m_mhz19b->SetABCOff();
}
