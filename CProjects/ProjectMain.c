#include "MSP.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ProjectHeader.h"

void Init()
{    
    //Place code that runs once
    //Set up UART
    EUSCI_A0 ->CTLW0 |=1; //put in reset state
    EUSCI_A0 ->MCTLW = 0; //no oversampling
    EUSCI_A0 ->CTLW0 |= 0x0080; //or can do = 0x81
    EUSCI_A0 ->BRW = 26; //baud rate
    
    //set pin 2 and 3 on port 1 to UART mode
    P1 ->SEL0 |=0x0C; 
    P1 ->SEL1 &=~0x0C;
    EUSCI_A0 -> CTLW0 &= ~(0x01);
    //Initialize RGB Ligths
    P2 -> SEL0 &= ~(0x07);
    P2 -> SEL1 &= ~(0x07);
    P2 -> DIR |=0x07;
    P2 -> OUT &=~0x07;
    //Initialize Buttons
    P1 -> SEL0 &= ~(0x12);
    P1 -> SEL1 &= ~(0x12);
    P1 -> DIR &= ~(0x12);
    P1 -> OUT &= ~(0x12);
    //Initialize Timers
    SysTick->CTRL|=(0x04);
    TIMER32_1->CONTROL|= 0X42;
		ADCInit();
}
void dprinter(char word[30])
{
	int i=0;
	while(word[i]!='\0')
	{
		EUSCI_A0 ->TXBUF =word[i];
		while((EUSCI_A0->IFG &2)==0){}
		i++;
	}
		EUSCI_A0 ->TXBUF ='\n';
		while((EUSCI_A0->IFG &2)==0){}
		EUSCI_A0 ->TXBUF ='\r';
		while((EUSCI_A0->IFG &2)==0){}
	return;
}
struct user typer()//Shows what user is typing and saves it to a char array/string
{
	struct user *temp;
	int l=0;
	while(1)
	{
		char x;
		if((EUSCI_A0->IFG &1)!=0)
		{
			x=EUSCI_A0->RXBUF;
			EUSCI_A0 ->TXBUF =x;
			while((EUSCI_A0->IFG &2)==0){}
			if(x=='\r')
			{
				temp->userinput[l]='\0';
				EUSCI_A0 ->TXBUF ='\n';
				while((EUSCI_A0->IFG &2)==0){}
				break;
			}
			if(x=='\b')
			{
				l--;
				temp->userinput[l]='\0';
				EUSCI_A0 ->TXBUF =0;
				while((EUSCI_A0->IFG &2)==0){}
			}
			else
			{
				temp->userinput[l]=x;
				l++;
			}
		}
	}
	return *temp;
}
void systimer(int y)//SysticTimer y seconds counter
{
	for(int x =y;x>0;x--)
	{
		SysTick->LOAD= 3000000-1;
		SysTick->CTRL|=1;
		while((SysTick->CTRL&0x10000)==0){/*Wait*/}
		SysTick->CTRL&=~(0x01);
	}
}
void menu()
{
	struct user temp;
	char erMsg[20];
	while(1)
	{
		dprinter("MSP432 Menu\n");
		
		dprinter("1. Temperature Reading");
		dprinter("2. Exit Program");
		temp=typer();
		switch (atoi(temp.userinput))
		{
			case 1:
				TempReader();
				break;
			case 2:
				done();
				break;
			default:
				sprintf(erMsg,"%s is not an option",temp.userinput);
				dprinter(erMsg);
				break;
		}
	}
	return;
}
void TempReader()
{
	int vX,mV,Ctemp,Ftemp;
	float V;
	int base=pow(2.0,14.0);
	char readings[150];
	dprinter("Enter Number of Temperature Readings");
	 int x=atoi(typer().userinput);
    for(int y=x;y>0;y--)
    {
			vX=ADCRead();
			V=(float)vX*1.0/base;
			V*=3.3;
			mV=V*1000;
			sprintf(readings,"The Voltage is %d mV",mV);
			dprinter(readings);
			Ctemp=(mV-500)/100;
			Ftemp=(1.8*Ctemp)+32;
			sprintf(readings,"The temperature is %d C",Ctemp);
			dprinter(readings);
			sprintf(readings,"The temperature is %d F",Ftemp);
			dprinter(readings);
		}
}
void done(){while(1);}
