#include "dataflash_driver.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Simulate EEPROM Read Function. Address could be a byte-alignment address.                               */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t SIM_EEPROM_READ(uint32_t address)
{
    uint32_t u32Data;

    u32Data = FMC_Read(((address / 4) * 4) + EEPROM_BASE);
    return ((u32Data >> ((address % 4) * 8)) & 0xFF);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Simulate EEPROM Write Function. Address could be a byte-alignment address.                              */
/*---------------------------------------------------------------------------------------------------------*/
void SIM_EEPROM_WRITE(uint32_t address, uint8_t data)
{
    uint32_t i, u32Data;
    uint32_t u32Start, u32End, au32DataBuf[FMC_FLASH_PAGE_SIZE / 4], *pu32DataBuf;

    if ((SIM_EEPROM_READ(address) == 0xFF) || (SIM_EEPROM_READ(address) == data))
    {
        /* Original flash data is 0xFF, or data is matched. Do not perform flash page erase. */

        /* Read oroginal data */
        u32Data = FMC_Read(((address / 4) * 4) + EEPROM_BASE);
        /* Assign 32-bit updated data */
        u32Data = ~(0xFF << ((address % 4) * 8));
        u32Data |= (data << ((address % 4) * 8));
        /* Write updated data */
        FMC_Write((((address / 4) * 4) + EEPROM_BASE), u32Data);
    }
    else
    {
        /* Need to update one page flash data. Perform flash page erase is mandatory before writing updated data. */

        /* Assign 32-bit updated data */
        u32Data = FMC_Read(((address / 4) * 4) + EEPROM_BASE);
        u32Data &= ~(0xFF << ((address % 4) * 8));
        u32Data |= (data << ((address % 4) * 8));

        /* Dump target page data to SRAM buffer */
        u32Start = (EEPROM_BASE + ((address / FMC_FLASH_PAGE_SIZE) * FMC_FLASH_PAGE_SIZE)); // page-size alignment
        u32End   = (u32Start + FMC_FLASH_PAGE_SIZE);
        pu32DataBuf = (uint32_t *)au32DataBuf;

        for (i = u32Start; i < u32End; i += 4)
        {
            *pu32DataBuf++ = FMC_Read(i);
        }

        /* Erase target page */
        FMC_Erase(EEPROM_BASE + ((address / FMC_FLASH_PAGE_SIZE)*FMC_FLASH_PAGE_SIZE));

        /* Set updated data to SRAM buffer */
        au32DataBuf[((address % FMC_FLASH_PAGE_SIZE) / 4)] = u32Data;

        /* Write target flash data from SRAM buffer */
        pu32DataBuf = (uint32_t *)au32DataBuf;

        for (i = u32Start; i < u32End; i += 4)
        {
            FMC_Write(i, *pu32DataBuf++);
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Allocate a Data Flash region to simulate EEPROM                                                         */
/*---------------------------------------------------------------------------------------------------------*/
int32_t SetDataFlashBase(uint32_t u32DFBA)
{
    uint32_t   au32Config[2];

    /* Read current User Configuration */
    if (FMC_ReadConfig(au32Config, 2) < 0)
    {
        printf("\nRead User Config failed!\n");
        return -1;
    }

    /* Just return when Data Flash has been enabled */
    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == u32DFBA))
        return 0;

    /* Enable User Configuration Update */
    FMC_ENABLE_CFG_UPDATE();

    /* Write User Configuration to Enable Data Flash */
    au32Config[0] &= ~0x1;
    au32Config[1] = u32DFBA;

    if (FMC_WriteConfig(au32Config, 2) < 0)
        return -1;

    // Perform chip reset to make new User Config take effect
    SYS->IPRSTC1 = SYS_IPRSTC1_CHIP_RST_Msk;
    return 0;
}


