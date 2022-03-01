/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __driver_ucan_H
#define __driver_ucan_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

//#include "stm32f1xx_hal.h"
#include "stm32g4xx_hal.h"

	
#define   CAN2_POWER_ON			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define   CAN2_POWER_OFF		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define   CAN3_POWER_ON			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
#define   CAN3_POWER_OFF		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
#define   CAN2_EN			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define   CAN2_EN_OFF		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define   CAN3_EN			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define   CAN3_EN_OFF		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

	typedef struct 
{
	  uint8_t CANFirstFlag;
    uint8_t  CANMode;
    uint32_t MaskCode;
    uint16_t BandRate;
	  uint16_t prescaler;
	  uint8_t SJW;
	  uint8_t bs1;
	  uint8_t bs2;
}_CANPara;
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
uint8_t MX_CAN3_Init(_CANPara CANTPara);
uint8_t MX_CAN2_Init(_CANPara CANTPara);
//uint8_t USER_CAN3_Send(FDCAN_TxHeaderTypeDef TTHead,uint8_t *Buf );
//uint8_t USER_CAN2_Send(FDCAN_TxHeaderTypeDef TTHead,uint8_t *Buf);
void Print_Test(void);
void CAN2_Data_Pack(FDCAN_RxHeaderTypeDef TRX,uint8_t *buf);
void CAN3_Data_Pack(FDCAN_RxHeaderTypeDef TRX,uint8_t *buf);
uint8_t USER_CAN3_Send(FDCAN_TxHeaderTypeDef TXHead,uint8_t *Buf);
uint8_t USER_CAN2_Send(FDCAN_TxHeaderTypeDef TXHead,uint8_t *Buf);
uint8_t lilei_can2_send(FDCAN_TxHeaderTypeDef TTHead);

void Driver_CANPara_Init(void);





extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;
extern _CANPara  Can2Para;
extern _CANPara  Can3Para;
extern FDCAN_TxHeaderTypeDef     TxHeader2;
extern FDCAN_TxHeaderTypeDef     TxHeader3;
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
