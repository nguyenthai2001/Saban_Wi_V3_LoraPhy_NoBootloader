#include "DataFlash_stack.h"

Saban_Master_Dataflash MasterDataFlash[2];
Saban_Device_Dataflash DeviceDataFlash[200];

/*---------------------------------------------------------------------------------------*/
/* Data Falsh Init and clientDataFlash update  */
/*---------------------------------------------------------------------------------------*/
void DataFlash_Master_Init(void)
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

    MasterDataFlash[1].RfFrequence = SIM_EEPROM_READ(0);
    MasterDataFlash[1].RFPower = SIM_EEPROM_READ(1);
    MasterDataFlash[1].RFBandwidth = SIM_EEPROM_READ(2);
    MasterDataFlash[1].RFSpreadingFactor = SIM_EEPROM_READ(3);
    MasterDataFlash[1].ErrCode = SIM_EEPROM_READ(4);

    MasterDataFlash[1].Masterid = SIM_EEPROM_READ(5);
    MasterDataFlash[1].DeviceNumber = SIM_EEPROM_READ(6);

    MasterDataFlash[1].Modbus_SlaveID = SIM_EEPROM_READ(7);
    MasterDataFlash[1].Modbus_Baudrate = SIM_EEPROM_READ(8);
    MasterDataFlash[1].Modbus_Register = SIM_EEPROM_READ(9);
    switch (MasterDataFlash[1].Modbus_Register)
    {
    case 0x00 :
        device[1].Modbus_mode = 0 ;                // Slave
        break ;
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
        device[1].Modbus_mode = 1 ;                  // master
        break ;
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
    MasterDataFlash[1].Modbus_Address = SIM_EEPROM_READ(10);

    for (i = 1; i <= MasterDataFlash[1].DeviceNumber ; i++)
    {
        poss  = startPosition + (i - 1) * step ;
        DeviceDataFlash[i].ClientID = SIM_EEPROM_READ(poss);
        DeviceDataFlash[i].Systemcode = SIM_EEPROM_READ(poss + 1);

        DeviceDataFlash[i].DataH = SIM_EEPROM_READ(poss + 2);
        DeviceDataFlash[i].DataL = SIM_EEPROM_READ(poss + 3);
    }

}

