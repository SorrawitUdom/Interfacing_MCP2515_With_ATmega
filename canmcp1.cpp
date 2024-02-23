#include <avr/io.h>
#include "canmcp1.h"

void spiWrite(unsigned short data){
	SPDR = data;
	while(!(SPSR & (1 << SPIF))) ;
}

void MCP_RESET(void){
	PORTB &= ~(1 << CS);
	spiWrite(0b11000000);
	PORTB |= (1 << CS);	
}

short setCanMode(unsigned short mode){
	short retVal = 0;
	uint16_t timeout = 0; 
	MCP_BIT_MOD(0x0F,0b11100000,mode);
	do{
		retVal = MCP_READ_BYTE(0x0E);
		timeout++;
	}while((retVal & 0b11100000 != mode) && timeout < 65535);
	if(timeout == 65535)
		return -1; 
	return retVal;
}

void setCanSpeed(unsigned short canSpeed){
	
}

void MCP_WRITE_BYTE(unsigned short addr, unsigned short data){
	PORTB &= ~(1 << CS);
	spiWrite(0x02);
	spiWrite(addr);
	spiWrite(data);
	PORTB |= (1 << CS); 
}

unsigned short MCP_READ_BYTE(unsigned short addr){
	unsigned short data = 0;
	PORTB &= ~(1 << CS);
	spiWrite(0x03);
	spiWrite(addr);
	spiWrite(0);
	data = SPDR;
	PORTB |= (1 << CS);
	return data;
}

void MCP_BIT_MOD(unsigned short addr, unsigned short mask, unsigned short data){
	PORTB &= ~(1 << CS);
	spiWrite(0x05);
	spiWrite(addr);
	spiWrite(mask);
	spiWrite(data);
	PORTB |= (1 << CS);   
}

unsigned short MCP_READ_STATUS(void){
	unsigned short data = 0;
	PORTB &= ~(1 << CS);
	spiWrite(0xA0);
	spiWrite(0);
	data = SPDR;
	PORTB |= (1 << CS);  
	return data;  
}

void MCP_LOAD_TX_BUFFER(uint8_t* data, uint8_t dlc){
	PORTB &= ~(1 << CS);
	spiWrite(0b01000001); // Load Command;
	
	for(int i = 0; i < dlc; i++){
		spiWrite(data[i]);
	} 
	
	PORTB |= (1 << CS);     
}

unsigned short MCP_CAN_TRANSMIT(CANMSG_T *msg){
	short stat = MCP_READ_STATUS();
	short ID_H;
	short ID_L;
	short SIDL_old;
	unsigned short dlc = msg->DLC;
	uint16_t timeout = 0;
	//unsigned short data = msg->dbyte[0];
	if(stat & READ_STAT_Tx0REQ){
		return -1;
	}
	ID_H = (msg->CANID) >> 3;
	ID_L = ((msg->CANID) & 7) << 5;
	
	MCP_WRITE_BYTE(0x31,ID_H);
	MCP_WRITE_BYTE(0x32,ID_L);
	MCP_WRITE_BYTE(0x35,dlc);
	
	MCP_LOAD_TX_BUFFER(msg->dbyte,dlc); //Load data to txBuffer;
	
	//request for transmit
	PORTB &= ~(1 << CS);
	spiWrite(0b10000001);
	PORTB |= (1 << CS); 
	
	return 0;
}

unsigned short getCanRxStat(void){
	unsigned short data = 0;
	PORTB &= ~(1 << CS);
	spiWrite(0b10110000);
	spiWrite(0);
	spiWrite(0);
	data = SPDR;
	PORTB |= (1 << CS);
	return data;	
	
}

int getCanMessage(uint8_t bufferNumber,CANMSG_T* msg){
	uint8_t canMessageControlBuffer[5];
	//uint8_t canMessageDataBuffer[8];
	uint8_t dlc;
	if(msg == NULL)
		return -1;
	PORTB &= ~(1 << CS);
	switch(bufferNumber){
		case RXB0 :
			spiWrite(0b10010000);
			break;
		case RXB1 :
			spiWrite(0b10010100);
			break;
		default :
			PORTB |= (1 << CS);
			return -1;
	}
	
	for(int i = 0; i < 5;i++){
		spiWrite(0);
		canMessageControlBuffer[i] = SPDR;
	}
	
	dlc = canMessageControlBuffer[4];
	
	for(int i = 0 ; i < dlc; i ++){
        spiWrite(0);
        msg->dbyte[i] = SPDR;
      }  
	PORTB |= (1 << CS);
    MCP_BIT_MOD(0x2C,1 << bufferNumber,0);
	
	msg->CANID = 0;
    msg->CANID = (canMessageControlBuffer[0] << 3);
    msg->CANID |= (canMessageControlBuffer[1] >> 5);	
	msg->DLC = dlc;
	return 0;
}

int filterSetup(uint8_t recieveBufferNo,uint8_t filterNo, uint16_t id){
	switch(recieveBufferNo){
		case RXB0:
			MCP_WRITE_BYTE(0x20,0b11111111);
			MCP_WRITE_BYTE(0x21,0b11100000);
		break;
		case RXB1:
			MCP_WRITE_BYTE(0x24,0b11111111);
			MCP_WRITE_BYTE(0x25,0b11100000);
		break;
		default:
			return -1;
	}
	
	MCP_WRITE_BYTE(filterNo,		(id >> 3));
	MCP_WRITE_BYTE(filterNo+0x01,	(id << 5));
	return id;
}



