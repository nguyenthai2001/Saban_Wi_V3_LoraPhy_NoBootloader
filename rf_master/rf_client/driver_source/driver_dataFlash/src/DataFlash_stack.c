
#include "DataFlash_stack.h"

/*---------------------------------------------------------------------------------------*/
/* Data Falsh Init and clientDataFlash update  */
/*---------------------------------------------------------------------------------------*/
void DataFlash_Client_Init( Saban *t_device,Saban_Client_Dataflash *client_pkg)
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
        //printf("Failed to set Data Flash base address!\n");
        while (1) {}
    }
    /* Read Data Flash base address */
    BaseAddr = FMC_ReadDataFlashBaseAddr();
    //printf("[Simulate EEPROM base address at Data Flash 0x%x]\n\n", BaseAddr);

    client_pkg->System_code = SIM_EEPROM_READ(0);
    client_pkg->SlaveID = SIM_EEPROM_READ(1);
    client_pkg->input_type = SIM_EEPROM_READ(2);
    client_pkg->output_number = SIM_EEPROM_READ(3);

    byte1 = SIM_EEPROM_READ(4);
    byte2 = SIM_EEPROM_READ(5);
    byte3 = SIM_EEPROM_READ(6);
    byte4 = SIM_EEPROM_READ(7);
    timeflash = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    client_pkg->timefalsh = timeflash ;

    client_pkg->RfFrequence = SIM_EEPROM_READ(14);
    client_pkg->RFPower = SIM_EEPROM_READ(15);
    client_pkg->RFBandwidth = SIM_EEPROM_READ(16);
    client_pkg->RFSpreadingFactor = SIM_EEPROM_READ(17);
    client_pkg->ErrCode            = SIM_EEPROM_READ(18);

    client_pkg->Modbus_SlaveID = SIM_EEPROM_READ(8) ;
    client_pkg->Modbus_Baudrate = SIM_EEPROM_READ(9);
    client_pkg->Modbus_Register = SIM_EEPROM_READ(10);
    client_pkg->Modbus_Address = SIM_EEPROM_READ(11);
    client_pkg->Modbus_Timeout = SIM_EEPROM_READ(12) << 8 | SIM_EEPROM_READ(13);

    switch (client_pkg->Modbus_Register)
    {
    case 0x00 :
        t_device->Modbus_mode = 0 ;
        break ;       // modebus slave
    case 0x01 :
        t_device->Modbus_mode = 0 ;
        break ;
    case 0x02 :
        t_device->Modbus_mode = 0 ;
        break ;
    case 0x03 :
        t_device->Modbus_mode = 0 ;
        break ;

    case 0x10 :
        t_device->Modbus_mode = 1 ;
        break ;        // modbus master
    case 0x11 :
        t_device->Modbus_mode = 1 ;
        break ;
    case 0x12 :
        t_device->Modbus_mode = 1 ;
        break ;
    case 0x13 :
        t_device->Modbus_mode = 1 ;
        break ;
    }
}

/*-----------------------------------------------------------------------------*/
/* Update DataFlash from Master  */
/*-----------------------------------------------------------------------------*/
void Update_DataFlash_From_Master(Saban *t_device , Saban_Client_Dataflash *client_pkg)
{
    uint8_t SystemCode ;
    //unsigned char MasterID ;
    unsigned char Commnad ;
    unsigned char MCCode ;
    unsigned char OutputNumber ;
    uint8_t SystemCodeNew ;

    SystemCode = client_pkg->System_code ;
    // masterid command mccode
    //MasterID = (SystemCode >> 4)&0x0F ;            // const
    Commnad = (SystemCode >> 2) & 0x03 ;
    MCCode = SystemCode & 0x03 ;

    if (Commnad != t_device->cmd || MCCode != t_device->mccode)
    {
        SystemCodeNew = (t_device->masterID << 4) | (t_device->cmd << 2) | t_device->mccode ;
        SIM_EEPROM_WRITE(0, SystemCodeNew);
        client_pkg->System_code = SystemCodeNew;
        //printf("Update Data Flash Done !!! ");
    }
    OutputNumber = client_pkg->output_number;
    if (OutputNumber != t_device->data_l)
    {
        SIM_EEPROM_WRITE(3, t_device->data_l);
        client_pkg->output_number = SIM_EEPROM_READ(3);
        //printf("Update Data Flash Done !!! ");
    }
}

/*----------------------------------------------------------------------*/
/* Update DataFlash From PC for InputType and TimeFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlash_InputType_From_PC(Saban_Client_Dataflash *client_pkg,uint8_t inputtype, uint32_t timeflash)
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

    if (inputtype != client_pkg->input_type)
    {
        SIM_EEPROM_WRITE(2, inputtype);
        client_pkg->input_type = SIM_EEPROM_READ(2);
        if (client_pkg->input_type != inputtype)
        {
            err = 1 ;
        }
    }
    if (timeflash != client_pkg->timefalsh)
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
        client_pkg->timefalsh = timeflashnew ;

        if (client_pkg->timefalsh != timeflash)
        {
            err = 2 ;
        }
    }
    return err ;
}

/*-----------------------------------------------------------------------------------*/
/* Update DataFlash from PC for MasterID and ClientID  */
/*-----------------------------------------------------------------------------------*/
uint8_t Update_DataFlash_ID_From_PC(Saban_Client_Dataflash *client_pkg,uint8_t data1, uint8_t data2)
{
    uint8_t err = 0 ;

    if (data1 != client_pkg->System_code)
    {
        SIM_EEPROM_WRITE(0, data1);
        client_pkg->System_code = SIM_EEPROM_READ(0) ;
        if (data1 != client_pkg->System_code)
        {
            err = 1 ;
        }
    }
    if (data2 != client_pkg->SlaveID)
    {
        SIM_EEPROM_WRITE(1, data2);
        client_pkg->SlaveID = SIM_EEPROM_READ(1) ;
        if (data2 != client_pkg->SlaveID)
        {
            err = 2 ;
        }
    }
    return err ;
}

