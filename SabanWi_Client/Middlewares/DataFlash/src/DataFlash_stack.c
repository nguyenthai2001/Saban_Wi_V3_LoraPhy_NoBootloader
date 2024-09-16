
#include "DataFlash_stack.h"

Saban_Client_Dataflash ClientDataFlash[2];

/*---------------------------------------------------------------------------------------*/
/* Data Falsh Init and clientDataFlash update  */
/*---------------------------------------------------------------------------------------*/
void DataFlash_Client_Init(void)
{
    uint32_t BaseAddr ;
    uint8_t byte1 ;
    uint8_t byte2 ;
    uint8_t byte3 ;
    uint8_t byte4 ;
    uint32_t timeflash = 0;

    /* Enable FMC ISP function */
    FMC_Open();
    /* Set Data Flash base address */
    if (SetDataFlashBase(DATA_FLASH_EEPROM_BASE) < 0)
    {
//            printf("Failed to set Data Flash base address!\n");
        while (1) {}
    }
    /* Read Data Flash base address */
    BaseAddr = FMC_ReadDataFlashBaseAddr();
//      printf("[Simulate EEPROM base address at Data Flash 0x%x]\n\n", BaseAddr);

    ClientDataFlash[1].System_code = SIM_EEPROM_READ(0);
    ClientDataFlash[1].SlaveID = SIM_EEPROM_READ(1);
    ClientDataFlash[1].input_type = SIM_EEPROM_READ(2);
    ClientDataFlash[1].output_number = SIM_EEPROM_READ(3);

    byte1 = SIM_EEPROM_READ(4);
    byte2 = SIM_EEPROM_READ(5);
    byte3 = SIM_EEPROM_READ(6);
    byte4 = SIM_EEPROM_READ(7);
    timeflash = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    ClientDataFlash[1].timefalsh = timeflash ;

    ClientDataFlash[1].RfFrequence = SIM_EEPROM_READ(14);
    ClientDataFlash[1].RFPower = SIM_EEPROM_READ(15);
    ClientDataFlash[1].RFBandwidth = SIM_EEPROM_READ(16);
    ClientDataFlash[1].RFSpreadingFactor = SIM_EEPROM_READ(17);
    ClientDataFlash[1].ErrCode            = SIM_EEPROM_READ(18);

    ClientDataFlash[1].Modbus_SlaveID = SIM_EEPROM_READ(8) ;
    ClientDataFlash[1].Modbus_Baudrate = SIM_EEPROM_READ(9);
    ClientDataFlash[1].Modbus_Register = SIM_EEPROM_READ(10);
    ClientDataFlash[1].Modbus_Address = SIM_EEPROM_READ(11);
    ClientDataFlash[1].Modbus_Timeout = SIM_EEPROM_READ(12) << 8 | SIM_EEPROM_READ(13);

    switch (ClientDataFlash[1].Modbus_Register)
    {
    case 0x00 :
        device[1].Modbus_mode = 0 ;
        break ;       // modebus slave
    case 0x01 :
        device[1].Modbus_mode = 0 ;
        break ;
    case 0x02 :
        device[1].Modbus_mode = 0 ;
        break ;
    case 0x03 :
        device[1].Modbus_mode = 0 ;
        break ;

    case 0x10 :
        device[1].Modbus_mode = 1 ;
        break ;        // modbus master
    case 0x11 :
        device[1].Modbus_mode = 1 ;
        break ;
    case 0x12 :
        device[1].Modbus_mode = 1 ;
        break ;
    case 0x13 :
        device[1].Modbus_mode = 1 ;
        break ;
    }
}

/*-----------------------------------------------------------------------------*/
/* Update DataFlash from Master  */
/*-----------------------------------------------------------------------------*/
void Update_DataFlash_From_Master(void)
{
    uint8_t SystemCode ;
//      unsigned char MasterID ;
    unsigned char Commnad ;
    unsigned char MCCode ;
    unsigned char OutputNumber ;
    uint8_t SystemCodeNew ;

    SystemCode = ClientDataFlash[1].System_code ;
    // masterid command mccode
//      MasterID = (SystemCode >> 4)&0x0F ;            // const
    Commnad = (SystemCode >> 2) & 0x03 ;
    MCCode = SystemCode & 0x03 ;

    if (Commnad != device[ClientDataFlash[1].SlaveID].cmd || MCCode != device[ClientDataFlash[1].SlaveID].mccode)
    {
        SystemCodeNew = (device[ClientDataFlash[1].SlaveID].masterID << 4) | (device[ClientDataFlash[1].SlaveID].cmd << 2) | device[ClientDataFlash[1].SlaveID].mccode ;
        SIM_EEPROM_WRITE(0, SystemCodeNew);
        ClientDataFlash[1].System_code = SystemCodeNew;
//            printf("Update Data Flash Done !!! ");
    }

    OutputNumber = ClientDataFlash[1].output_number;
    if (OutputNumber != device[ClientDataFlash[1].SlaveID].data_l)
    {
        SIM_EEPROM_WRITE(3, device[ClientDataFlash[1].SlaveID].data_l);
        ClientDataFlash[1].output_number = SIM_EEPROM_READ(3);
//            printf("Update Data Flash Done !!! ");
    }
}

