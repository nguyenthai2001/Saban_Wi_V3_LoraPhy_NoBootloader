
#include "gpio_control.h"
#include "gpio_driver.h"
#include "DataFlash_stack.h"

unsigned char u8int = 0;

uint8_t u32int_1 = 0 ;
uint8_t u32int_2 = 0 ;
uint8_t u32int_3 = 0 ;
uint8_t u32int_4 = 0 ;
uint8_t u32int_5 = 0 ;
uint8_t u32int_6 = 0 ;
uint8_t u32int_7 = 0 ;
uint8_t u32int_8 = 0 ;

uint8_t u8status_1 = 0x01 ;
uint8_t u8status_2 = 0x01 ;
uint8_t u8status_3 = 0x01 ;
uint8_t u8status_4 = 0x01 ;
uint8_t u8status_5 = 0x01 ;
uint8_t u8status_6 = 0x01 ;
uint8_t u8status_7 = 0x01 ;
uint8_t u8status_8 = 0x01 ;

uint32_t Time_Rising_1 ;
uint32_t Time_Falling_1 ;

uint32_t Time_Rising_2 ;
uint32_t Time_Falling_2 ;

uint32_t Time_Rising_3 ;
uint32_t Time_Falling_3 ;

uint32_t Time_Rising_4 ;
uint32_t Time_Falling_4 ;

uint32_t Time_Rising_5 ;
uint32_t Time_Falling_5 ;

uint32_t Time_Rising_6 ;
uint32_t Time_Falling_6 ;

uint32_t Time_Rising_7 ;
uint32_t Time_Falling_7 ;

uint32_t Time_Rising_8 ;
uint32_t Time_Falling_8 ;

volatile uint32_t u32_TimeTick = 0;

/*--------------------------------------------------------------------*/
/* Check Input Status  */
/*--------------------------------------------------------------------*/
uint8_t Check_Button_Status_1(Saban_Client_Dataflash *client_pkg)
{      
      
      u32int_1 = 1 ;
      if(PA6 == 1)
      {            
            
            if(Time_Falling_1 >= client_pkg->timefalsh)
            {
                  u8status_1 = 0b10 ;
            }
            Time_Falling_1 = 0 ;
      }
      if(PA6 == 0)
      {
            
            if(Time_Rising_1 >= client_pkg->timefalsh)
            {
                  u8status_1 = 0b01;
            }
            Time_Rising_1 = 0;
      }
      
      return u8status_1 ;
}

uint8_t Check_Button_Status_2(Saban_Client_Dataflash *client_pkg)
{      
      u32int_2 = 1;
      if(PC7 == 1)
      {                       
            if(Time_Falling_2 >= client_pkg->timefalsh)
            {
                  u8status_2 = 0b10 ;
            }
            Time_Falling_2 = 0 ;
      }
      if(PC7 == 0)
      {
            
            if(Time_Rising_2 >= client_pkg->timefalsh)
            {
                  u8status_2 = 0b01;
            }
            Time_Rising_2 = 0;
      }
      
      return u8status_2 ;
}

uint8_t Check_Button_Status_3(Saban_Client_Dataflash *client_pkg)
{      
      u32int_3 = 1;
      if(PC6 == 1)
      {                       
            if(Time_Falling_3 >= client_pkg->timefalsh)
            {
                  u8status_3 = 0b10 ;
            }
            Time_Falling_3 = 0 ;
      }
      if(PC6 == 0)
      {
            
            if(Time_Rising_3 >= client_pkg->timefalsh)
            {
                  u8status_3 = 0b01;
            }
            Time_Rising_3 = 0;
      }
      
      return u8status_3 ;
}

uint8_t Check_Button_Status_4(Saban_Client_Dataflash *client_pkg)
{      
      u32int_4 = 1;
      if(PB15 == 1)
      {                       
            if(Time_Falling_4 >= client_pkg->timefalsh)
            {
                  u8status_4 = 0b10 ;
            }
            Time_Falling_4 = 0 ;
      }
      if(PB15 == 0)
      {
            
            if(Time_Rising_4 >= client_pkg->timefalsh)
            {
                  u8status_4 = 0b01;
            }
            Time_Rising_4 = 0;
      }
      
      return u8status_4 ;
}

uint8_t Check_Button_Status_5(Saban_Client_Dataflash *client_pkg)
{      
      u32int_5 = 1;
      if(PB8 == 1)
      {                       
            if(Time_Falling_5 >= client_pkg->timefalsh)
            {
                  u8status_5 = 0b10 ;
            }
            Time_Falling_5 = 0 ;
      }
      if(PB8 == 0)
      {
            
            if(Time_Rising_5 >= client_pkg->timefalsh)
            {
                  u8status_5 = 0b01;
            }
            Time_Rising_5 = 0;
      }
      
      return u8status_5 ;
}

