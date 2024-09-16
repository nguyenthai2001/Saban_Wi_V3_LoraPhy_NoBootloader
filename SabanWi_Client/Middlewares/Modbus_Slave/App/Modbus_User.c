#include "stdio.h"
#include "stdint.h"
#include "NUC200Series.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "port.h"
#include "mbconfig.h"
#include <string.h>
#include <stdarg.h>
#include "mbutils.h"
#include "Modbus_User.h"
 
/* Important Note
 * If test modbus with ModbusPoll, ModbusSlave Software. You need tick to check box "PLC Address (Base1)"
 * and the following REG_START_ADDRESS is not < 1
*/

/* ----------------------- Static variables ---------------------------------*/
 
 USHORT   usRegInputStart = REG_INPUT_START;
 USHORT   usRegInputBuf[REG_INPUT_NREGS];                           // Mảng 16bit lưu giá trị của Register Input (Read Only)
 
 USHORT   usRegHoldingStart = REG_HOLDING_START;
 USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];                   // Mảng 16bit lưu giá trị của Register Holding (Read/Write)
 
 USHORT     usCoilStart = REG_COIL_START;
 UCHAR  usCoilBuf[(REG_COIL_NREGS/8)+1];                                    // Mảng 1 bit lưu giá trị của Output Coil  (Read/Write )
 
 USHORT     usDescreteStart = REG_DESCRETES_INPUT_START;
 UCHAR  usDescreteBuf[(REG_DESCRETES_INPUT_NREGS / 8)+1];       // Mảng 1bit  lưu giá trị của Descretes Input (Read Only)
 
//static UCHAR Vendor[3] = "MCL";
 const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };


void ShowMBErrStatus( eMBErrorCode eStatus) ;

/* ------------------------ Function defination --------------------------------*/

/*
 * UseModbus()
 * This function Ininitialize Modbus Protocol and Enable it
 * Define Use ASCII or RTU in "mbconfig.h"
*/
eMBErrorCode UseModbus(void)
{
    #if USE_INIT_DEBUG > 0
    S_DBG("\r\nInit HMI Modbus");
    #endif
     
        eMBErrorCode    eStatus;
        /* Initialize Protocol Stack. */
          
        #if MB_ASCII_ENABLED > 0
        /* ------- Initialize modbus ASCII------ */
        if( ( eStatus = eMBInit( MB_ASCII, 0x01, (UCHAR)0, 115200, MB_PAR_NONE ) ) != MB_ENOERR ) // 0xAA = 170
        {
            #ifdef USE_DEBUG_MB
            ShowMBErrStatus(eStatus);
            #endif
            return eStatus;
        }
        #endif
              
        // Lưu ý đối với Modbus RTU : Dùng modbus RTU dễ bị nhiễu, nhận sai dữ liệu, vì vậy phần cứng phải ổn định
        // Nên sử dụng các chân Hardware handshake controll để điều khiển luồng dữ liệu
        #if MB_RTU_ENABLED >0 
        /* ---------- Initialize Modbus RTU -------- */
        if( ( eStatus = eMBInit( MB_RTU, 0x01,0,115200, MB_PAR_NONE ) ) != MB_ENOERR ) // Slave Address = 0x01
        {
            #ifdef USE_DEBUG_MB
            ShowMBErrStatus(eStatus);
            #endif
            return eStatus;
        }
        #endif
        
        //        /* ---- Set Slave ID ---- */
        if ((eStatus = eMBSetSlaveID(0x1,TRUE,ucSlaveID,3)) != MB_ENOERR)
        {
            #ifdef USE_DEBUG_MB
            ShowMBErrStatus(eStatus);
            #endif
            return eStatus;
        }
         
        /* ----- Enable Modbus ---- */
        if( ( eStatus = eMBEnable() ) != MB_ENOERR )
        {
            #ifdef USE_DEBUG_MB
            ShowMBErrStatus(eStatus);
            #endif
            return eStatus;
        }
         
        return eStatus;
}

#ifdef USE_DEBUG_MB
 