/*----------------------------------------------------------------------*/
/* Update device client Config From PC to DataFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlashDevice_From_PC(uint8_t stt, uint8_t ClientID, uint8_t Systemcode, uint8_t datah, uint8_t datal)
{
    uint8_t err = 0 ;

    uint8_t startPosition = 11;
    uint8_t step = 4;
    uint8_t poss ;

    poss  = startPosition + (stt - 1) * step ;
    printf("\n DataFlash addr : %d", poss + 1);

    if (ClientID != DeviceDataFlash[stt].ClientID)
    {
        SIM_EEPROM_WRITE(poss, ClientID);
        DeviceDataFlash[stt].ClientID = SIM_EEPROM_READ(poss);
        if (DeviceDataFlash[stt].ClientID != ClientID)
        {
            err = 1 ;
        }
    }
    if (Systemcode != DeviceDataFlash[stt].Systemcode)
    {
        SIM_EEPROM_WRITE(poss + 1, Systemcode);
        DeviceDataFlash[stt].Systemcode = SIM_EEPROM_READ(poss + 1);
        if (DeviceDataFlash[stt].Systemcode != Systemcode)
        {
            err = 2 ;
        }
    }
    if (datah != DeviceDataFlash[stt].DataH)
    {
        SIM_EEPROM_WRITE(poss + 2, datah);
        DeviceDataFlash[stt].DataH = SIM_EEPROM_READ(poss + 2);
        if (DeviceDataFlash[stt].DataH != datah)
        {
            err = 3 ;
        }
    }
    if (datal != DeviceDataFlash[stt].DataL)
    {
        SIM_EEPROM_WRITE(poss + 3, datal);
        DeviceDataFlash[stt].DataL = SIM_EEPROM_READ(poss + 3);
        if (DeviceDataFlash[stt].DataL != datal)
        {
            err = 4 ;
        }
    }
    return err ;
}

/*----------------------------------------------------------------------*/
/* Update Modbus Config From PC to DataFlash */
/*----------------------------------------------------------------------*/
uint8_t Update_DataFlash_ModbusConfig_From_PC(uint8_t mbregister, uint8_t mbslaveid, uint8_t mbbaudrate, uint8_t address)
{
    uint8_t err = 0  ;

    if (mbregister != MasterDataFlash[1].Modbus_Register)
    {
        SIM_EEPROM_WRITE(9, mbregister);
        MasterDataFlash[1].Modbus_Register = SIM_EEPROM_READ(9);
        if (MasterDataFlash[1].Modbus_Register != mbregister)
        {
            err = 3 ;
        }
    }
    if (mbslaveid != MasterDataFlash[1].Modbus_SlaveID)
    {
        SIM_EEPROM_WRITE(7, mbslaveid);
        MasterDataFlash[1].Modbus_SlaveID = SIM_EEPROM_READ(7);
        if (MasterDataFlash[1].Modbus_SlaveID != mbslaveid)
        {
            err = 1 ;
        }
    }
    if (mbbaudrate != MasterDataFlash[1].Modbus_Baudrate)
    {
        SIM_EEPROM_WRITE(8, mbbaudrate);
        MasterDataFlash[1].Modbus_Baudrate = SIM_EEPROM_READ(8);
        if (MasterDataFlash[1].Modbus_Baudrate != mbbaudrate)
        {
            err = 5 ;
        }
    }
    if (address != MasterDataFlash[1].Modbus_Address)
    {
        SIM_EEPROM_WRITE(10, address);
        MasterDataFlash[1].Modbus_Address = SIM_EEPROM_READ(10);
        if (MasterDataFlash[1].Modbus_Address != address)
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

    if (freg != MasterDataFlash[1].RfFrequence)
    {
        SIM_EEPROM_WRITE(0, freg);
        MasterDataFlash[1].RfFrequence = SIM_EEPROM_READ(0);
        if (MasterDataFlash[1].RfFrequence != freg)
        {
            err = 1 ;
        }
    }

    if (power != MasterDataFlash[1].RFPower)
    {
        SIM_EEPROM_WRITE(1, power);
        MasterDataFlash[1].RFPower = SIM_EEPROM_READ(1);
        if (MasterDataFlash[1].RFPower != power)
        {
            err = 2 ;
        }
    }

    if (bw != MasterDataFlash[1].RFBandwidth)
    {
        SIM_EEPROM_WRITE(2, bw);
        MasterDataFlash[1].RFBandwidth = SIM_EEPROM_READ(2);
        if (MasterDataFlash[1].RFBandwidth != bw)
        {
            err = 3 ;
        }
    }

    if (sf != MasterDataFlash[1].RFSpreadingFactor)
    {
        SIM_EEPROM_WRITE(3, sf);
        MasterDataFlash[1].RFSpreadingFactor = SIM_EEPROM_READ(3);
        if (MasterDataFlash[1].RFSpreadingFactor != sf)
        {
            err = 4 ;
        }
    }

    if (errcode != MasterDataFlash[1].ErrCode)
    {
        SIM_EEPROM_WRITE(4, errcode);
        MasterDataFlash[1].ErrCode = SIM_EEPROM_READ(4);
        if (MasterDataFlash[1].ErrCode != errcode)
        {
            err = 5 ;
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
    if (data1 != MasterDataFlash[1].Masterid)
    {
        SIM_EEPROM_WRITE(5, data1);
        MasterDataFlash[1].Masterid  = SIM_EEPROM_READ(5);
        if (MasterDataFlash[1].Masterid != data1)
        {
            err = 1 ;
        }
    }

    if (data2 != MasterDataFlash[1].DeviceNumber)
    {
        SIM_EEPROM_WRITE(6, data2);
        MasterDataFlash[1].DeviceNumber  = SIM_EEPROM_READ(6);
        if (MasterDataFlash[1].DeviceNumber != data2)
        {
            err = 2 ;
        }
    }
    return err ;
}
