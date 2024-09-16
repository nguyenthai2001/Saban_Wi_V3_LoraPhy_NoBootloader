
#include "SX1278.h"
#include "SX1278_HW.h"
#include "SX1278_LoRa.h"
#include "SX1278-Fsk.h"

/*!
 * SX1276 registers variable
 */
unsigned char SX1276Regs[0x70] = {0};

uint8_t LoRaOn = 0;

uint8_t SX1276SetMode(uint8_t mode )
{
      if(mode == 0 )
      {
            LoRaOn = 0 ;
      }
      if(mode == 1)
      {
            LoRaOn = 1 ;
      }
      return LoRaOn ; 
}

uint8_t SX1276GetMode(void)
{
      return LoRaOn ;
}

void SX1276Init(void)
{
    // Initialize LoRa registers structure
    SX1276LR = (tSX1276LR *)SX1276Regs;
    SX1276FSk = (tSX1276FSk *)SX1276Regs;
    SX1276InitIO();
    SX1276Reset();
    // REMARK: After radio reset the default modem is FSK      
    if(LoRaOn == 0)
    {
          SX1276SetLoRaOn( LoRaOn );
          // Initialize FSK modem
          SX1276FskInit( );
    }
    else
    {
          SX1276SetLoRaOn( LoRaOn );
          // Initialize LoRa modem
          SX1276LoRaInit( );  
    }          
              
}

void SX1276Reset(void)
{
    SX1276SetReset(RADIO_RESET_ON);
    // Wait 1ms
    CLK_SysTickDelay(1000);
    SX1276SetReset(RADIO_RESET_OFF);
    // Wait 6ms
    CLK_SysTickDelay(6000);
}

void SX1276SetLoRaOn(uint8_t enable)
{
    LoRaOn = enable;
    
    if( LoRaOn == 1)
    {
      SX1276LoRaSetOpMode(RFLR_OPMODE_SLEEP);

      SX1276LR->RegOpMode = (SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK) | RFLR_OPMODE_LONGRANGEMODE_ON;
      SX1276Write(REG_LR_OPMODE, SX1276LR->RegOpMode);

      SX1276LoRaSetOpMode(RFLR_OPMODE_STANDBY);
      //                         RxDone                     RxTimeout                  FhssChangeChannel          CadDone
      SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
      //                         CadDetected                ModeReady
      SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
      SX1276WriteBuffer(REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2);

      SX1276ReadBuffer(REG_LR_OPMODE, SX1276Regs + 1, 0x70 - 1);
    }
     if(LoRaOn == 0)
    {
        SX1276LoRaSetOpMode( RFLR_OPMODE_SLEEP );
        
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_OFF;
        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );
        
        SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
        
        SX1276ReadBuffer( REG_OPMODE, SX1276Regs + 1, 0x70 - 1 );
    }
}

void SX1276SetOpMode(unsigned char opMode)
{
    if( LoRaOn == 0 )
    {
        SX1276FskSetOpMode( opMode );
    }
    else
    {
        SX1276LoRaSetOpMode( opMode );
    }
}

unsigned char SX1276GetOpMode(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskGetOpMode( );
    }
    else
    {
        return SX1276LoRaGetOpMode( );
    }
}

double SX1276ReadRssi(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskReadRssi( );
    }
    else
    {
        return SX1276LoRaReadRssi( );
    }
}

unsigned char SX1276ReadRxGain(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskReadRxGain( );
    }
    else
    {
        return SX1276LoRaReadRxGain( );
    }
}

unsigned char SX1276GetPacketRxGain(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskGetPacketRxGain(  );
    }
    else
    {
        return SX1276LoRaGetPacketRxGain(  );
    }
}

unsigned char SX1276GetPacketSnr(void)
{
    if( LoRaOn == 0 )
    {
         while( 1 )
         {
             // Useless in FSK mode
             // Block program here
         }
    }
    else
    {
        return SX1276LoRaGetPacketSnr(  );
    }
}

double SX1276GetPacketRssi(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskGetPacketRssi(  );
    }
    else
    {
        return SX1276LoRaGetPacketRssi( );
    }
}

uint32_t SX1276GetPacketAfc( void )
{
    if( LoRaOn == 0 )
    {
        return SX1276FskGetPacketAfc(  );
    }
    else
    {
         while( 1 )
         {
             // Useless in LoRa mode
             // Block program here
         }
    }
}

void SX1276StartRx(void)
{
    if( LoRaOn == 0 )
    {
        SX1276FskSetRFState( RF_STATE_RX_INIT );
    }
    else
    {
        SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
    }
}

void SX1276GetRxPacket(void *buffer, unsigned short int size_in)
{
    if( LoRaOn == 0 )
    {
        SX1276FskGetRxPacket( buffer, size_in );
    }
    else
    {
        SX1276LoRaGetRxPacket( buffer, size_in );
    }
}

void SX1276SetTxPacket(const void *buffer, unsigned short int size_in)
{
    if( LoRaOn == 0 )
    {
        SX1276FskSetTxPacket( buffer, size_in );
    }
    else
    {
        SX1276LoRaSetTxPacket( buffer, size_in );
    }
}

unsigned char SX1276GetRFState(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskGetRFState( );
    }
    else
    {
        return SX1276LoRaGetRFState( );
    }
}

void SX1276SetRFState(unsigned char state)
{
    if( LoRaOn == 0 )
    {
        SX1276FskSetRFState( state );
    }
    else
    {
        SX1276LoRaSetRFState( state );
    }
}

unsigned long int SX1276Process(void)
{
    if( LoRaOn == 0 )
    {
        return SX1276FskProcess( );
    }
    else
    {
        return SX1276LoRaProcess( );
    }
}

void SX1276StartCad(void)
{
    if( LoRaOn == 0 )
    {        
    }
    else
    {
        SX1276LoRaSetRFState( RFLR_STATE_CAD_INIT );
    }
}

