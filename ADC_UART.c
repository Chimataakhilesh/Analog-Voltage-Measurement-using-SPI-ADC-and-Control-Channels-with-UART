#include<LPC21XX.H>
#include "lcd_header.h"
/*Typedef definitions*/
typedef unsigned char u8;
typedef unsigned int u32;
typedef float f32;

/*defines*/
#define CS 1<<7
#define Rx 0x4
#define Tx 0x1 
#define MISO 0x400
#define MOSI 0x1000
#define CLK 0x100

#define MSTR_BIT 5
#define SPIF_BIT 7
#define Mode_3 3
#define SW 9

u8 ch;
f32 Volts;

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

int main()
{
	u8 r = '\r';
	LCD_INIT();
	UART_INIT();
	SPI_INIT();
	while(1)
	{
		UART_STRING("ENTER THE CHANNEL NO");
		UART_TX(r);
		ch = UART_RX();
		LCD_COMMAND(0x80);
    LCD_STRING("CHANNEL: ");
		switch(ch)
		{
			case '0': ch0();break;
			case '1': ch1();break;
			case '2': ch2();break;
			case '3': ch3();break;
			default: UART_STRING("Please enter correct option 0 - 3");break;
		}
		UART_TX(r);
		LCD_COMMAND(0x01);
	}
}
void ch0(void)
{
	LCD_INTEGER(ch-48);
	while(1)
	{
		Volts = Read_ADC(0);
		LCD_COMMAND(0xc0);
		LCD_FLOAT(Volts);
		if(((IOPIN0>>SW)&1)==0)
		{
			while(((IOPIN0>>SW)&1)==0);
			break;
		}
	}
}
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

void UART_INIT(void)
{
	PINSEL0 |= Tx|Rx;
	U0LCR = 0x83;
	U0DLL = 97;
	U0DLM = 0;
	U0LCR = 0x03;
}
void SPI_INIT(void)
{
	PINSEL0 |= CLK|MOSI|MISO;
	IODIR0 |= CS;
	IOSET0 = CS;
	S0SPCCR = 150;
	S0SPCR = ((1<<MSTR_BIT)|Mode_3);
}
void UART_TX(u8 txbyte)
{
	U0THR = txbyte;
	while(((U0LSR>>5)&1)==0);
}
u8 UART_RX(void)
{
	while((U0LSR&1)==0);
	return U0RBR;
}
void UART_STRING(u8 *s)
{
	while(*s)
	{
		UART_TX(*s++);
	}
}
u8 SPI(u8 data)
{
	S0SPDR = data;
	while(!(S0SPSR>>SPIF_BIT)&1);
	return S0SPDR;
}
f32 Read_ADC(u8 channel)
{
	u8 hbyte,lbyte;
	u32 ADC_VAL;
	
	IOCLR0 = CS;
	SPI(0x06);
	hbyte = SPI(channel<<6);
	lbyte = SPI(0x00);
	
	IOSET0 = CS;
	ADC_VAL = (((hbyte & 0x0F)<<8)|lbyte);
	return (ADC_VAL * 3.3)/4096;
}

void LCD_FLOAT(float f)
{
	int n;
	n = f;
	LCD_INTEGER(n);
	LCD_DATA('.');
	n = (f-n)*100;
	LCD_INTEGER(n);
}
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
void LCD_STRING(u8 *s)
{
	while(*s)
	{
		LCD_DATA(*s++);
	}
}
