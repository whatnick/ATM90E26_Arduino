/* ATM90E26 Energy Monitor Demo Application

   The MIT License (MIT)

 Copyright (c) 2016 whatnick and Ryzee

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#include "energyic_SPI.h"
#include <SPI.h>

ATM90E26_SPI::ATM90E26_SPI(int pin) {
  _cs = pin;

  metering[_plconsth] = 0x00B9;
  metering[_plconstl] = 0xC1F3;
  metering[_lgain] = 0x1D39;
  metering[_lphi] = Lphi_Default;
  metering[_ngain] = Ngain_Default;
  metering[_nphi] = Nphi_Default;
  metering[_pstartth] = PStartTh_Default;
  metering[_pnolth] = PNolTh_Default;
  metering[_qstartth] = QStartTh_Default;
  metering[_qnolth] = QNolTh_Default;
  metering[_mmode] = MMode_Default;
  _crc1 = 0x4A34;

  measurement[_ugain] = 0xD464;
  measurement[_igain] = 0x6E49;
  measurement[_igainn] = IgainN_Default;
  measurement[_uoffset] = Uoffset_Default;
  measurement[_ioffestl] = IoffsetL_Default;
  measurement[_ioffsetn] = IoffsetN_Default;
  measurement[_poffestl] = PoffsetL_Default;
  measurement[_qoffsetl] = QoffsetL_Default;
  measurement[_poffsetn] = PoffsetN_Default;
  measurement[_qoffsetn] = QoffsetN_Default;
  _crc2 = 0xD294;
}
void ATM90E26_SPI::SetLGain(unsigned short lgain) { metering[_lgain] = lgain; }
void ATM90E26_SPI::SetUGain(unsigned short ugain) {
  measurement[_ugain] = ugain;
}
void ATM90E26_SPI::SetIGain(unsigned short igain) {
  measurement[_igain] = igain;
}

unsigned short ATM90E26_SPI::CalcCheckSum(int checksum_id) {
  //#CS1: metering
  // L2C=MOD(H21+H22+...+H2B+L21+L22+...+L2B, 2^8)
  // H2C=H21 XOR H22 XOR ... XOR H2B XOR L21 XOR L22 XOR ... XORL2
  if (checksum_id == 1) {
    unsigned char l2c = 0;
    unsigned char h2c = 0;
    for (int i = 0; i < 11; i++) {
      l2c += metering[i];
      l2c += metering[i] >> 8;
      h2c ^= metering[i];
      h2c ^= metering[i] >> 8;
    }
    return ((unsigned short)h2c << 8) | l2c;
  }
  //#CS2: measurement
  // L3B=MOD(H31+H32+...+H3A+L31+L32+...+L3A, 2^8)
  // H3B=H31 XOR H32 XOR ... XOR H3A XOR L31 XOR L32 XOR ... XORL3A
  else if (checksum_id == 2) {
    unsigned char l3b = 0;
    unsigned char h3b = 0;
    for (int i = 0; i < 10; i++) {
      l3b += measurement[i];
      l3b += measurement[i] >> 8;
      h3b ^= measurement[i];
      h3b ^= measurement[i] >> 8;
    }
    return ((unsigned short)h3b << 8) | l3b;
  }
  return 0;
}

unsigned short ATM90E26_SPI::CommEnergyIC(unsigned char RW,
                                          unsigned char address,
                                          unsigned short val) {

  unsigned char *data = (unsigned char *)&val;
  unsigned short output;
// SPI interface rate is 200 to 160k bps. It Will need to be slowed down for
// EnergyIC
#if !defined(ENERGIA) && !defined(ESP8266) && !defined(ARDUINO_ARCH_SAMD)
  SPISettings settings(200000, MSBFIRST, SPI_MODE3);
#else
  SPISettings settings(200000, MSBFIRST, SPI_MODE3);
#endif

  // switch MSB and LSB of value
  output = (val >> 8) | (val << 8);
  val = output;

  // Set read write flag
  address |= RW << 7;

  // Transmit and receive data
#if !defined(ENERGIA)
  SPI.beginTransaction(settings);
#endif
  // Disable LoRa chip on M0-LoRa
  digitalWrite(8, HIGH);
  digitalWrite(_cs, LOW);
  delayMicroseconds(10);
  SPI.transfer(address);
  /* Must wait 4 us for data to become valid */
  delayMicroseconds(4);

  // Read data
  // Do for each byte in transfer
  if (RW) {
    for (byte i = 0; i < 2; i++) {
      *data = SPI.transfer(0x00);
      data++;
    }
    // Transfer16 is not valid on Energia
    // val = SPI.transfer16(0x00);
  } else {
    for (byte i = 0; i < 2; i++) {
      SPI.transfer(*data); // write all the bytes
      data++;
    }
    // Transfer16 is not valid on Energia
    // SPI.transfer16(val);
  }

  digitalWrite(_cs, HIGH);
  // Reenable LoRa chip on M0-LoRa
  digitalWrite(8, LOW);
  delayMicroseconds(10);
#if !defined(ENERGIA)
  SPI.endTransaction();
#endif

  output = (val >> 8) | (val << 8); // reverse MSB and LSB
  return output;
  // Use with transfer16
  // return val;
}