/*******************************************************************************/
/* Update RF Config from PC to DataFlash */
/*******************************************************************************/
uint8_t Update_DataFlash_RFConfig_From_PC(Saban_Client_Dataflash *client_pkg,uint8_t freg, uint8_t power,
                                          uint8_t bw, uint8_t sf, uint8_t errcode, uint16_t rxtimeout)
{
    uint8_t err = 0 ;

    uint8_t byteh = 0;
    uint8_t bytel = 0;
    uint16_t rxtimeoutnew = 0;
    uint8_t bytehnew = 0;
    uint8_t bytelnew = 0;

    if (freg != client_pkg->RfFrequence)
    {
        SIM_EEPROM_WRITE(14, freg);
        client_pkg->RfFrequence = SIM_EEPROM_READ(14);
        if (client_pkg->RfFrequence != freg)
        {
            err = 1 ;
        }
    }

    if (power != client_pkg->RFPower)
    {
        SIM_EEPROM_WRITE(15, power);
        client_pkg->RFPower = SIM_EEPROM_READ(15);
        if (client_pkg->RFPower != power)
        {
            err = 2 ;
        }
    }

    if (bw != client_pkg->RFBandwidth)
    {
        SIM_EEPROM_WRITE(16, bw);
        client_pkg->RFBandwidth = SIM_EEPROM_READ(16);
        if (client_pkg->RFBandwidth != bw)
        {
            err = 3 ;
        }
    }

    if (sf != client_pkg->RFSpreadingFactor)
    {
        SIM_EEPROM_WRITE(17, sf);
        client_pkg->RFSpreadingFactor = SIM_EEPROM_READ(17);
        if (client_pkg->RFSpreadingFactor != sf)
        {
            err = 4 ;
        }
    }

    if (errcode != client_pkg->ErrCode)
    {
        SIM_EEPROM_WRITE(18, errcode);
        client_pkg->ErrCode = SIM_EEPROM_READ(18);
        if (client_pkg->ErrCode != errcode)
        {
            err = 5 ;
        }
    }

    if (rxtimeout != client_pkg->RXTimeout)
    {
        byteh = ((rxtimeout >> 8) & 0xFF);
        bytel = (rxtimeout & 0xFF); // Byte th?p nh?t
        SIM_EEPROM_WRITE(19, byteh);
        SIM_EEPROM_WRITE(20, bytel);

        bytehnew = SIM_EEPROM_READ(19);
        bytelnew = SIM_EEPROM_READ(20);
        rxtimeoutnew = (bytehnew << 8) | bytelnew;
        client_pkg->RXTimeout  = rxtimeoutnew ;

        if (client_pkg->RXTimeout != rxtimeoutnew)
        {
            err = 6 ;
        }
    }
    return err ;
}

/*----------------------------------------------------------------------*/
/* Update Modbus Config From PC to DataFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlash_ModbusConfig_From_PC(Saban_Client_Dataflash *client_pkg ,uint8_t mbregister, uint8_t mbslaveid, 
                                               uint8_t mbbaudrate, uint8_t address, uint16_t timeout)
{
    uint8_t err = 0  ;

    uint8_t datah = 0 ;
    uint8_t datal = 0 ;
    uint8_t datahnew = 0 ;
    uint8_t datalnew = 0 ;
    uint16_t timeoutnew = 0 ;

    if (mbslaveid != client_pkg->Modbus_SlaveID)
    {
        SIM_EEPROM_WRITE(8, mbslaveid);
        client_pkg->Modbus_SlaveID = SIM_EEPROM_READ(8);
        if (client_pkg->Modbus_SlaveID != mbslaveid)
        {
            err = 1 ;
        }
    }

    if (address != client_pkg->Modbus_Address)
    {
        SIM_EEPROM_WRITE(11, address);
        client_pkg->Modbus_Address = SIM_EEPROM_READ(11);
        if (client_pkg->Modbus_Address != address)
        {
            err = 2 ;
        }
    }

    if (mbregister != client_pkg->Modbus_Register)
    {
        SIM_EEPROM_WRITE(10, mbregister);
        client_pkg->Modbus_Register = SIM_EEPROM_READ(10);
        if (client_pkg->Modbus_Register != mbregister)
        {
            err = 3 ;
        }
    }

    if (timeout != client_pkg->Modbus_Timeout)
    {
        datah = ((timeout >> 8) & 0xFF);
        datal = (timeout & 0xFF); // Byte th?p nh?t
        SIM_EEPROM_WRITE(12, datah);
        SIM_EEPROM_WRITE(13, datal);

        datahnew = SIM_EEPROM_READ(12);
        datalnew = SIM_EEPROM_READ(13);
        timeoutnew = (datahnew << 8) | datalnew;
        client_pkg->Modbus_Timeout = timeoutnew;

        if (client_pkg->Modbus_Timeout != timeoutnew)
        {
            err = 4 ;
        }
    }

    if (mbbaudrate != client_pkg->Modbus_Baudrate)
    {
        SIM_EEPROM_WRITE(9, mbbaudrate);
        client_pkg->Modbus_Baudrate = SIM_EEPROM_READ(9);
        if (client_pkg->Modbus_Baudrate != mbbaudrate)
        {
            err = 5 ;
        }
    }
    return err ;
}


