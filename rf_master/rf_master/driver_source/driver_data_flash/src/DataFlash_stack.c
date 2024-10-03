#include "DataFlash_stack.h"

/*---------------------------------------------------------------------------------------*/
/* Data Falsh Init and clientDataFlash update  */
/*---------------------------------------------------------------------------------------*/
void DataFlash_Master_Init(Saban *t_device, Saban_Master_Dataflash *master_pkg, Saban_Device_Dataflash device_pkg[])
{
    uint32_t BaseAddr ;
    int i = 0;

    uint8_t startPosition = 11;
    uint8_t step = 4;
    uint8_t poss ;

    /* Enable FMC ISP function */
    FMC_Open();
    /* Set Data Flash base address */
    if (SetDataFlashBase(DATA_FLASH_EEPROM_BASE) < 0)
    {
        printf("Failed to set Data Flash base address!\n");
        while (1) {}
    }
    /* Read Data Flash base address */
    BaseAddr = FMC_ReadDataFlashBaseAddr();
    printf("\n[Simulate EEPROM base address at Data Flash 0x%x]\n\n", BaseAddr);

    master_pkg->RfFrequence = SIM_EEPROM_READ(0);
    master_pkg->RFPower = SIM_EEPROM_READ(1);
    master_pkg->RFBandwidth = SIM_EEPROM_READ(2);
    master_pkg->RFSpreadingFactor = SIM_EEPROM_READ(3);
    master_pkg->ErrCode = SIM_EEPROM_READ(4);

    master_pkg->Masterid = SIM_EEPROM_READ(5);
    master_pkg->DeviceNumber = SIM_EEPROM_READ(6);

    master_pkg->Modbus_SlaveID = SIM_EEPROM_READ(7);
    master_pkg->Modbus_Baudrate = SIM_EEPROM_READ(8);
    master_pkg->Modbus_Register = SIM_EEPROM_READ(9);
    switch (master_pkg->Modbus_Register)
    {
    case 0x00 :
        t_device-> Modbus_mode = 0 ;                // Slave
        break ;
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
        t_device->Modbus_mode = 1 ;                  // master
        break ;
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
    master_pkg->Modbus_Address = SIM_EEPROM_READ(10);

    for (i = 1; i <= master_pkg->DeviceNumber ; i++)
    {
        poss  = startPosition + (i - 1) * step ;
        device_pkg[i].ClientID = SIM_EEPROM_READ(poss);
        device_pkg[i].Systemcode = SIM_EEPROM_READ(poss + 1);

        device_pkg[i].DataH = SIM_EEPROM_READ(poss + 2);
        device_pkg[i].DataL = SIM_EEPROM_READ(poss + 3);
    }
}

/*----------------------------------------------------------------------*/
/* Update device client Config From PC to DataFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlashDevice_From_PC(Saban_Device_Dataflash device_pkg[], uint8_t stt, uint8_t ClientID, uint8_t Systemcode, uint8_t datah, uint8_t datal)
{
    uint8_t err = 0 ;

    uint8_t startPosition = 11;
    uint8_t step = 4;
    uint8_t poss ;

    poss  = startPosition + (stt - 1) * step ;
    printf("\n DataFlash addr : %d", poss + 1);

    if (ClientID != device_pkg[stt].ClientID)
    {
        SIM_EEPROM_WRITE(poss, ClientID);
        device_pkg[stt].ClientID = SIM_EEPROM_READ(poss);
        if (device_pkg[stt].ClientID != ClientID)
        {
            err = 1 ;
        }
    }
    if (Systemcode != device_pkg[stt].Systemcode)
    {
        SIM_EEPROM_WRITE(poss + 1, Systemcode);
        device_pkg[stt].Systemcode = SIM_EEPROM_READ(poss + 1);
        if (device_pkg[stt].Systemcode != Systemcode)
        {
            err = 2 ;
        }
    }
    if (datah != device_pkg[stt].DataH)
    {
        SIM_EEPROM_WRITE(poss + 2, datah);
        device_pkg[stt].DataH = SIM_EEPROM_READ(poss + 2);
        if (device_pkg[stt].DataH != datah)
        {
            err = 3 ;
        }
    }
    if (datal != device_pkg[stt].DataL)
    {
        SIM_EEPROM_WRITE(poss + 3, datal);
        device_pkg[stt].DataL = SIM_EEPROM_READ(poss + 3);
        if (device_pkg[stt].DataL != datal)
        {
            err = 4 ;
        }
    }
    return err ;
}

/*----------------------------------------------------------------------*/
/* Update Modbus Config From PC to DataFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlash_ModbusConfig_From_PC(Saban_Master_Dataflash *master_pkg, uint8_t mbregister, uint8_t mbslaveid, uint8_t mbbaudrate, uint8_t address)
{
    uint8_t err = 0  ;

    if (mbregister != master_pkg[1].Modbus_Register)
    {
        SIM_EEPROM_WRITE(9, mbregister);
        master_pkg->Modbus_Register = SIM_EEPROM_READ(9);
        if (master_pkg->Modbus_Register != mbregister)
        {
            err = 3 ;
        }
    }
    if (mbslaveid != master_pkg[1].Modbus_SlaveID)
    {
        SIM_EEPROM_WRITE(7, mbslaveid);
        master_pkg->Modbus_SlaveID = SIM_EEPROM_READ(7);
        if (master_pkg->Modbus_SlaveID != mbslaveid)
        {
            err = 1 ;
        }
    }
    if (mbbaudrate != master_pkg[1].Modbus_Baudrate)
    {
        SIM_EEPROM_WRITE(8, mbbaudrate);
        master_pkg->Modbus_Baudrate = SIM_EEPROM_READ(8);
        if (master_pkg->Modbus_Baudrate != mbbaudrate)
        {
            err = 5 ;
        }
    }
    if (address !=  master_pkg->Modbus_Address)
    {
        SIM_EEPROM_WRITE(10, address);
        master_pkg->Modbus_Address = SIM_EEPROM_READ(10);
        if (master_pkg->Modbus_Address != address)
        {
            err = 2 ;
        }
    }
    return err ;
}

/*******************************************************************************/
/* Update RF Config from PC to DataFlash */
/*******************************************************************************/
uint8_t Update_DataFlash_RFConfig_From_PC(Saban_Master_Dataflash *master_pkg, uint8_t freg, uint8_t power, uint8_t bw, uint8_t sf, uint8_t errcode, uint16_t rxtimeout)
{
    uint8_t err = 0 ;

    if (freg !=  master_pkg->RfFrequence)
    {
        SIM_EEPROM_WRITE(0, freg);
        master_pkg->RfFrequence = SIM_EEPROM_READ(0);
        if (master_pkg->RfFrequence != freg)
        {
            err = 1 ;
        }
    }

    if (power !=  master_pkg->RFPower)
    {
        SIM_EEPROM_WRITE(1, power);
        master_pkg->RFPower = SIM_EEPROM_READ(1);
        if (master_pkg->RFPower != power)
        {
            err = 2 ;
        }
    }

    if (bw !=  master_pkg->RFBandwidth)
    {
        SIM_EEPROM_WRITE(2, bw);
        master_pkg->RFBandwidth = SIM_EEPROM_READ(2);
        if (master_pkg->RFBandwidth != bw)
        {
            err = 3 ;
        }
    }

    if (sf !=  master_pkg->RFSpreadingFactor)
    {
        SIM_EEPROM_WRITE(3, sf);
        master_pkg->RFSpreadingFactor = SIM_EEPROM_READ(3);
        if (master_pkg->RFSpreadingFactor != sf)
        {
            err = 4 ;
        }
    }

    if (errcode !=  master_pkg->ErrCode)
    {
        SIM_EEPROM_WRITE(4, errcode);
        master_pkg->ErrCode = SIM_EEPROM_READ(4);
        if (master_pkg->ErrCode != errcode)
        {
            err = 5 ;
        }
    }
    return err ;
}

/*-----------------------------------------------------------------------------------*/
/* Update DataFlash from PC for MasterID and ClientID  */
/*-----------------------------------------------------------------------------------*/
uint8_t Update_DataFlash_ID_From_PC(Saban_Master_Dataflash *master_pkg, uint8_t data1, uint8_t data2)
{
    uint8_t err = 0 ;
    if (data1 !=  master_pkg->Masterid)
    {
        SIM_EEPROM_WRITE(5, data1);
        master_pkg->Masterid  = SIM_EEPROM_READ(5);
        if (master_pkg->Masterid != data1)
        {
            err = 1 ;
        }
    }

    if (data2 !=  master_pkg->DeviceNumber)
    {
        SIM_EEPROM_WRITE(6, data2);
        master_pkg->DeviceNumber  = SIM_EEPROM_READ(6);
        if (master_pkg->DeviceNumber != data2)
        {
            err = 2 ;
        }
    }
    return err ;
}