void ShowMBErrStatus( eMBErrorCode eStatus)
{
        switch(eStatus)
        {
            case MB_ENOERR:
                printf("MB: No error");
                break;
            case MB_ENOREG:
                printf("MB: illegal reg address");
                break;
            case MB_EINVAL:
                printf("MB: illegal argument");
                break;
            case MB_EPORTERR:
                printf("MB: porting layer error");
                break;
            case MB_ENORES:
                printf("MB: insufficient resources");
                break;
            case MB_EIO:
                printf("MB: I/O error");
                break;
            case MB_EILLSTATE:
                printf("MB: protocol stack in illegal state");
                break;
            case MB_ETIMEDOUT:
                printf("MB: timeout error occurred");
                break;
 
            default: 
                printf("Err: Unknown");
                break;
        }
}
#endif

#ifdef REGION_MB_REG_PROCESS
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT             iRegIndex;
 
    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( USHORT )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
 
    return eStatus;
}
  
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT             iRegIndex;
 
    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( USHORT )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;
 
            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/*
 * Hàm eMBRegCoilsCB
 * Viết bởi facebook.com/microcontroler, copy từ hàm eMBRegHoldingCB rồi chỉnh sửa
 * do cùng đặc điểm là có khả năng cả ghi cả đọc
 * Hàm này tác động trực tiếp vào mảng dữ liệu usCoilBuf[]
*/
 
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
   eMBErrorCode    eStatus = MB_ENOERR;
   int             iRegIndex;
 
   if( ( usAddress >= REG_COIL_START ) &&
       ( usAddress + usNCoils <= REG_COIL_START + REG_COIL_NREGS ) )
   {
       iRegIndex = ( int )( usAddress - usCoilStart );
       switch ( eMode )
       {
           /* Pass current register values to the protocol stack. */
       case MB_REG_READ:
           while( usNCoils > 0 )
           {
                UCHAR ucResult = xMBUtilGetBits( usCoilBuf, iRegIndex, 1 );              
                xMBUtilSetBits( pucRegBuffer, iRegIndex - ( usAddress - REG_COIL_START ), 1, ucResult );
 
//               *pucRegBuffer++ = ( UCHAR ) ( usCoilBuf[iRegIndex] >> 8 );
//               *pucRegBuffer++ = ( UCHAR ) ( usCoilBuf[iRegIndex] & 0xFF );
               iRegIndex++;
               usNCoils--;
           }
           break;
 
           /* Update current register values with new values from the
            * protocol stack. */
       case MB_REG_WRITE:
           while( usNCoils > 0 )
           {
                  UCHAR ucResult = xMBUtilGetBits( pucRegBuffer, iRegIndex - ( usAddress - REG_COIL_START ), 1 );
                  xMBUtilSetBits( usCoilBuf, iRegIndex, 1, ucResult );
//               usCoilBuf[iRegIndex] = *pucRegBuffer++ << 8;
//               usCoilBuf[iRegIndex] |= *pucRegBuffer++;
               iRegIndex++;
               usNCoils--;
           }
       }
   }
   else
   {
       eStatus = MB_ENOREG;
   }
   return eStatus;   
}

/*
 * Hàm eMBRegDiscreteCB
 * Hàm này thao tác với chức năng đọc Descretes Input
 * Write: facebook.com/microcontroler
 * Copy từ hàm eMBRegInputCB do cùng đặc điểm là thanh ghi chỉ đọc
 * Hàm này tác động trực tiếp vào mảng usDescreteBuf[]
*/
 
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
   eMBErrorCode    eStatus = MB_ENOERR;
   int             iRegIndex;
 
//   usRegInputBuf[0] = (GPIOD->ODR & 0xF000);
//   usRegInputBuf[0] = usRegInputBuf[0] >> 12;
//   usRegInputBuf[1] = usRegInputBuf[0];
 
   if( ( usAddress >= REG_DESCRETES_INPUT_START )
       && ( usAddress + usNDiscrete <= REG_DESCRETES_INPUT_START + REG_DESCRETES_INPUT_NREGS) )
   {
       iRegIndex = ( int )( usAddress - usDescreteStart );
       while( usNDiscrete > 0 )
       {
            UCHAR ucResult = xMBUtilGetBits( usDescreteBuf, iRegIndex, 1 );
              
            xMBUtilSetBits( pucRegBuffer, iRegIndex - ( usAddress - usDescreteStart ), 1, ucResult );
//           *pucRegBuffer++ = ( unsigned char )( usDescreteBuf[iRegIndex] >> 8 );
//           *pucRegBuffer++ = ( unsigned char )( usDescreteBuf[iRegIndex] & 0xFF );
           iRegIndex++;
           usNDiscrete--;
       }
   }
   else
   {
       eStatus = MB_ENOREG;
   }
   return eStatus;
}
 
