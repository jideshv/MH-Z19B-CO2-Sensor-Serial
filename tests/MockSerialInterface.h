//
//  MockSerialInterface.h
//
//
//  Created by Jidesh V on 7/23/18.
//
#ifndef MOCKSERIALINTERFACE_H
#define MOCKSERIALINTERFACE_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"

class IFakeSerial {
public:
  IFakeSerial() {}
  virtual ~IFakeSerial() {}
  
  virtual void begin(unsigned long, uint32_t) = 0;
  virtual void end(void) = 0;
  
  virtual int available(void) = 0;
  
  virtual int read(void) = 0;
  virtual size_t write(const uint8_t*, size_t) = 0;
};

class MockSerial : public IFakeSerial {
public:
  MOCK_METHOD2(begin, void(unsigned long, uint32_t));
  MOCK_METHOD0(end, void(void));
  
  MOCK_METHOD0(available, int(void));
  
  MOCK_METHOD0(read, int(void));
  MOCK_METHOD2(write, size_t(const uint8_t*, size_t));
};
#endif /* MOCKSERIALINTERFACE_H */
