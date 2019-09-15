
#include "Interrapt.h"


//_____________________________________UART_1________________________________________________________________________________________________________________________________________________________
unsigned char RXUbyte = 0 ;
unsigned char bufferRXU[40] = {0};
volatile unsigned char *massU = bufferRXU; //глобальная переменная
volatile int RXU_flag = 0;
volatile int RXU_q    = 0;
volatile int nU       = 0;
volatile int UNAR     = 0;
int flagU             = 0;
int iRXU              = 0;
//____________________________________________________________________________________________________________________________________________________________________________________________________

/*//_____________________________________UART_2________________________________________________________________________________________________________________________________________________________
unsigned char RXU2byte = 0;
unsigned char bufferRXU2[18] = {0};
volatile unsigned char *massU2 = bufferRXU2; //глобальная переменная
volatile int RXU2_flag  = 0;
volatile int RXU2_q     = 0;
volatile int nU2        = 0;
volatile int UNAR       = 0;
int flagU2              = 0;
int iRXU2               = 0;
*/
//____________________________________________________________________________________________________________________________________________________________________________________________________

//_____________________________________SPI_2__________________________________________________________________________________________________________________________________________________________
/*unsigned char RXS2byte = 0 ;
unsigned char bufferRXS2[13] = {0};
volatile unsigned char *massS2 = bufferRXS2; //глобальная переменная
volatile int RXS2_flag = 0;
volatile int RXS2_q    = 0;
volatile int nS2       =13;
int flagS2             = 0;
int iRXS2              = 0;*/





void UART1_IRQHandler (void)
{
//=-=-=-=-=-=-=-=-=-=-=-=Прерывание прием =-=-=-=-=-=-=-=-=-=-=-=-=-=-	
		if ((MDR_UART1->RIS & 0x10)== 0x10)
			{	 
				MDR_UART1->ICR = 0x10;                                                            //Tx_interrupt_flagU2 clear  (Сброс флага)
	                                                             //MDR_PORTC->RXTX ^= 0x04;          //светодиод;   //Показываем что прерывание обработано
	      RXUbyte = MDR_UART1->DR&0xFF;                                                                                                    								 
				bufferRXU[iRXU]=RXUbyte;                                                       //получаем символ ложим символ в буфер.
				if (bufferRXU [1] != 0x10){nU=8;}					
				else if ((bufferRXU [1] == 0x10) && (iRXU == 6)) {nU = 9 + bufferRXU[6];} 
				iRXU++;                                                                          //наращиваем счетчик буфера	
				//RXU2_q = 1;		
	      if(iRXU>=nU)
        {RXU_flag=1;iRXU=0; UNAR=1;}
				//MDR_PORTA->RXTX ^= 0x2000;
       
			}	
//=-=-=-=-=-=-=-=-=-=-=-=Прерывание передача =-=-=-=-=-=-=-=-=-=-=-=-=-=-	
			 if ((MDR_UART1->RIS & 0x020)== 0x0020)
			 {
				MDR_UART1->ICR = 0x0020;
						if(RXU_q == 1)
						{
							RXU_q  = 0;
							int c = 1;
							while (c < nU)
							  {
								//if ((MDR_UART2->DR & 0xF00)== 0X00)	
								if ((MDR_UART1->FR & 0x80)== 0X80) 
									{
									MDR_UART1->DR = massU[c];    //передача    
									c++;												
									}
									//MDR_PORTC->RXTX ^= 0x0300;
                  //MDR_PORTC->RXTX ^= 0x0100;
								}
						}
				}
				 
			 
 }

//*****************************************************************************************************************************************************

void UART2_IRQHandler (void)
{
//=-=-=-=-=-=-=-=-=-=-=-=Прерывание прием =-=-=-=-=-=-=-=-=-=-=-=-=-=-	
			if ((MDR_UART2->RIS & 0x10)== 0x10)
			{	 
				MDR_UART2->ICR = 0x10;     //Tx_interrupt_flagU2 clear  (Сброс флага)
	                                         //MDR_PORTC->RXTX ^= 0x04;          //светодиод;   //Показываем что прерывание обработано
	      RXUbyte = MDR_UART2->DR&0xFF;  
				
				
                                           //MDR_TIMER1-> CNT = 0;			    
                                           //MDR_TIMER1-> CNTRL = 0x00000001;
				                                   //if(RXU2byte == 0x0A || RXU2byte == 0x0D) {RXU2_flag=1;iRXU2=0;} //флаг конец посылки CR или LF (для каретки)									 
				bufferRXU[iRXU]=RXUbyte;  //получаем символ ложим символ в буфер.
				if (bufferRXU [1] != 0x10){nU=8;}					
				else if ((bufferRXU [1] == 0x10) && (iRXU == 6)) {nU = 9 + bufferRXU[6];} 
				iRXU++; //наращиваем счетчик буфера	
				//RXU2_q = 1;		
	      if(iRXU>=nU)
        {RXU_flag=1;iRXU=0;UNAR = 2;}
			}	
//=-=-=-=-=-=-=-=-=-=-=-=Прерывание передача =-=-=-=-=-=-=-=-=-=-=-=-=-=-	
			 if ((MDR_UART2->RIS & 0x020)== 0x0020)
			 {
				MDR_UART2->ICR = 0x0020;
						if(RXU_q == 1)
						{
							RXU_q  = 0;
							int c = 1;
							while (c < nU)
							  {
								//if ((MDR_UART2->DR & 0xF00)== 0X00)	
								if ((MDR_UART2->FR & 0x80)== 0X80) 
									{
									MDR_UART2->DR = massU[c];    //передача    
									c++;												
									}
								}
						}
				}
				 
			 
 }

//*****************************************************************************************************************************************************
/*void SPI2_IRQ (void)
{
	if ((MDR_SSP2->RIS & 0x04)== 0x04)  //RX прием
  {
	RXS2byte = MDR_SSP2->DR&0xFF;
	bufferRXS2[iRXS2]=RXS2byte;
	iRXS2++;
	if(iRXS2>=nS2)
        {RXS2_flag=1; iRXS2=0;}
	}
	
	
 if ((MDR_SSP2->RIS & 0x08)== 0x08)  //TX передача
 {
	if (RXS2_q == 1)
	{
	    RXS2_q  = 0;
			int c   = 1;
			while (c < nS2)
							  {	
								if ((MDR_SSP2->MIS & 0x01)!= 0X01) 
									{
									MDR_SSP2->DR = massU2[c];    //передача    
									c++;												
									}
								}
						}
	}		
	 
 }
	 
 
//____________________________________________________________________________________________________________________________________________________________________________________________________
*/
 
 
	
	