#endif

#ifdef REGION_ACCESS_MB_BIT_REG

/*
 * MBSetCoil
 * Hàm này dùng để ghi giá trị 1 coil nào đó (giá trị 0|1)
 * Tham số bitOfset có nghĩa là vị trí của bit đó
 * Tham số thứ 2 là giá trị đặt cho Coil (0-1)
 * Hàm trả về :
 *  ACCESS_NO_ERR: Nếu không có lỗi gì
 *  ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
 *              Ví dụ: #define REC_COIL_START   30 -> Tức là vùng nhớ Coil được truy cập từ địa chỉ 30 trở lên
 *                  Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
 *                  Nếu địa chỉ > REC_COIL_START + REC_COIL_NREGS-1 thì hàm cũng trả về lỗi này
 * Write: facebook.com/microcontroler
 * Gmail: 
*/
eMBAccessDataCode MBSetCoil(USHORT bitOfset, char value)
{
    UCHAR ByteAddr=0, BitAddr=0;
    UCHAR Mybyte=0;
     
    /* ---------- Check Address Correction -------------*/
     
    #if REG_COIL_START -1 >0
    if (bitOfset < REG_COIL_START ) return ACCESS_ADDR_ERR;
    #endif
    if (bitOfset > (REG_COIL_START + REG_COIL_NREGS -1)) return ACCESS_ADDR_ERR;
     
    USHORT DeltaBitOffset = bitOfset - REG_COIL_START;
     
    ByteAddr = DeltaBitOffset/8;
    BitAddr = DeltaBitOffset %8;
 
     
    Mybyte = 1;
    Mybyte = Mybyte << (BitAddr);
    if (value == 1)
    {
            usCoilBuf[ByteAddr]|= Mybyte;
    }
    else
    {
            Mybyte = ~ Mybyte;
            usCoilBuf[ByteAddr] &= Mybyte;
    }
     
    return ACCESS_NO_ERR;
}
//
 
 
/*
 * MBGetCoil
 * Hàm này dùng để lấy giá trị của 1 coil trong thanh ghi
 * bitOfset là địa chỉ bit
 * GetValue là 1 con trỏ trả về giá trị của Bit cần biết
 * Hàm trả về :
 *  ACCESS_NO_ERR: Nếu không có lỗi gì
 *  ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
 *              Ví dụ: #define REC_COIL_START   30 -> Tức là vùng nhớ Coil được truy cập từ địa chỉ 30 trở lên
 *                  Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
 *                  Nếu địa chỉ > REC_COIL_START + REC_COIL_NREGS-1 thì hàm cũng trả về lỗi này
 * Write: facebook.com/microcontroler
 * Gmail: 
*/
eMBAccessDataCode MBGetCoil(USHORT bitOfset, UCHAR* GetValue)
{
    UCHAR ByteAddr=0, BitAddr=0;
    UCHAR Result  = 0;
     
    /* ---------- Check Address Correction -------------*/
    #if REG_COIL_START - 1 >0
    if (bitOfset < REG_COIL_START ) return ACCESS_ADDR_ERR;
    #endif
    if (bitOfset > (REG_COIL_START + REG_COIL_NREGS -1)) return ACCESS_ADDR_ERR;
     
     
    USHORT DeltaBitOffset = bitOfset - REG_COIL_START;
     
    ByteAddr = DeltaBitOffset/8;
    BitAddr = DeltaBitOffset %8;
     
    Result = usCoilBuf[ByteAddr] >> BitAddr;
    Result &= (UCHAR)1;
     
    *GetValue = Result;
     
    return ACCESS_NO_ERR;
}
//
 
 
/*
 * MBSetDescretesInputbit
 * Hàm này dùng để ghi 1 bit vào Vùng nhớ Descretes Input
 * bitOfset là vị trí bit cần set
 * value là giá trị cần set cho bit
 * Hàm trả về :
 *  ACCESS_NO_ERR: Nếu không có lỗi gì
 *  ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
 *              Ví dụ: #define REG_DESCRETES_INPUT_START   30 -> Tức là vùng nhớ DescretesInput được truy cập từ địa chỉ 30 trở lên
 *                  Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
 *                  Nếu địa chỉ > REG_DESCRETES_INPUT_START + REG_DESCRETES_INPUT_NREGS-1 thì hàm cũng trả về lỗi này
 
 * Write: facebook.com/microcontroler
*/
eMBAccessDataCode MBSetDescretesInputbit(USHORT bitOfset, char value)
{
    UCHAR ByteAddr=0, BitAddr=0;
    UCHAR Mybyte;
    /* ---------- Check Address Correction -------------*/
    #if REG_DESCRETES_INPUT_START - 1 >0
    if (bitOfset < (REG_DESCRETES_INPUT_START )) return ACCESS_ADDR_ERR;
    #endif
    if (bitOfset > (REG_DESCRETES_INPUT_START + REG_DESCRETES_INPUT_NREGS -1)) return ACCESS_ADDR_ERR;
     
     
    USHORT DeltaBitOffset = bitOfset  - REG_COIL_START;
     
    ByteAddr = DeltaBitOffset/8;
    BitAddr = DeltaBitOffset %8;
 
 
    Mybyte = 1;
    Mybyte = Mybyte << (BitAddr);
    if (value == 1)
    {
            usDescreteBuf[ByteAddr]|= Mybyte;
    }
    else
    {
            Mybyte = ~ Mybyte;
            usDescreteBuf[ByteAddr] &= Mybyte;
    }
     
    return ACCESS_NO_ERR;
 
}
//
 