/*----------------------------------------------------------------------*/
/* Update DataFlash From PC for InputType and TimeFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlash_InputType_From_PC(uint8_t inputtype, uint32_t timeflash)
{
    uint8_t err = 0 ;
    uint8_t byte1 ;
    uint8_t byte2 ;
    uint8_t byte3 ;
    uint8_t byte4 ;

    uint8_t byte1new ;
    uint8_t byte2new ;
    uint8_t byte3new ;
    uint8_t byte4new ;
    uint32_t timeflashnew = 0;

    if (inputtype != ClientDataFlash[1].input_type)
    {
        SIM_EEPROM_WRITE(2, inputtype);
        ClientDataFlash[1].input_type = SIM_EEPROM_READ(2);
        if (ClientDataFlash[1].input_type != inputtype)
        {
            err = 1 ;
        }
    }
    if (timeflash != ClientDataFlash[1].timefalsh)
    {
        byte1 = ((timeflash >> 24) & 0xFF); // Byte cao nh?t
        byte2 = ((timeflash >> 16) & 0xFF);
        byte3 = ((timeflash >> 8) & 0xFF);
        byte4 = (timeflash & 0xFF); // Byte th?p nh?t
        SIM_EEPROM_WRITE(4, byte1);
        SIM_EEPROM_WRITE(5, byte2);
        SIM_EEPROM_WRITE(6, byte3);
        SIM_EEPROM_WRITE(7, byte4);

        byte1new = SIM_EEPROM_READ(4);
        byte2new = SIM_EEPROM_READ(5);
        byte3new = SIM_EEPROM_READ(6);
        byte4new = SIM_EEPROM_READ(7);
        timeflashnew = (byte1new << 24) | (byte2new << 16) | (byte3new << 8) | byte4new;
        ClientDataFlash[1].timefalsh = timeflashnew ;

        if (ClientDataFlash[1].timefalsh != timeflash)
        {
            err = 2 ;
        }
    }
    return err ;
}

/*-----------------------------------------------------------------------------------*/
/* Update DataFlash from PC for MasterID and ClientID  */
/*-----------------------------------------------------------------------------------*/
uint8_t Update_DataFlash_ID_From_PC(uint8_t data1, uint8_t data2)
{
    uint8_t err = 0 ;

    if (data1 != ClientDataFlash[1].System_code)
    {
        SIM_EEPROM_WRITE(0, data1);
        ClientDataFlash[1].System_code = SIM_EEPROM_READ(0) ;
        if (data1 != ClientDataFlash[1].System_code)
        {
            err = 1 ;
        }
    }
    if (data2 != ClientDataFlash[1].SlaveID)
    {
        SIM_EEPROM_WRITE(1, data2);
        ClientDataFlash[1].SlaveID = SIM_EEPROM_READ(1) ;
        if (data2 != ClientDataFlash[1].SlaveID)
        {
            err = 2 ;
        }
    }
    return err ;
}

