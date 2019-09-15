#include "Interrapt.h"
#include "MDR1986VE1T.h"
#include "Initmcontroller.h"
#include "adrpr.h"

//___________________Адресное пространство______________________________|
#define dann (*f)
#define qer (*g1) 
#define qar (*g2) 
#define qtr (*g3) 
#define qsr (*g4) 
#define qcr (*g5)




//_____________глобальные переменные__________Interrapt.h_______________|
extern unsigned char *massU;
extern int UNAR;
extern int RXU_flag;
extern int RXU_q;
extern int nU;
//_____________локальные переменные_____________________________________|
unsigned short length = 0;
unsigned short    crc = 0; 
unsigned char  colreg = 0;
unsigned char      ks = 0;
int addr = 0;
int colb = 0;


//______________________________C_R_C__1_6________________________
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
unsigned short CRC16_PA(unsigned char *data, int n)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
{
    unsigned char            crc16h, crc16l;
    unsigned char            cl,ch;
    unsigned char            saveh, savel;
    int                      i;
    unsigned int             flagU2;
    unsigned short           res;
    unsigned char            b;

    crc16l = 0xFF;
    crc16h = 0xFF;

    cl = 0x01;
    ch = 0xA0;

    for (i=0;i<n;i++)
    {
        b = *data++;
        crc16l ^= b;

        for (flagU2 = 0; flagU2 <8; flagU2++)
        {
            saveh = crc16h;
            savel = crc16l;

            crc16h >>= 1;
            crc16l >>= 1;

            if (saveh & 0x01)
              {
               crc16l |= 0x80;
              }

            if (savel & 0x01)
              {
               crc16h ^= ch;  
               crc16l ^= cl;
              }
        }
    }

    res = (crc16l << 8) | crc16h;
    return res;
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
void request03 (unsigned char *dmas, int UN) // 0x4 чтение регистров  1..n
{  
	  //{id|fun|adrr|adrr|count|count|crc|crc}
	
	  //unsigned short  adrr   = 0;
	  
	  addr = (*(dmas + 2)<<8)|*(dmas + 3);                       //Адрес чтения
    colreg  = (*(dmas + 4)<<8)|*(dmas + 5);                    //Кол-во регистров
    for (int i = 0; i < (colreg*2); i+=2)                      //цикл по адресам согласно количеству определить регистр
    {    
					*(dmas+3+ks) = adrpr[addr + i];                      // данные из регистра ао адресу записываем в байт Hi байт
					ks++; 
			    *(dmas+3+ks) = adrpr[addr + i+1];                    // данные из регистра ао адресу записываем в байт Lo байт
					ks++;
		}
    *(dmas+2) = ks;                                            //байт с количеством (Количество байт далее)
    crc = CRC16_PA (dmas , ks+3);                              //СRС16
    *(dmas+3+ks) = (crc>> 8);                                  //ch
		ks++;
    *(dmas+3+ks) = crc;                                        //cl
    nU = ks + 4;
		
		addr    = 0;
		colreg  = 0;
    crc     = 0;
		ks      = 0;
		RXU_q  = 1;
	  if (UN == 1) {MDR_UART1->DR = *(dmas+0);}
    if (UN == 2) {MDR_UART2->DR = *(dmas+0);}                                 //передач
    
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
void request06 (unsigned char *dmas, int UN) // 0x4 запись в регистор
{
//{id|fun|adrr|adrr|data|data|crc|crc}	
	
     addr = (*(dmas + 2)<<8)|*(dmas + 3);                      //Адрес записи
     (adrpr[0 + addr]) = (*(dmas + 4)<<8)|*(dmas + 5);				 //Кол-во регистров
     MDR_PORTC->RXTX = dann;
		 addr   = 0;	
		 RXU_q = 1;
		 if (UN == 1) {MDR_UART1->DR = *(dmas+0);}
     if (UN == 2) {MDR_UART2->DR = *(dmas+0);} 
		/* int c = 0;
     while (c<n)	
     //for (int c=0;c<=7;c++)
     {
			
        //if ((MDR_UART2->RIS & 0x20)== 0X20)         //состояние порта
			  if ((MDR_UART2->FR & 0x80)== 0X80) 
        {
            MDR_UART2->DR = *(dmas + c);    //передача  
			      //MDR_PORTC->RXTX^= 0x04;   
            c++;			
        }
     }*/
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
void request10 (unsigned char *dmas, int UN) // 0x4 запись в регистры  1..n
{ 
//{id|fun|adrr|adrr|count|count|sizeb|data|data|crc|crc}
	
	   addr    = (*(dmas + 2)<<8)|*(dmas + 3);                        //Адрес записи
	   colreg  = (*(dmas + 4)<<8)|*(dmas + 5);
	   colb    =  *(dmas + 6);
	   
	   for (int i = 0;  i < colreg; i++)                              //переделать как 06
	   {
	      (adrpr[0 + addr+i]) =  *(dmas + 7 + i);
			  ks++;
		 }
	  *(dmas + 4) = (ks>>8);
	  *(dmas + 5) = ks;
		 
	  crc = CRC16_PA (dmas ,6);                                       //СRС16 
    *(dmas + 6) = (crc>> 8);                                        //ch
    *(dmas + 7) = crc;                                              //cl
		
		RXU_q   = 1;
		nU      = 8;
		 
		ks     = 0;
		crc    = 0;
	  addr   = 0;
		colreg = 0;
	  colb   = 0;
	  
		if (UN == 1) {MDR_UART1->DR = *(dmas+0);}
    if (UN == 2) {MDR_UART2->DR = *(dmas+0);} 
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&//
void UARTx_SEND (unsigned char *data)
{
	  //unsigned char bufferRXU2[18] = {0x01,0x06,0xFF,0xEE,0xFF,0xEE,0x18,0x57};
    if (*data == 0x01)
    {
        unsigned short crcp = (*(data+nU-2))<<8|(*(data+nU-1));  //(число = элемент+1)
        crc = CRC16_PA (data, (nU-2));                          //(массив,кол-во эл.)
        

        if (crcp == crc )
        {
         crc = 0;
            switch (*(data + 1))
            {
            case 0x03:  request03(massU,UNAR);  break;
            case 0x06:  request06(massU,UNAR);  break;
					  case 0x10:  request10(massU,UNAR);  break;
            default:
            break;
            }
        }
    }
}

void SPI_SEND_MVH9M_CAP (unsigned char *data)
{
}
/*{ 
	  
			RXU2_flag = 0;
			//MDR_PORTC->RXTX = 0x04;
				int c=0,c2=0;
				for(int i=17;i>=0; i--)
				{
								if ((*(data+i)|| c2)==0){c=i-1;}
								else{c2=1;}
				}
				if(c==0) {c=17;}
				length =  (*(data+c-3)<<8)|(*(data+c-2));
				unsigned short crcp = (*(data+c-1))<<8|(*(data+c));  //(число = элемент+1)
				crc = CRC16_PA (data, (c-1) );
				if (crcp == crc )
				{
					
						 crc = 0;
						if (*data == 0x01)
						{
								switch (*(data + 1))
								{
								case 0x04:   request1(massU2); break;
								case 0x06:   request2(massU2); break;
								default:
								break;
						    }
						}
				}
}*/

//_____________________________/////////////////////////////////////___________________________________/////////////////////////////____________________________________
int main (void)	
{     
	qer=55;
	qar=2;
	qtr=3;
	qsr=4;
	qcr=5;
	    INITZ_CLKPORT();
			
	    INIPORTA();
	   INIPORTB();
	    INIPORTC();
    	INITZ_UART1();
	
	
	
	    INIPORTD();
	    INIPORTE();
	    INIPORTF();
	    
	MDR_PORTA->RXTX = 0xFFFF;
	MDR_PORTB->RXTX = 0xFFFF;
	MDR_PORTC->RXTX = 0xFFFF;
	MDR_PORTD->RXTX = 0xFFFF;
	MDR_PORTE->RXTX = 0xFFFF;
	MDR_PORTF->RXTX = 0xFFFF;
	
	int i=0;
			//INITZ_TMR();
			//PORTD_PD0_PD1 ();
	    //INITZ_UART1();
			//UART2_SEND(massU2);
	//___________________________________Программа_______________________________________________________________________________________
		while(1)
		{	
			if(RXU_flag == 1) {RXU_flag = 0; UARTx_SEND(massU); }
			  
				
	MDR_PORTA->RXTX ^= 0xFFFF;
	MDR_PORTB->RXTX ^= 0xFFFF;
	MDR_PORTC->RXTX ^= 0xFFFF;
	MDR_PORTD->RXTX ^= 0xFFFF;
	MDR_PORTE->RXTX ^= 0xFFFF;
	MDR_PORTF->RXTX ^= 0xFFFF;
		}
}