/*
 * MBGetDescretesInputbit
 * Hàm này dùng để lấy giá trị của 1 bit trong thanh ghi DescretesInput (thanh ghi Bit, Master chỉ đọc)
 * bitOfset là vị trí bit cần set
 * GetValue là con trỏ nhận về giá trị của bit cần lấy.
 * Hàm trả về :
 *  ACCESS_NO_ERR: Nếu không có lỗi gì
 *  ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
 *              Ví dụ: #define REG_DESCRETES_INPUT_START   30 -> Tức là vùng nhớ DescretesInput được truy cập từ địa chỉ 30 trở lên
 *                  Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
 *                  Nếu địa chỉ > REG_DESCRETES_INPUT_START + REG_DESCRETES_INPUT_NREGS-1 thì hàm cũng trả về lỗi này
 
 * 
*/
eMBAccessDataCode MBGetDescretesInputbit(USHORT bitOfset, UCHAR* GetValue)
{
    UCHAR ByteAddr=0, BitAddr=0;
    UCHAR Result  = 0;
     
    /* ---------- Check Address Correction -------------*/
    #if REG_DESCRETES_INPUT_START - 1 > 0
    if (bitOfset < REG_DESCRETES_INPUT_START ) return ACCESS_ADDR_ERR;
    #endif
    if (bitOfset > (REG_DESCRETES_INPUT_START + REG_DESCRETES_INPUT_NREGS -1)) return ACCESS_ADDR_ERR;
     
     
    USHORT DeltaBitOffset = bitOfset - REG_DESCRETES_INPUT_START;
     
    ByteAddr = DeltaBitOffset/8;
    BitAddr = DeltaBitOffset %8;
     
    Result = usDescreteBuf[ByteAddr] >> BitAddr;
    Result &= (UCHAR)1;
     
    *GetValue = Result;
     
    return ACCESS_NO_ERR;
}
//
#endif

#ifdef REGION_ACCESS_INPUT_HOLDING_REGISTER

/*
 *  Write: facebook.com/microcontroler
 *  Hàm này dùng để trích xuất dữ liệu 16bit từ các thanh ghi modbus
 *  Ví dụ cần lấy dữ liệu 16 bit tại địa chỉ 45 của thanh ghi REG INPUT
 *  MBGetData16Bits(REG_INPUT, 45, &Value);
* Hàm trả về giá trị
*   ACCESS_NO_ERR: Nếu không có lỗi gì
*   ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
*               Ví dụ: #define REG_HOLDING_START       30 -> Tức là vùng nhớ Holding được truy cập từ địa chỉ 30 trở lên
*                   Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
*       ACCESS_WRONG_DATA_TYPE: Nghĩa là loại dữ liệu bị sai, hàm này chỉ được truy cập vào 2 thanh ghi dữ liệu là
*               REG_INPUT và REG_HOLDING
*/
eMBAccessDataCode MBGetData16Bits(MB_Data_Type DataType, USHORT Address, USHORT* Value)
{
    USHORT BeginAddress;
    USHORT LastAddress16;
     
    switch (DataType)
    {
        case    REG_INPUT:
             
            /* ---------Check Address Correction ------------ */
            BeginAddress = REG_INPUT_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR; 
            LastAddress16 = REG_INPUT_START + REG_INPUT_NREGS -1;
            if (Address > LastAddress16) return ACCESS_ADDR_ERR;
 
            /* --------Assign Data ------------*/
            *Value = usRegInputBuf[Address - BeginAddress ];
         
            break;
         
        case REG_HOLDING:
            /* ------ Check Address correction --------*/
            BeginAddress = REG_HOLDING_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR;
            LastAddress16 = REG_HOLDING_START + REG_HOLDING_NREGS -1;
            if (Address > LastAddress16) return ACCESS_ADDR_ERR;
         
            /* -------- Assign Data -------------*/
            *Value = usRegHoldingBuf[Address - BeginAddress];
         
            break;
         
        default:
            return ACCESS_WRONG_DATA_TYPE;
 
    }
    return ACCESS_NO_ERR;
}
//
 
