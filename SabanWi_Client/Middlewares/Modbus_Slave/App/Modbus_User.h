#ifndef _MODBUS_USER_H
#define _MODBUS_USER_H

//#define USE_DEBUG_MB
#define REGION_MB_REG_PROCESS
#define REGION_ACCESS_MB_BIT_REG
#define REGION_ACCESS_INPUT_HOLDING_REGISTER

#define REG_INPUT_START 1
#define REG_INPUT_NREGS 16

#define REG_HOLDING_START 1
#define REG_HOLDING_NREGS 100

#define REG_COIL_START 1
#define REG_COIL_NREGS 16

#define REG_DESCRETES_INPUT_START 1
#define REG_DESCRETES_INPUT_NREGS 16

typedef enum
{
    REG_INPUT,
    REG_HOLDING,
    DESCRETE_IP,
    COIL_OP
} MB_Data_Type;

typedef enum
{
    ACCESS_ADDR_ERR, // Lỗi địa chỉ truy cập
    ACCESS_NO_ERR,  // Không hề có lỗi
    ACCESS_WRONG_DATA_TYPE // Lỗi sai kiểu dữ liệu modbus
} eMBAccessDataCode;


eMBErrorCode UseModbus(void);

#ifdef REGION_MB_REG_PROCESS
    //eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs );
    //eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode );
    //eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode );
    //eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete );

#endif

#ifdef REGION_ACCESS_MB_BIT_REG
    eMBAccessDataCode MBSetCoil(USHORT bitOfset, char value);
    eMBAccessDataCode MBGetCoil(USHORT bitOfset, UCHAR* GetValue);
    eMBAccessDataCode MBSetDescretesInputbit(USHORT bitOfset, char value);
    eMBAccessDataCode MBGetDescretesInputbit(USHORT bitOfset, UCHAR* GetValue);

#endif

#ifdef REGION_ACCESS_INPUT_HOLDING_REGISTER
    eMBAccessDataCode MBGetData16Bits(MB_Data_Type DataType, USHORT Address, USHORT* Value);
    eMBAccessDataCode MBGetData32Bits(MB_Data_Type DataType, USHORT Address, ULONG* Value);
    eMBAccessDataCode MBSetData16Bits(MB_Data_Type DataType, USHORT Address, USHORT Value);

    eMBAccessDataCode MBSetData32Bits(MB_Data_Type DataType, USHORT Address, ULONG Value);
    eMBAccessDataCode MBSetFloatData(MB_Data_Type DataType, USHORT Address, float Value);
    eMBAccessDataCode MBGetFloatData(MB_Data_Type DataType, USHORT Address, float* Value);
#endif

#endif