uint8_t Check_Button_Status_6(Saban_Client_Dataflash *client_pkg)
{      
      u32int_6 = 1;
      if(PA11 == 1)
      {                       
            if(Time_Falling_6 >= client_pkg->timefalsh)
            {
                  u8status_6 = 0b10 ;
            }
            Time_Falling_6 = 0 ;
      }
      if(PA11 == 0)
      {
            
            if(Time_Rising_6 >= client_pkg->timefalsh)
            {
                  u8status_6 = 0b01;
            }
            Time_Rising_6 = 0;
      }
      
      return u8status_6 ;
}

uint8_t Check_Button_Status_7(Saban_Client_Dataflash *client_pkg)
{      
      u32int_7 = 1;
      if(PA10 == 1)
      {                       
            if(Time_Falling_7 >= client_pkg->timefalsh)
            {
                  u8status_7 = 0b10 ;
            }
            Time_Falling_7 = 0 ;
      }
      if(PA10 == 0)
      {
            
            if(Time_Rising_7 >= client_pkg->timefalsh)
            {
                  u8status_7 = 0b01;
            }
            Time_Rising_7 = 0;
      }
      
      return u8status_7 ;
}

uint8_t Check_Button_Status_8(Saban_Client_Dataflash *client_pkg)
{      
      u32int_8 = 1;
      if(PA9 == 1)
      {                       
            if(Time_Falling_8 >= client_pkg->timefalsh)
            {
                  u8status_8 = 0b10 ;
            }
            Time_Falling_8 = 0 ;
      }
      if(PA9 == 0)
      {
            
            if(Time_Rising_8 >= client_pkg->timefalsh)
            {
                  u8status_8 = 0b01;
            }
            Time_Rising_8 = 0;
      }
      
      return u8status_8 ;
}

/*-------------------------------------------------------------------------------------------*/
/* Return Input Status  */
/*-------------------------------------------------------------------------------------------*/
uint8_t return_status_1(void)
{
     return u8status_1 ;
}
uint8_t return_status_2(void)
{
     return u8status_2 ;
}
uint8_t return_status_3(void)
{
     return u8status_3 ;
}
uint8_t return_status_4(void)
{    
      return u8status_4 ;
}
uint8_t return_status_5(void)
{     
      return u8status_5 ;
}
uint8_t return_status_6(void)
{     
      return u8status_6 ;
}
uint8_t return_status_7(void)
{     
      return u8status_7 ;
}
uint8_t return_status_8(void)
{     
      return u8status_8 ;
}
      
void GPAB_IRQHandler(void)
{         		
//      TIMER_Start(TIMER1);
		if(GPIO_GET_INT_FLAG(PA,BIT6))
		{
				Time_Rising_1 = 0;
				Time_Falling_1 = 0;
				GPIO_CLR_INT_FLAG(PA,BIT6);
				Check_Button_Status_1(&client_dataflash);
		}
		
		if(GPIO_GET_INT_FLAG(PB,BIT15))
		{
				Time_Rising_4 = 0;
				Time_Falling_4 = 0;
				GPIO_CLR_INT_FLAG(PB,BIT15);
				Check_Button_Status_4(&client_dataflash);
		}
		
		if(GPIO_GET_INT_FLAG(PB,BIT8))
		{
				Time_Rising_5 = 0;
				Time_Falling_5 = 0;
				GPIO_CLR_INT_FLAG(PB,BIT8);
				Check_Button_Status_5(&client_dataflash);
		}
		
		if(GPIO_GET_INT_FLAG(PA,BIT11))
		{
				Time_Rising_6 = 0;
				Time_Falling_6 = 0;
				GPIO_CLR_INT_FLAG(PA,BIT11);
				Check_Button_Status_6(&client_dataflash);
		}
		
		if(GPIO_GET_INT_FLAG(PA,BIT10))
		{
				Time_Rising_7 = 0;
				Time_Falling_7 = 0;
				GPIO_CLR_INT_FLAG(PA,BIT10);
				Check_Button_Status_7(&client_dataflash);
		}
					 
		if(GPIO_GET_INT_FLAG(PA,BIT9))
		{
					Time_Rising_8 = 0;
					Time_Falling_8 = 0;
					GPIO_CLR_INT_FLAG(PA,BIT9);
					Check_Button_Status_8(&client_dataflash);
		}
		
		else
		{
					PA->ISRC = PA->ISRC ;
					PB->ISRC = PB->ISRC;
		}
}


void GPCDEF_IRQHandler(void)
{
		if(GPIO_GET_INT_FLAG(PC,BIT7))
		{
				Time_Rising_2 = 0;
				Time_Falling_2 = 0;
				GPIO_CLR_INT_FLAG(PC,BIT7);
				Check_Button_Status_2(&client_dataflash);
		}
		
		if(GPIO_GET_INT_FLAG(PC,BIT6))
		{
				Time_Rising_3 = 0;
				Time_Falling_3 = 0;
				GPIO_CLR_INT_FLAG(PC,BIT6);
				Check_Button_Status_3(&client_dataflash);
		}
		
		else
		{     
				PC->ISRC = PC->ISRC ;
				PD->ISRC = PD->ISRC;
		}
}