/*
 * Write: facebook.com/microcontroler
 *  Hàm này dùng để trích xuất dữ liệu 32bit từ các thanh ghi modbus
 *  Ví dụ cần lấy dữ liệu 32 bit tại địa chỉ 45 của thanh ghi REG HOLDING
 *  MBGetData32Bits(REG_HOLDING, 45, &Value);
* Hàm trả về giá trị
*   ACCESS_NO_ERR: Nếu không có lỗi gì
*   ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
*               Ví dụ: #define REG_HOLDING_START       30 -> Tức là vùng nhớ Holding được truy cập từ địa chỉ 30 trở lên
*                   Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
*       ACCESS_WRONG_DATA_TYPE: Nghĩa là loại dữ liệu bị sai, hàm này chỉ được truy cập vào 2 thanh ghi dữ liệu là
*               REG_INPUT và REG_HOLDING
*/
eMBAccessDataCode MBGetData32Bits(MB_Data_Type DataType, USHORT Address, ULONG* Value)
{
    USHORT BeginAddress;
    USHORT LastAddress32;
     
    USHORT HightData = 0;
    USHORT LowData = 0;
    ULONG Result = 0;
    switch (DataType)
    {
        case    REG_INPUT:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_INPUT_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR; 
            LastAddress32 = REG_INPUT_START + REG_INPUT_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
 
         
            /* -------- Assign Value to Reg ----- */
            LowData = usRegInputBuf[Address - BeginAddress ];
            HightData = usRegInputBuf[Address - BeginAddress + 1];
         
            break;
        case REG_HOLDING:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_HOLDING_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR;
            LastAddress32 = REG_HOLDING_START + REG_HOLDING_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
             
            /* -------- Assign Value to Reg ----- */
            LowData = usRegHoldingBuf[Address - BeginAddress ];
            HightData = usRegHoldingBuf[Address - BeginAddress + 1];
         
            break;
        default:
            return ACCESS_WRONG_DATA_TYPE;
 
    }
    Result = ((ULONG)HightData)<<16;
    Result += LowData;
     
    *Value = Result;
     
    return ACCESS_NO_ERR;
}
//
 