/*******************************************************************************/
/* Update RF Config from PC to DataFlash */
/*******************************************************************************/
uint8_t Update_DataFlash_RFConfig_From_PC(uint8_t freg, uint8_t power, uint8_t bw, uint8_t sf, uint8_t errcode, uint16_t rxtimeout)
{
    uint8_t err = 0 ;

    uint8_t byteh = 0;
    uint8_t bytel = 0;
    uint16_t rxtimeoutnew = 0;
    uint8_t bytehnew = 0;
    uint8_t bytelnew = 0;

    if (freg != ClientDataFlash[1].RfFrequence)
    {
        SIM_EEPROM_WRITE(14, freg);
        ClientDataFlash[1].RfFrequence = SIM_EEPROM_READ(14);
        if (ClientDataFlash[1].RfFrequence != freg)
        {
            err = 1 ;
        }
    }

    if (power != ClientDataFlash[1].RFPower)
    {
        SIM_EEPROM_WRITE(15, power);
        ClientDataFlash[1].RFPower = SIM_EEPROM_READ(15);
        if (ClientDataFlash[1].RFPower != power)
        {
            err = 2 ;
        }
    }

    if (bw != ClientDataFlash[1].RFBandwidth)
    {
        SIM_EEPROM_WRITE(16, bw);
        ClientDataFlash[1].RFBandwidth = SIM_EEPROM_READ(16);
        if (ClientDataFlash[1].RFBandwidth != bw)
        {
            err = 3 ;
        }
    }

    if (sf != ClientDataFlash[1].RFSpreadingFactor)
    {
        SIM_EEPROM_WRITE(17, sf);
        ClientDataFlash[1].RFSpreadingFactor = SIM_EEPROM_READ(17);
        if (ClientDataFlash[1].RFSpreadingFactor != sf)
        {
            err = 4 ;
        }
    }

    if (errcode != ClientDataFlash[1].ErrCode)
    {
        SIM_EEPROM_WRITE(18, errcode);
        ClientDataFlash[1].ErrCode = SIM_EEPROM_READ(18);
        if (ClientDataFlash[1].ErrCode != errcode)
        {
            err = 5 ;
        }
    }

    if (rxtimeout != ClientDataFlash[1].RXTimeout)
    {
        byteh = ((rxtimeout >> 8) & 0xFF);
        bytel = (rxtimeout & 0xFF); // Byte th?p nh?t
        SIM_EEPROM_WRITE(19, byteh);
        SIM_EEPROM_WRITE(20, bytel);

        bytehnew = SIM_EEPROM_READ(19);
        bytelnew = SIM_EEPROM_READ(20);
        rxtimeoutnew = (bytehnew << 8) | bytelnew;
        ClientDataFlash[1].RXTimeout  = rxtimeoutnew ;

        if (ClientDataFlash[1].RXTimeout != rxtimeoutnew)
        {
            err = 6 ;
        }
    }
    return err ;
}

/*----------------------------------------------------------------------*/
/* Update Modbus Config From PC to DataFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlash_ModbusConfig_From_PC(uint8_t mbregister, uint8_t mbslaveid, uint8_t mbbaudrate, uint8_t address, uint16_t timeout)
{
    uint8_t err = 0  ;

    uint8_t datah = 0 ;
    uint8_t datal = 0 ;
    uint8_t datahnew = 0 ;
    uint8_t datalnew = 0 ;
    uint16_t timeoutnew = 0 ;

    if (mbslaveid != ClientDataFlash[1].Modbus_SlaveID)
    {
        SIM_EEPROM_WRITE(8, mbslaveid);
        ClientDataFlash[1].Modbus_SlaveID = SIM_EEPROM_READ(8);
        if (ClientDataFlash[1].Modbus_SlaveID != mbslaveid)
        {
            err = 1 ;
        }
    }

    if (address != ClientDataFlash[1].Modbus_Address)
    {
        SIM_EEPROM_WRITE(11, address);
        ClientDataFlash[1].Modbus_Address = SIM_EEPROM_READ(11);
        if (ClientDataFlash[1].Modbus_Address != address)
        {
            err = 2 ;
        }
    }

    if (mbregister != ClientDataFlash[1].Modbus_Register)
    {
        SIM_EEPROM_WRITE(10, mbregister);
        ClientDataFlash[1].Modbus_Register = SIM_EEPROM_READ(10);
        if (ClientDataFlash[1].Modbus_Register != mbregister)
        {
            err = 3 ;
        }
    }

    if (timeout != ClientDataFlash[1].Modbus_Timeout)
    {
        datah = ((timeout >> 8) & 0xFF);
        datal = (timeout & 0xFF); // Byte th?p nh?t
        SIM_EEPROM_WRITE(12, datah);
        SIM_EEPROM_WRITE(13, datal);

        datahnew = SIM_EEPROM_READ(12);
        datalnew = SIM_EEPROM_READ(13);
        timeoutnew = (datahnew << 8) | datalnew;
        ClientDataFlash[1].Modbus_Timeout = timeoutnew;

        if (ClientDataFlash[1].Modbus_Timeout != timeoutnew)
        {
            err = 4 ;
        }
    }

    if (mbbaudrate != ClientDataFlash[1].Modbus_Baudrate)
    {
        SIM_EEPROM_WRITE(9, mbbaudrate);
        ClientDataFlash[1].Modbus_Baudrate = SIM_EEPROM_READ(9);
        if (ClientDataFlash[1].Modbus_Baudrate != mbbaudrate)
        {
            err = 5 ;
        }
    }
    return err ;
}


