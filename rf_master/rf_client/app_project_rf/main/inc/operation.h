/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OPERATION_H__
#define __OPERATION_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

//typedef struct
//{
//    uint8_t machineCode[3];
//    uint8_t line;
//    uint8_t lane ;
//    uint8_t partNumber[20];
//    uint8_t slot ;
//    uint8_t Number[2] ;
//    uint8_t level ;
//    uint8_t status[5] ;
//	  uint8_t user[20];
//} pkg_feedback_status_request;

typedef struct
{
    char machineCode[3];  // 3 k� t? cho m� m�y (kh�ng c?n k� t? null)
    char line;             // 1 k� t? cho line
    char lane;             // 1 k� t? cho lane
    char partNumber[18];   // 20 k� t? cho s? ph?n (kh�ng c?n k� t? null)
    char slot;             // 1 k� t? cho slot
    char Number[2];       // 2 k� t? (kh�ng c?n k� t? null)
    char level[5];        // 5 k� t? cho level (kh�ng c?n k� t? null)
    char status[5];       // 5 k� t? cho status (kh�ng c?n k� t? null)
    char user[10];        // 20 k� t? cho ngu?i d�ng (kh�ng c?n k� t? null)
} pkg_feedback_status_request;

typedef struct
{
    uint8_t HMI_User[30];
    uint8_t HMI_Pass[30];
} data_user_pass;

/* USER CODE BEGIN Prototypes */

void Radio_Start(Saban_Client_Dataflash *client_pkg);
void OnClient(Saban *t_device , Saban_Client_Dataflash *client_pkg);
void Modbus_Init(Saban *t_device,Saban_Client_Dataflash *client_pkg);
uint8_t Modbus_Start(Saban *t_device , Saban_Client_Dataflash *client_pkg);
void Modbus_Test_PC(Saban *t_device , Saban_Client_Dataflash *client_pkg);
void Client_Get_HMI_Data(uint16_t t_modbus_result[]);
void save_modbus_to_packet_feedback_status_request(pkg_feedback_status_request *pkg, uint16_t modbus_result[]);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__OPERATION_H__ */