/*
*   Write: facebook.com/microcontroler
*   Hàm này dùng để thay đổi dữ liệu 16bit của thanh ghi Modbus
* Ví dụ muốn gán giá trị 325 cho thanh ghi Holding tại địa chỉ 170 thì thực hiện như sau:
* MBSetData16Bits(REG_HOLDING, 170, 325);
* Hàm trả về giá trị
*   ACCESS_NO_ERR: Nếu không có lỗi gì
*   ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
*               Ví dụ: #define REG_HOLDING_START       30 -> Tức là vùng nhớ Holding được truy cập từ địa chỉ 30 trở lên
*                   Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
*       ACCESS_WRONG_DATA_TYPE: Nghĩa là loại dữ liệu bị sai, hàm này chỉ được truy cập vào 2 thanh ghi dữ liệu là
*               REG_INPUT và REG_HOLDING
*/
eMBAccessDataCode MBSetData16Bits(MB_Data_Type DataType, USHORT Address, USHORT Value)
{
    USHORT BeginAddress;
    USHORT LastAddress16;
     
    switch (DataType)
    {
        case    REG_INPUT:
            BeginAddress = REG_INPUT_START;
            if (Address < (BeginAddress -1 )) return ACCESS_ADDR_ERR; 
            LastAddress16 = REG_INPUT_START + REG_INPUT_NREGS -1;
         
            if (Address > LastAddress16) return ACCESS_ADDR_ERR;
 
            usRegInputBuf[Address - BeginAddress ] = Value;
         
            break;
        case REG_HOLDING:
            BeginAddress = REG_HOLDING_START;
            if (Address <( BeginAddress -1)) return ACCESS_ADDR_ERR;
            LastAddress16 = REG_HOLDING_START + REG_HOLDING_NREGS -1;
         
            if (Address > LastAddress16) return ACCESS_ADDR_ERR;
         
            usRegHoldingBuf[Address - BeginAddress ] = Value;
            break;
        default:
            return ACCESS_WRONG_DATA_TYPE;
 
    }
    return ACCESS_NO_ERR;
}
//
 
 
/*
*   Write: facebook.com/microcontroler
*   hàm này dùng để thay đổi dữ liệu 32bit của thanh ghi Modbus
* Ví dụ: Cần ghi giá trị 0x554433 vào địa chỉ 100 của thanh ghi HOLDING
* MBSetData32Bits(REG_HOLDING, 100, 0x554433);
* Hàm trả về giá trị
*   ACCESS_NO_ERR: Nếu không có lỗi gì
*   ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
*               Ví dụ: #define REG_HOLDING_START       30 -> Tức là vùng nhớ Holding được truy cập từ địa chỉ 30 trở lên
*                   Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
*       ACCESS_WRONG_DATA_TYPE: Nghĩa là loại dữ liệu bị sai, hàm này chỉ được truy cập vào 2 thanh ghi dữ liệu là
*               REG_INPUT và REG_HOLDING
*/
eMBAccessDataCode MBSetData32Bits(MB_Data_Type DataType, USHORT Address, ULONG Value)
{
    USHORT BeginAddress;
    USHORT LastAddress32;
     
    USHORT HightData = (Value & 0xFFFF0000)>>16;
    USHORT LowData = (Value & 0x0000FFFF);
     
    switch (DataType)
    {
        case    REG_INPUT:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_INPUT_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR; 
            LastAddress32 = REG_INPUT_START + REG_INPUT_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
 
         
            /* -------- Assign Value to Reg ----- */
        usRegInputBuf[Address - BeginAddress ] = LowData;   // Origin: HightData
        usRegInputBuf[Address - BeginAddress + 1] = HightData;  // Origin: LowData
         
            break;
        case REG_HOLDING:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_HOLDING_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR;
            LastAddress32 = REG_HOLDING_START + REG_HOLDING_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
             
            /* -------- Assign Value to Reg ----- */
            usRegHoldingBuf[Address - BeginAddress ] = LowData;
            usRegHoldingBuf[Address - BeginAddress + 1] = HightData;
         
            break;
        default:
            return ACCESS_WRONG_DATA_TYPE;
    }
    return ACCESS_NO_ERR;
}
//
 
 
/*
*   Write: facebook.com/microcontroler
*   hàm này dùng để thay đổi dữ liệu float của thanh ghi Modbus
* Ví dụ: Cần ghi giá trị 324.434 vào địa chỉ 100 của thanh ghi HOLDING
* MBSetFloatData(REG_HOLDING, 100, 324.434);
* Hàm trả về giá trị
*   ACCESS_NO_ERR: Nếu không có lỗi gì
*   ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
*               Ví dụ: #define REG_HOLDING_START       30 -> Tức là vùng nhớ Holding được truy cập từ địa chỉ 30 trở lên
*                   Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
*       ACCESS_WRONG_DATA_TYPE: Nghĩa là loại dữ liệu bị sai, hàm này chỉ được truy cập vào 2 thanh ghi dữ liệu là
*               REG_INPUT và REG_HOLDING
*/
 