double ATM90E26_SPI::GetLineVoltage() {
  unsigned short voltage = CommEnergyIC(1, Urms, 0xFFFF);
  return (double)voltage / 100;
}

unsigned short ATM90E26_SPI::GetMeterStatus() {
  return CommEnergyIC(1, EnStatus, 0xFFFF);
}

double ATM90E26_SPI::GetLineCurrent() {
  unsigned short current = CommEnergyIC(1, Irms, 0xFFFF);
  return (double)current / 1000;
}

double ATM90E26_SPI::GetActivePower() {
  short int apower = (short int)CommEnergyIC(
      1, Pmean, 0xFFFF); // Complement, MSB is signed bit
  return (double)apower;
}

double ATM90E26_SPI::GetReactivePower() {
  short int apower = (short int)CommEnergyIC(
      1, Qmean, 0xFFFF); // Complement, MSB is signed bit
  return (double)apower;
}

double ATM90E26_SPI::GetApparentPower() {
  short int apower = (short int)CommEnergyIC(
      1, Smean, 0xFFFF); // Complement, MSB is signed bit
  return (double)apower;
}

double ATM90E26_SPI::GetPhaseAngle() {
  short int apower = (short int)CommEnergyIC(
      1, Pangle, 0xFFFF); // Complement, MSB is signed bit
  return (double)apower;
}

double ATM90E26_SPI::GetFrequency() {
  unsigned short freq = CommEnergyIC(1, Freq, 0xFFFF);
  return (double)freq / 100;
}

double ATM90E26_SPI::GetPowerFactor() {
  short int pf = (short int)CommEnergyIC(1, PowerF, 0xFFFF); // MSB is signed
                                                             // bit
  // if negative
  if (pf & 0x8000) {
    pf = (pf & 0x7FFF) * -1;
  }
  return (double)pf / 1000;
}

double ATM90E26_SPI::GetImportEnergy() {
  // Register is cleared after reading
  unsigned short ienergy = CommEnergyIC(1, APenergy, 0xFFFF);
  return (double)ienergy *
         0.0001; // returns kWh if PL constant set to 1000imp/kWh
}

double ATM90E26_SPI::GetExportEnergy() {
  // Register is cleared after reading
  unsigned short eenergy = CommEnergyIC(1, ANenergy, 0xFFFF);
  return (double)eenergy *
         0.0001; // returns kWh if PL constant set to 1000imp/kWh
}

unsigned short ATM90E26_SPI::GetSysStatus() {
  return CommEnergyIC(1, SysStatus, 0xFFFF);
}

void ATM90E26_SPI::CalibrateEnergyIC() {
  unsigned short systemstatus;

  // Calculate checksums
  _crc1 = CalcCheckSum(1);
  _crc2 = CalcCheckSum(2);

  // Set metering calibration values
  CommEnergyIC(0, CalStart, 0x5678); // Metering calibration startup command.
                                     // Register 21 to 2B need to be set
  CommEnergyIC(0, PLconstH, metering[_plconsth]); // PL Constant MSB
  CommEnergyIC(0, PLconstL, metering[_plconstl]); // PL Constant LSB
  CommEnergyIC(0, Lgain, metering[_lgain]);       // Line calibration gain
  CommEnergyIC(0, CSOne, _crc1); // Write CSOne, as self calculated

  Serial.print("Checksum 1:");
  Serial.println(
      CommEnergyIC(1, CSOne, 0x0000),
      HEX); // Checksum 1. Needs to be calculated based off the above values.

  // Set measurement calibration values
  CommEnergyIC(
      0, AdjStart,
      0x5678); // Measurement calibration startup command, registers 31-3A
  CommEnergyIC(0, Ugain, measurement[_ugain]);  // Voltage rms gain
  CommEnergyIC(0, IgainL, measurement[_igain]); // L line current gain
  CommEnergyIC(0, CSTwo, _crc2); // Write CSTwo, as self calculated

  Serial.print("Checksum 2:");
  Serial.println(
      CommEnergyIC(1, CSTwo, 0x0000),
      HEX); // Checksum 2. Needs to be calculated based off the above values.

  CommEnergyIC(0, CalStart, 0x8765); // Checks correctness of 21-2B registers
                                     // and starts normal metering if ok
  CommEnergyIC(0, AdjStart, 0x8765); // Checks correctness of 31-3A registers
                                     // and starts normal measurement  if ok

  systemstatus = GetSysStatus();

  if (systemstatus & 0xC000) {
    // checksum 1 error
    Serial.println("Checksum 1 Error!!");
  }
  if (systemstatus & 0x3000) {
    // checksum 2 error
    Serial.println("Checksum 2 Error!!");
  }
}

void ATM90E26_SPI::InitEnergyIC() {

  // pinMode(energy_IRQ,INPUT );
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  delay(10);
  // pinMode(energy_WO,INPUT );

  /* Enable SPI */
  SPI.begin();
#if defined(ENERGIA)
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
#endif

  CommEnergyIC(0, SoftReset, 0x789A); // Perform soft reset
  CommEnergyIC(0, FuncEn, 0x0030);    // Voltage sag irq=1, report on warnout
                                      // pin=1, energy dir change irq=0
  CommEnergyIC(0, SagTh, 0x1F2F);     // Voltage sag threshhold

  CalibrateEnergyIC();
}