/*---------------------------------------------------------------------*/
/* Timer use  for Input status Busy  */
/*----------------------------------------------------------------------*/
void TMR1_IRQHandler(void)
{
      if(TIMER_GetIntFlag(TIMER1) == 1)
      {            
            TIMER_ClearIntFlag(TIMER1);
            if(u32int_1 == 1)
            {
                  if(PA6 == 1)
                  {
                        Time_Falling_1 ++ ;
                        if(Time_Falling_1 >= client_dataflash.timefalsh)
                        {
                             u8status_1 = 0b01 ;
                             Time_Falling_1 = 0 ;
                        }
                  }
                  if(PA6 == 0)
                  {
                        Time_Rising_1 ++ ;                  
                        if(Time_Rising_1 >= client_dataflash.timefalsh)
                        {
                              u8status_1 = 0b10;
                              Time_Rising_1 = 0 ;
                        } 
                  }
            }
            
            if(u32int_2 == 1)
            {
                  if(PC7 == 1)
                  {
                        Time_Falling_2 ++ ;
                        if(Time_Falling_2 >= client_dataflash.timefalsh)
                        {
                             u8status_2 = 0b01 ;
                             Time_Falling_2 = 0 ;
                        }
                  }
                  if(PC7 == 0)
                  {
                        Time_Rising_2 ++ ;                  
                        if(Time_Rising_2 >= client_dataflash.timefalsh)
                        {
                              u8status_2 = 0b10;
                              Time_Rising_2 = 0 ;
                        } 
                  }
            }
            
            if(u32int_3 == 1)
            {
                  if(PC6 == 1)
                  {
                        Time_Falling_3 ++ ;
                        if(Time_Falling_3 >= client_dataflash.timefalsh)
                        {
                             u8status_3 = 0b01 ;
                             Time_Falling_3 = 0 ;
                        }
                  }
                  if(PC6 == 0)
                  {
                        Time_Rising_3 ++ ;                  
                        if(Time_Rising_3 >= client_dataflash.timefalsh)
                        {
                              u8status_3 = 0b10;
                              Time_Rising_3 = 0 ;
                        } 
                  }
            }
            
            if(u32int_4 == 1)
            {
                  if(PB15 == 1)
                  {
                        Time_Falling_4 ++ ;
                        if(Time_Falling_4 >= client_dataflash.timefalsh)
                        {
                             u8status_4 = 0b01 ;
                             Time_Falling_4 = 0 ;
                        }
                  }
                  if(PB15 == 0)
                  {
                        Time_Rising_4 ++ ;                  
                        if(Time_Rising_4 >= client_dataflash.timefalsh)
                        {
                              u8status_4 = 0b10;
                              Time_Rising_4 = 0 ;
                        } 
                  }
            }
            
            if(u32int_5 == 1)
            {
                  if(PB8 == 1)
                  {
                        Time_Falling_5 ++ ;
                        if(Time_Falling_5 >= client_dataflash.timefalsh)
                        {
                             u8status_5 = 0b01 ;
                             Time_Falling_5 = 0 ;
                        }
                  }
                  if(PB8 == 0)
                  {
                        Time_Rising_5 ++ ;                  
                        if(Time_Rising_5 >= client_dataflash.timefalsh)
                        {
                              u8status_5 = 0b10;
                              Time_Rising_5 = 0 ;
                        } 
                  }
            }
            
            
            if(u32int_6 == 1)
            {
                  if(PA11 == 1)
                  {
                        Time_Falling_6 ++ ;
                        if(Time_Falling_6 >= client_dataflash.timefalsh)
                        {
                             u8status_6 = 0b01 ;
                             Time_Falling_6 = 0 ;
                        }
                  }
                  if(PA11 == 0)
                  {
                        Time_Rising_6 ++ ;                  
                        if(Time_Rising_6 >= client_dataflash.timefalsh)
                        {
                              u8status_6 = 0b10;
                              Time_Rising_6 = 0 ;
                        } 
                  }
            }

            if(u32int_7 == 1)
            {
                  if(PA10 == 1)
                  {
                        Time_Falling_7 ++ ;
                        if(Time_Falling_7 >= client_dataflash.timefalsh)
                        {
                             u8status_7 = 0b01 ;
                             Time_Falling_7 = 0 ;
                        }
                  }
                  if(PA10 == 0)
                  {
                        Time_Rising_7 ++ ;                  
                        if(Time_Rising_7 >= client_dataflash.timefalsh)
                        {
                              u8status_7 = 0b10;
                              Time_Rising_7 = 0 ;
                        } 
                  }
            }
            
            
            if(u32int_8 == 1)
            {
                  if(PA9 == 1)
                  {
                        Time_Falling_8 ++ ;
                        if(Time_Falling_8 >= client_dataflash.timefalsh)
                        {
                             u8status_8 = 0b01 ;
                             Time_Falling_8 = 0 ;
                        }
                  }
                  if(PA9 == 0)
                  {
                        Time_Rising_8 ++ ;                  
                        if(Time_Rising_8 >= client_dataflash.timefalsh)
                        {
                              u8status_8 = 0b10;
                              Time_Rising_8 = 0 ;
                        } 
                  }
            }
      }
}

