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
#ifndef __usb_data_H
#define __usb_data_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
//#include "stm32f1xx_hal.h"
#define USB_BUF_SIZE   600
#define USB_SEND_SIZE   63
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#pragma pack(1)
typedef struct 
{
    uint16_t head;
    uint16_t tail;
    uint8_t buf[USB_BUF_SIZE];
}_UsbData;

typedef struct
{
	uint16_t index;
	uint16_t cmd;
	uint16_t len;
	uint8_t buf[USB_BUF_SIZE];
}_ProDeal;
#pragma pack()
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
void Usb_Data_Init(void);
void Usb_Buf_In(uint8_t * buf,uint32_t length);
void USB_Data_Out(void);
void Pro_Parse(_ProDeal TProData);
void Usb_Pro_Pack(uint16_t cmd ,uint8_t *buf,uint16_t length);
void Usb_Pro_PackT(uint16_t cmd ,uint16_t ResponeCmd,uint8_t *buf,uint16_t length);
uint8_t Usb_Send_Data(uint8_t *buf,uint16_t length);
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
