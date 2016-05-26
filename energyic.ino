 /* ATM90E26 Energy Monitor Demo Application

    The MIT License (MIT)

  Copyright (c) 2016 whatnick and Ryzee

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
 
 #include <SPI.h>
 #include "energyic.h"
 
 unsigned short CommEnergyIC(unsigned char RW,unsigned char address, unsigned short val){
	

	unsigned char* data=(unsigned char*)&val;
	unsigned short output;
  //SPI interface rate is 200 to 160k bps. It Will need to be slowed down for EnergyIC
  #if !defined(ENERGIA)
  SPISettings settings(200000, MSBFIRST, SPI_MODE3); 
  #endif	
	//switch MSB and LSB of value
	output=(val>>8)|(val<<8);
	val=output;
	
	//Set read write flag
	address|=RW<<7;
	
	//Transmit and receive data
  #if !defined(ENERGIA)
  SPI.beginTransaction(settings);
  #endif      
	digitalWrite (energy_CS,LOW);
        delayMicroseconds(10);
	SPI.transfer(address);
  /* Must wait 4 us for data to become valid */
  delayMicroseconds(4);

  //Read data
  //Do for each byte in transfer
  if(RW)
  {
    for (byte i=0; i<2; i++)
    {
      /* Transer the byte */
      *data = SPI.transfer (0x00);
      data++;
    }
  }
  else
  {
    for (byte i=0; i<2; i++)
    {
      /* Transer the byte */
      SPI.transfer(*data);             // write all the bytes
      data++;
    }
  }
  
	digitalWrite(energy_CS,HIGH);
  delayMicroseconds(10);
  #if !defined(ENERGIA)
  SPI.endTransaction();
  #endif
        
	output=(val>>8)|(val<<8); //reverse MSB and LSB
	return output;
}

double  GetLineVoltage(){
	unsigned short voltage=CommEnergyIC(1,Urms,0xFFFF);
	return (double)voltage/100;
}

unsigned short  GetMeterStatus(){
  return CommEnergyIC(1,EnStatus,0xFFFF);
}

double GetLineCurrent(){
	unsigned short current=CommEnergyIC(1,Irms,0xFFFF);
	return (double)current/1000;
}

double GetActivePower(){
	short int apower= (short int)CommEnergyIC(1,Pmean,0xFFFF); //Complement, MSB is signed bit
	return (double)apower;
}

double GetFrequency(){
	unsigned short freq=CommEnergyIC(1,Freq,0xFFFF);
	return (double)freq/100;
}

double GetPowerFactor(){
	short int pf= (short int)CommEnergyIC(1,PowerF,0xFFFF); //MSB is signed bit
	//if negative
	if(pf&0x8000){
		pf=(pf&0x7FFF)*-1;
	}
	return (double)pf/1000;
}

double GetImportEnergy(){
	//Register is cleared after reading
	unsigned short ienergy=CommEnergyIC(1,APenergy,0xFFFF);
	return (double)ienergy/10/1000; //returns kWh if PL constant set to 1000imp/kWh
}

double GetExportEnergy(){
	//Register is cleared after reading
	unsigned short eenergy=CommEnergyIC(1,ANenergy,0xFFFF);
	return (double)eenergy/10/1000; //returns kWh if PL constant set to 1000imp/kWh
}

unsigned short GetSysStatus(){
	return CommEnergyIC(1,SysStatus,0xFFFF);
}


void InitEnergyIC(){
	unsigned short systemstatus;
        
	pinMode(energy_IRQ,INPUT );
	pinMode(energy_CS,OUTPUT );
	pinMode(energy_WO,INPUT );
 
  /* Enable SPI */  
  SPI.begin();
  #if defined(ENERGIA)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE3);
    SPI.setClockDivider(SPI_CLOCK_DIV16);
  #endif

         
	CommEnergyIC(0,SoftReset,0x789A); //Perform soft reset
	CommEnergyIC(0,FuncEn,0x0030); //Voltage sag irq=1, report on warnout pin=1, energy dir change irq=0
	CommEnergyIC(0,SagTh,0x1F2F); //Voltage sag threshhold
		

	//Set metering calibration values
	CommEnergyIC(0,CalStart,0x5678); //Metering calibration startup command. Register 21 to 2B need to be set
	CommEnergyIC(0,PLconstH,0x00B9); //PL Constant MSB
	CommEnergyIC(0,PLconstL,0xC1F3); //PL Constant LSB
	CommEnergyIC(0,Lgain,0x1D39); 	//Line calibration gain
	CommEnergyIC(0,Lphi,0x0000); //Line calibration angle
	CommEnergyIC(0,PStartTh,0x08BD); //Active Startup Power Threshold
	CommEnergyIC(0,PNolTh,0x0000); //Active No-Load Power Threshold
	CommEnergyIC(0,QStartTh,0x0AEC); //Reactive Startup Power Threshold
	CommEnergyIC(0,QNolTh,0x0000); //Reactive No-Load Power Threshold
	CommEnergyIC(0,MMode,0x9422); //Metering Mode Configuration. All defaults. See pg 31 of datasheet.
	CommEnergyIC(1,CSOne,0x0000); //Checksum 1. Needs to be calculated based off the above values.


	//Set measurement calibration values
	CommEnergyIC(0,AdjStart,0x5678); //Measurement calibration startup command
	CommEnergyIC(0,Ugain,0x7A8E);    //Voltage rms gain
	CommEnergyIC(0,IgainL,0x2F6E);   //L line current gain
	CommEnergyIC(0,Uoffset,0x0000);  //Voltage offset
	CommEnergyIC(0,IoffsetL,0x0000); //L line current offset
	CommEnergyIC(0,PoffsetL,0x0000); //L line active power offset
	CommEnergyIC(0,QoffsetL,0x0000); //L line reactive power offset
	CommEnergyIC(1,CSTwo,0x0000);    //Checksum 2. Needs to be calculated based off the above values.
	
	//CommEnergyIC(0,CalStart,0x8765); //Checks correctness of 21-2B registers and starts normal metering if ok
	//CommEnergyIC(0,AdjStart,0x8765); //Checks correctness of 31-3A registers and starts normal measurement  if ok
	
	if (systemstatus&0xC000){
		//checksum 1 error
	}
	if (systemstatus&0x3000){
		//checksum 2 error
	}

}








