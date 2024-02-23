#ifndef CANMCP_H
#define CANMCP_H
	#include <stdlib.h>
	#define CS    2
	#define MOSI  3
	#define MISO  4
	#define SCK   5

	#define READ_STAT_Rx0IF		0b00000001
	#define READ_STAT_Rx1IF	 	0b00000010
	#define READ_STAT_Tx0REQ 	0b00000100
	#define READ_STAT_Tx0IF		0b00001000		
	#define READ_STAT_Tx1REQ	0b00010000
	#define READ_STAT_Tx1IF		0b00100000
	#define READ_STAT_Tx2REQ 	0b01000000
	#define READ_STAT_Tx2IF		0b10000000

	#define CAN_NORMAL_MODE     0b00000000
	#define CAN_SLEEP_MODE      0b00100000
	#define CAN_LOOPBACK_MODE   0b01000000
	#define CAN_LISTEN_MODE     0b01100000
	#define CAN_CONFIG_MODE     0b10000000
	
	#define CANTX0
	#define CANTX1
	#define CANTX2
	#define CANSTAT 0x0E
	
	#define RXB0 0
	#define RXB1 1
	
	//register list
	#define RXB0DLC	0x65
	#define RXB1DLC 0x75
	#define RXB0D0	0x66
	#define RXB1D0	0x76
	
	#define RXF0	0x00
	#define RXF1	0x04
	#define RXF2	0x08
	#define RXF3	0x10
	#define RXF4	0x14
	#define RXF5	0x18
	
	//#define 125_KBPS			0b00000001
	
	typedef struct canMsg{
		uint16_t CANID;
		uint8_t DLC;
		uint8_t dbyte[8];
		
	}CANMSG_T;
	
	//static void *canData = calloc(8,8);
	
/*
* for write the spi message to the MCP2515 CAN controller
*/
	void spiWrite(unsigned short data);

/*
	for reset the MCP2515 by sending reset instruction (0b11000000) via SPI
*/
	void MCP_RESET(void);
	
	short setCanMode(unsigned short mode);
	
	void setCanSpeed(unsigned short canSpeed);
	
/*
	Write byte to the given address register
*/
	void MCP_WRITE_BYTE(unsigned short addr, unsigned short data);
	
	unsigned short MCP_READ_BYTE(unsigned short addr);
	
	void MCP_BIT_MOD(unsigned short addr, unsigned short mask, unsigned short data);
	
/*
	Read MCP status 
	function return 8bit of data that respresent these following status
	- bit0: Rx0IF
	- bit1:	Rx1IF
	- bit2: Tx0REQ
	- bit3:	Tx0IF
	- bit4: Tx1REQ
	- bit5: Tx1IF
	- bit6: Tx2REQ
	- bit7: Tx2IF
*/
	unsigned short MCP_READ_STATUS(void);
	unsigned short getCanRxStat(void);
	
	void MCP_LOAD_TX_BUFFER(uint8_t* data, uint8_t dlc);
	unsigned short MCP_CAN_TRANSMIT(CANMSG_T *msg);
	
	//void *readCanRecieve(uint8_t rxBuffer);
	
	int getCanMessage(uint8_t bufferNumber,CANMSG_T* msg);
	int filterSetup(uint8_t recieveBufferNo,uint8_t filterNo, uint16_t id);
	
#endif