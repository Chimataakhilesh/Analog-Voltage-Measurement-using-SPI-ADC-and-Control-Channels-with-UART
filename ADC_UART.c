#include<LPC21XX.H>
#include "lcd_header.h"

 //Typedef Definitions

typedef unsigned char u8;   // 8-bit unsigned
typedef unsigned int u32;   // 32-bit unsigned
typedef float f32;          // floating point


// Pin Definitions

#define CS 1<<7      // Chip Select for ADC (P0.7)
#define Rx 0x4       // UART RX pin
#define Tx 0x1       // UART TX pin
#define MISO 0x400   // SPI MISO
#define MOSI 0x1000  // SPI MOSI
#define CLK 0x100    // SPI Clock

#define MSTR_BIT 5   // SPI Master bit
#define SPIF_BIT 7   // SPI Transfer complete flag
#define Mode_3 3     // SPI Mode 3 (CPOL=1 CPHA=1)

#define SW 9         // Push button connected to P0.9


// Global Variables

u8 ch;       // stores UART channel input
f32 Volts;   // stores converted voltage value


// Function Prototypes

void LCD_STRING(u8 *);
void LCD_INTEGER(u32);
void LCD_FLOAT(f32);

void UART_INIT(void);
void UART_TX(u8);
void UART_STRING(u8 *);
u8 UART_RX(void);

void SPI_INIT(void);
u8 SPI(u8 data);

f32 Read_ADC(u8 channel);

void ch0(void);
void ch1(void);
void ch2(void);
void ch3(void);


 //Main Function

int main()
{
	u8 r = '\r';   // carriage return

	LCD_INIT();    // Initialize LCD
	UART_INIT();   // Initialize UART
	SPI_INIT();    // Initialize SPI

	while(1)
	{
		// Ask user for ADC channel through UART
		UART_STRING("ENTER THE CHANNEL NO");
		UART_TX(r);

		ch = UART_RX();   // receive channel number

		LCD_COMMAND(0x80);
		LCD_STRING("CHANNEL: ");

		// Select channel function
		switch(ch)
		{
			case '0': ch0();break;
			case '1': ch1();break;
			case '2': ch2();break;
			case '3': ch3();break;

			default:
			UART_STRING("Please enter correct option 0 - 3");
			break;
		}

		UART_TX(r);
		LCD_COMMAND(0x01); // clear LCD
	}
}


 //Channel 0 Reading

void ch0(void)
{
	LCD_INTEGER(ch-48); // convert ASCII to number

	while(1)
	{
		Volts = Read_ADC(0);  // read ADC channel 0

		LCD_COMMAND(0xc0);    // second line
		LCD_FLOAT(Volts);     // display voltage

		// Exit when switch pressed
		if(((IOPIN0>>SW)&1)==0)
		{
			while(((IOPIN0>>SW)&1)==0);
			break;
		}
	}
}


// Channel 1 Reading

void ch1(void)
{
	LCD_INTEGER(ch-48);

	while(1)
	{
		Volts = Read_ADC(1);

		LCD_COMMAND(0xc0);
		LCD_FLOAT(Volts);

		if(((IOPIN0>>SW)&1)==0)
		{
			while(((IOPIN0>>SW)&1)==0);
			break;
		}
	}
}


// Channel 2 Reading

void ch2(void)
{
	LCD_INTEGER(ch-48);

	while(1)
	{
		Volts = Read_ADC(2);

		LCD_COMMAND(0xc0);
		LCD_FLOAT(Volts);

		if(((IOPIN0>>SW)&1)==0)
		{
			while(((IOPIN0>>SW)&1)==0);
			break;
		}
	}
}

 //Channel 3 Reading

void ch3(void)
{
	LCD_INTEGER(ch-48);

	while(1)
	{
		Volts = Read_ADC(3);

		LCD_COMMAND(0xc0);
		LCD_FLOAT(Volts);

		if(((IOPIN0>>SW)&1)==0)
		{
			while(((IOPIN0>>SW)&1)==0);
			break;
		}
	}
}

/*-
 UART Initialization
 Baud Rate : 9600
*/
void UART_INIT(void)
{
	PINSEL0 |= Tx|Rx;   // select UART function

	U0LCR = 0x83;       // 8-bit, enable divisor latch
	U0DLL = 97;         // baud rate setting
	U0DLM = 0;

	U0LCR = 0x03;       // disable divisor latch
}


 //SPI Initialization (Master Mode)
	
void SPI_INIT(void)
{
	PINSEL0 |= CLK|MOSI|MISO; // select SPI pins

	IODIR0 |= CS;   // CS as output
	IOSET0 = CS;    // CS HIGH (inactive)

	S0SPCCR = 150;  // SPI clock prescaler

	S0SPCR = ((1<<MSTR_BIT)|Mode_3); // Master + Mode 3
}

// UART Transmit One Byte

void UART_TX(u8 txbyte)
{
	U0THR = txbyte;
	while(((U0LSR>>5)&1)==0); // wait until transmitted
}

 //UART Receive One Byte

u8 UART_RX(void)
{
	while((U0LSR&1)==0); // wait until data available
	return U0RBR;
}


 //Send String through UART

void UART_STRING(u8 *s)
{
	while(*s)
	{
		UART_TX(*s++);
	}
}


// SPI Data Transfer

u8 SPI(u8 data)
{
	S0SPDR = data;

	while(!(S0SPSR>>SPIF_BIT)&1); // wait until transfer complete

	return S0SPDR;
}


 //Read ADC via SPI

f32 Read_ADC(u8 channel)
{
	u8 hbyte,lbyte;
	u32 ADC_VAL;

	IOCLR0 = CS; // select ADC

	SPI(0x06);             // start bit
	hbyte = SPI(channel<<6);
	lbyte = SPI(0x00);

	IOSET0 = CS; // deselect ADC

	ADC_VAL = (((hbyte & 0x0F)<<8)|lbyte);

	// convert ADC value to voltage
	return (ADC_VAL * 3.3)/4096;
}

 //Display Floating Point on LCD

void LCD_FLOAT(float f)
{
	int n;

	n = f;
	LCD_INTEGER(n);

	LCD_DATA('.');

	n = (f-n)*100;
	LCD_INTEGER(n);
}

// Display Integer on LCD

void LCD_INTEGER(unsigned int n)
{
	if(n>=100)
	{
		LCD_DATA((n/100)+48);
		LCD_DATA(((n/10)%10)+48);
		LCD_DATA((n%10)+48);
	}
	else if(n>=10)
	{
		LCD_DATA((n/10)+48);
		LCD_DATA((n%10)+48);
	}
	else
	{
		LCD_DATA((n%10)+48);
	}
}

//Display String on LCD

void LCD_STRING(u8 *s)
{
	while(*s)
	{
		LCD_DATA(*s++);
	}
}