/**
 *    @brief        Saban_Input_Control(void)
 *
 *    @param[in]    None
 *    @return       None
 
 *    @details      Read Input function for Client 
 */	
uint16_t Saban_Input_Control (Saban *t_device,Saban_Client_Dataflash *client_pkg)
{
      int i , j ; 
      int pos[8] = {0} ;
      int type[8] = {0} ;
      uint16_t status = 0 ;
      for(i = 0; i<8 ;++i)
      {
            if(t_device->data_h & (1<<i))
            {
                  pos[i] = i+1 ;
            }
            else
            {
                  pos[i] = 0 ;
            }                  
      }
      
      for(j = 0; j<8 ;++j)
      {
            if(client_pkg->input_type & (1<<j))
            {
                  type[j] = j+1 ;
            }
            else
            {
                  type[j] = 0 ;
            }                  
      }  
      if(pos[0] != 0 && type[0] == 1) status |= (return_status_1()) << 0;
      if(pos[0] != 0 && type[0] == 0) status |= (LED1 == 1 ? 0b01 : 0b10) << 0;
      
      if(pos[1] != 0 && type[1] == 2) status |= (return_status_2()) << 2;
      if(pos[1] != 0 && type[1] == 0) status |= (LED2 == 1 ? 0b01 : 0b10) << 2;
      
      if(pos[2] != 0 && type[2] == 3) status |= (return_status_3()) << 4;
      if(pos[2] != 0 && type[2] == 0) status |= (LED3 == 1 ? 0b01 : 0b10) << 4;
      
      if(pos[3] != 0 && type[3] == 4) status |= (return_status_4()) << 6;
      if(pos[3] != 0 && type[3] == 0) status |= (LED4 == 1 ? 0b01 : 0b10) << 6;
      
      if(pos[4] != 0 && type[4] == 5) status |= (return_status_5()) << 8;
      if(pos[4] != 0 && type[4] == 0) status |= (LED5 == 1 ? 0b01 : 0b10) << 8;
      
      if(pos[5] != 0 && type[5] == 6) status |= (return_status_6()) << 10;
      if(pos[5] != 0 && type[5] == 0) status |= (LED6 == 1 ? 0b01 : 0b10) << 10;
      
      if(pos[6] != 0 && type[6] == 7) status |= (return_status_7()) << 12;
      if(pos[6] != 0 && type[6] == 0) status |= (LED7 == 1 ? 0b01 : 0b10) << 12;
      
      if(pos[7] != 0 && type[7] == 8) status |= (return_status_8()) << 14;
      if(pos[7] != 0 && type[7] == 0) status |= (LED8 == 1 ? 0b01 : 0b10) << 14;

      return status ;
}

/**
 *    @brief        Saban_Output_Control(void)
 *
 *    @param[in]    None
 *    @return       None
 
 *    @details      Control output function for Client 
 */	
void Saban_Output_Control (Saban_Client_Dataflash *client_pkg)
{
      int i ; 
      int pos[8] = {0} ;
      for(i = 0; i<8 ;++i)
      {
            if(client_pkg->output_number & (1<<i))
            {
                  pos[i] = i+1 ;
            }
            else
            {
                  pos[i] = 0 ;
            }                  
      }
      if(pos[0] != 0) LED9 = 1 ;
      if(pos[0] == 0) LED9 = 0 ;
      if(pos[1] != 0) LED10 = 1;
      if(pos[1] == 0) LED10 = 0;
      if(pos[2] != 0) LED11 = 1;
      if(pos[2] == 0) LED11 = 0;
      if(pos[3] != 0) LED12 = 1;
      if(pos[3] == 0) LED12 = 0;
      if(pos[4] != 0) LED13 = 1;
      if(pos[4] == 0) LED13 = 0;
      if(pos[5] != 0) LED14 = 1;
      if(pos[5] == 0) LED14 = 0;
      if(pos[6] != 0) LED15 = 1;
      if(pos[6] == 0) LED15 = 0;
      if(pos[7] != 0) LED16 = 1;
      if(pos[7] == 0) LED16 = 0;

}