typedef union _data {
  float f;
  char  s[4];
    uint32_t U32;
} myData;
 
 
eMBAccessDataCode MBSetFloatData(MB_Data_Type DataType, USHORT Address, float Value)
{
    USHORT BeginAddress;
    USHORT LastAddress32;
 
    myData q;
    q.f = Value;
     
    USHORT *HightData;
    USHORT *LowData;
     
    HightData = (USHORT*)q.s +1;
    LowData = (USHORT *)q.s ;
     
    switch (DataType)
    {
        case    REG_INPUT:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_INPUT_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR; 
            LastAddress32 = REG_INPUT_START + REG_INPUT_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
 
         
            /* -------- Assign Value to Reg ----- */
        usRegInputBuf[Address - BeginAddress ] = *LowData;   // Origin: HightData
        usRegInputBuf[Address - BeginAddress + 1] = *HightData; // Origin: LowData
         
            break;
        case REG_HOLDING:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_HOLDING_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR;
            LastAddress32 = REG_HOLDING_START + REG_HOLDING_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
             
            /* -------- Assign Value to Reg ----- */
            usRegHoldingBuf[Address - BeginAddress ] = *LowData;
            usRegHoldingBuf[Address - BeginAddress + 1] = *HightData;
         
            break;
        default:
            return ACCESS_WRONG_DATA_TYPE;
    }
    return ACCESS_NO_ERR;
}
//
 
 
/*
 * Write: 
 *  Hàm này dùng để trích xuất dữ liệu float từ thanh ghi lưu trữ dữ liệu
 *  Ví dụ cần lấy dữ liệu float  tại địa chỉ 45 của thanh ghi REG HOLDING
 *  MBGetFloatData(REG_HOLDING, 45, &Value);
* Hàm trả về giá trị
*   ACCESS_NO_ERR: Nếu không có lỗi gì
*   ACCESS_ADDR_ERR: Nếu truy cập vào 1 địa chỉ thanh ghi mà không được cấp phát bộ nhớ
*               Ví dụ: #define REG_HOLDING_START       30 -> Tức là vùng nhớ Holding được truy cập từ địa chỉ 30 trở lên
*                   Nếu địa chỉ < 30 thì Hàm sẽ trả về lỗi ACCESS_ADDR_ERR
*       ACCESS_WRONG_DATA_TYPE: Nghĩa là loại dữ liệu bị sai, hàm này chỉ được truy cập vào 2 thanh ghi dữ liệu là
*               REG_INPUT và REG_HOLDING
*/
#include <math.h>
#include <float.h>
 
eMBAccessDataCode MBGetFloatData(MB_Data_Type DataType, USHORT Address, float* Value)
{
    USHORT BeginAddress;
    USHORT LastAddress32;
    char *P;
 
     
    USHORT HightData = 0;
    USHORT LowData = 0;
 
    myData  Result;
     
    switch (DataType)
    {
        case    REG_INPUT:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_INPUT_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR; 
            LastAddress32 = REG_INPUT_START + REG_INPUT_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
 
         
            /* -------- Assign Value to Reg ----- */
            LowData = usRegInputBuf[Address - BeginAddress ];
            HightData = usRegInputBuf[Address - BeginAddress + 1];
         
            break;
        case REG_HOLDING:
             
            /*  --- Check Address Correction --- */
            BeginAddress = REG_HOLDING_START;
            if (Address < (BeginAddress -1)) return ACCESS_ADDR_ERR;
            LastAddress32 = REG_HOLDING_START + REG_HOLDING_NREGS -2;
            if (Address > LastAddress32) return ACCESS_ADDR_ERR;
             
            /* -------- Assign Value to Reg ----- */
            LowData = usRegHoldingBuf[Address - BeginAddress ];
            HightData = usRegHoldingBuf[Address - BeginAddress + 1];
         
            break;
        default:
            return ACCESS_WRONG_DATA_TYPE;
    }
 
     
    P = (char*)&LowData;
    Result.s[0] = *P;
    Result.s[1] = *(P +1);
     
    P = (char*)&HightData;
    Result.s[2] = *P;
    Result.s[3] = *(P + 1);
     
    *Value = Result.f;
    float val = Result.f;
        switch(fpclassify(*Value)) {
                case FP_INFINITE:  
                    *Value = 0; 
                    break;//return "Inf";
                case FP_NAN:       
                    *Value = 0; 
                break;//return "NaN";
                case FP_NORMAL:    
                    break;//return "normal";
                case FP_SUBNORMAL: 
                    *Value = 0; 
                    break;//return "subnormal";
                case FP_ZERO:       
                    break;//return "zero";
                default:
                    *Value = 0;
                    break;//return "unknown";
        }
     
    if (val > (float)10000.0)
        *Value = 0;
    if (val <(float)-10000.0)
        *Value = 0;
    if ((val > (float)0)&&(val < (float)0.0000001))   *Value = 0;
     
    return ACCESS_NO_ERR;
}
//
 
#endif 

