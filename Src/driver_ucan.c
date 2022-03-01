
/********************************************************************************
* Include Headers
********************************************************************************/

#include "tool.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "driver_ucan.h"
#include "stm32g4xx_hal_fdcan.h"


//#define   CAN1FIFO   CAN_RX_FIFO0	           
//#define   CAN2FIFO   CAN_RX_FIFO1


//CAN_TxHeaderTypeDef     TxHeader1;
//CAN_RxHeaderTypeDef     RxMeg1;
//CAN_TxHeaderTypeDef     TxHeader2;
//CAN_RxHeaderTypeDef     RxMeg2;
//CAN_HandleTypeDef hcan1;
//CAN_HandleTypeDef hcan2;
_CANPara  Can2Para=NULL;
_CANPara  Can3Para=NULL;

uint8_t             PrintFlag=0;




FDCAN_HandleTypeDef hfdcan2=NULL;
FDCAN_HandleTypeDef hfdcan3=NULL;
FDCAN_FilterTypeDef sFilterConfig=NULL;
FDCAN_TxHeaderTypeDef TxHeader2=NULL;
FDCAN_TxHeaderTypeDef TxHeader3=NULL;
FDCAN_RxHeaderTypeDef RxHeader=NULL;
uint8_t RxData[64];
uint8_t TxData[64] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22};

unsigned char glbaoTD=0;
//uint8_t    CanData[8];
uint8_t    RCanDataBuf[256];


void Driver_CANPara_Init()
{
   memset((int8_t *)&Can3Para,0x00,sizeof(_CANPara));
	
	  memset((int8_t *)&Can2Para,0x00,sizeof(_CANPara));
	memset((int8_t *)&sFilterConfig,0x00,sizeof(FDCAN_FilterTypeDef));
	// memset((int8_t *)&TxHeaderLiLei,0x00,sizeof(FDCAN_TxHeaderTypeDef));
	  memset((int8_t *)&TxHeader2,0x00,sizeof(FDCAN_TxHeaderTypeDef));
	  memset((int8_t *)&TxHeader3,0x00,sizeof(FDCAN_TxHeaderTypeDef));
	  memset((int8_t *)&RxHeader,0x00,sizeof(FDCAN_RxHeaderTypeDef));
	
	 memset((int8_t *)&hfdcan2,0x00,sizeof(FDCAN_HandleTypeDef));
	memset((int8_t *)&hfdcan3,0x00,sizeof(FDCAN_HandleTypeDef));
	CAN2_EN;
	CAN3_EN;
	
	  /* Prepare Tx Header */

	
	TxHeader2.Identifier = 0x322;
  TxHeader2.IdType = FDCAN_STANDARD_ID;
  TxHeader2.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader2.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader2.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader2.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader2.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader2.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader2.MessageMarker = 0;
	
	TxHeader3.Identifier = 0x323;
  TxHeader3.IdType = FDCAN_STANDARD_ID;
  TxHeader3.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader3.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader3.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader3.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader3.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader3.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader3.MessageMarker = 0;
	
	
	glbaoTD=88;


}
uint8_t MX_CAN2_Init(_CANPara CANTPara)
{	
	
	uint8_t CAN2SetFlag=0;
	
	
	MyPrintf("bate:%04X,maskcode:%08X\r\n",CANTPara.BandRate,CANTPara.MaskCode);
	hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan2.Init.AutoRetransmission = DISABLE;
  hfdcan2.Init.TransmitPause = DISABLE;
  hfdcan2.Init.ProtocolException = DISABLE;
  hfdcan2.Init.NominalPrescaler = 17;
  hfdcan2.Init.NominalSyncJumpWidth = 1;
  hfdcan2.Init.NominalTimeSeg1 = 10;
  hfdcan2.Init.NominalTimeSeg2 = 9;
  hfdcan2.Init.DataPrescaler = 17;
  hfdcan2.Init.DataSyncJumpWidth = 1;
  hfdcan2.Init.DataTimeSeg1 = 10;
  hfdcan2.Init.DataTimeSeg2 = 9;
  hfdcan2.Init.StdFiltersNbr = 1;
  hfdcan2.Init.ExtFiltersNbr = 0;
  hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
  {
		CAN2SetFlag=1;
  }
	
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x666;
  sFilterConfig.FilterID2 = 0x000;
  if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig) != HAL_OK)
  {
		CAN2SetFlag=2;
  }
	if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
  {
		CAN2SetFlag=3;
  }
	if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
		CAN2SetFlag=4;
  }
	
	
	if (HAL_FDCAN_EnableTxDelayCompensation(&hfdcan2) != HAL_OK)
  {
    CAN2SetFlag=5;
  }
  
  if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
  {
    CAN2SetFlag=6;
  }
	
	/*TxHeader.Identifier = 0x321;
  TxHeader.IdType = FDCAN_STANDARD_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker = 0;*/
	/*if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, TxData) != HAL_OK)
	{
	    MyPrintf("inti send error\r\n");
	}*/
  return CAN2SetFlag;
}



uint8_t MX_CAN3_Init(_CANPara CANTPara)
{
	
	uint8_t CAN3SetFlag=0;
	
	
  hfdcan3.Instance = FDCAN3;
  hfdcan3.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan3.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan3.Init.AutoRetransmission = DISABLE;
  hfdcan3.Init.TransmitPause = DISABLE;
  hfdcan3.Init.ProtocolException = DISABLE;
  hfdcan3.Init.NominalPrescaler = 17;
  hfdcan3.Init.NominalSyncJumpWidth = 1;
  hfdcan3.Init.NominalTimeSeg1 = 10;
  hfdcan3.Init.NominalTimeSeg2 = 9;
  hfdcan3.Init.DataPrescaler = 17;
  hfdcan3.Init.DataSyncJumpWidth = 1;
  hfdcan3.Init.DataTimeSeg1 = 10;
  hfdcan3.Init.DataTimeSeg2 = 9;
  hfdcan3.Init.StdFiltersNbr = 1;
  hfdcan3.Init.ExtFiltersNbr = 0;
  hfdcan3.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan3) != HAL_OK)
  {
    CAN3SetFlag=1;
  }
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x666;
  sFilterConfig.FilterID2 = 0x000;
	if (HAL_FDCAN_ConfigFilter(&hfdcan3, &sFilterConfig) != HAL_OK)
  {
		 CAN3SetFlag=2;
  }
	if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
  {
		 CAN3SetFlag=3;
  }
	if (HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
		  CAN3SetFlag=4;
  }
  if (HAL_FDCAN_EnableTxDelayCompensation(&hfdcan3) != HAL_OK)
  {
			CAN3SetFlag=5;
  }
  
  if (HAL_FDCAN_Start(&hfdcan3) != HAL_OK)
  {
			CAN3SetFlag=6;
  }
	
	return  CAN3SetFlag; 
}







void Print_Test(void)
{
	
	
	static uint32_t AppTime=0;
  static uint8_t AppStep=0;
	int result=0;

  switch(AppStep)
  {
		case 0:
					GetSysTick((uint32_t *)&AppTime);
					AppStep=1;
					break;
		case 1:
				 if(CheckSysTick((uint32_t *)&AppTime,5000))
				 {  
					 
					 //Read_Update_Para();
					 GetSysTick((uint32_t *)&AppTime);
					 //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
					 MyPrintf("lilei five seconds here-%u\r\n",AppTime);
					// result=CDC_Transmit_FS("T123456789012345678901234567890123456789012345678901234567890\r\n",strlen("T123456789012345678901234567890123456789012345678901234567890\r\n"));
					 AppStep=0;
				 }
	       break;
			case 2:
				 if(CheckSysTick((uint32_t *)&AppTime,3000))
				 {  
					 
					 //MyPrintf("app  can send.initFlag:%d\r\n",Can2Para.CANFirstFlag);
					 if(Can2Para.CANFirstFlag)
					 {
							//MyPrintf("hello ID-1:%08X\r\n",TxHeaderLiLei.Identifier);
							/*if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeaderLiLei, TxData) != HAL_OK)
							{
								MyPrintf("hello can send error\r\n");
							}
							else
							{
									MyPrintf("hello can can send success\r\n");
						 
							}*/
						 	//MyPrintf("hello ID-2:%08X\r\n",TxHeaderLiLei.Identifier);
					 
					 //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
					 //MyPrintf("lilei five seconds here-%u\r\n",AppTime);
						AppStep=0;
					 }
				 }
	       break;
		default:
				AppStep=0;
				break;
  }

	if(PrintFlag==1)
	{
			//MyPrintf("CAN1:RID:%04X-%02X %02X %02X %02X %02X %02X %02X %02X\r\n",RxMeg1.StdId,CanData[0],CanData[1],CanData[2],CanData[3],CanData[4],CanData[5],CanData[6],CanData[7]);		
			PrintFlag=0;
		 //USER_CAN1_SSend(TxHeader1,CanData);
		 CAN2_Data_Pack(RxHeader,RxData);	
	}
	else if(PrintFlag==2)
	{
			//MyPrintf("CAN2:RID:%04X-%02X %02X %02X %02X %02X %02X %02X %02X\r\n",RxMeg2.StdId,CanData[0],CanData[1],CanData[2],CanData[3],CanData[4],CanData[5],CanData[6],CanData[7]);		
		 //MyPrintf("lilei-come on\r\n");	
		PrintFlag=0;
		//USER_CAN2_SSend(TxHeader2,CanData);
		CAN3_Data_Pack(RxHeader,RxData);	
	}
}



uint8_t lilei_can2_send(FDCAN_TxHeaderTypeDef TTHead)
{
	
	 /*TxHeaderLiLei.Identifier = 0x321;
  TxHeaderLiLei.IdType = FDCAN_STANDARD_ID;
  TxHeaderLiLei.TxFrameType = FDCAN_DATA_FRAME;
  TxHeaderLiLei.DataLength = FDCAN_DLC_BYTES_8;
  TxHeaderLiLei.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeaderLiLei.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeaderLiLei.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeaderLiLei.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeaderLiLei.MessageMarker = 0;
	
	*/
	
					 

	 	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TTHead, TxData) != HAL_OK)
		{
				MyPrintf("inti send error\r\n");
		}
		else
		{
				MyPrintf("can send success\r\n"); 
		}
		return 0;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
    /* Retrieve Rx messages from RX FIFO0 */
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
    {
			 //Error_Handler();
			MyPrintf("recive CANFIFO error\r\n");
    }
		
		if(hfdcan->Instance==FDCAN2)
		{
				PrintFlag=1;
		}
		else if(hfdcan->Instance==FDCAN3)
		{
				PrintFlag=2;
		}
		
  }
}



void CAN2_Data_Pack(FDCAN_RxHeaderTypeDef TRX,uint8_t *buf)
{
		uint16_t index=0;
	  uint8_t DataIndex=0;
	  uint8_t Can2Rlen=0;
	
	
	  RCanDataBuf[index++]=1; 
		RCanDataBuf[index++]=(uint8_t)((TRX.Identifier>>24)&0xff);
		RCanDataBuf[index++]=(uint8_t)((TRX.Identifier>>16)&0xff);
		RCanDataBuf[index++]=(uint8_t)((TRX.Identifier>>8)&0xff);
		RCanDataBuf[index++]=(uint8_t)(TRX.Identifier&0xff);
	  
	  switch(TRX.DataLength)
		{
			case FDCAN_DLC_BYTES_0:
					Can2Rlen=0;
					break;
      case FDCAN_DLC_BYTES_1:
					Can2Rlen=1;
				  break;
      case FDCAN_DLC_BYTES_2:
					Can2Rlen=2;
					break;
      case FDCAN_DLC_BYTES_3:
					Can2Rlen=3;
					break;
			case FDCAN_DLC_BYTES_4:
				  Can2Rlen=4;
					break;
			case FDCAN_DLC_BYTES_5:
				  Can2Rlen=5;
					break;
			case FDCAN_DLC_BYTES_6:
				  Can2Rlen=6;
					break;
			case FDCAN_DLC_BYTES_7:
				  Can2Rlen=7;
					break;
			case FDCAN_DLC_BYTES_8:
				  Can2Rlen=8;
					break;
			case FDCAN_DLC_BYTES_12:
				  Can2Rlen=12;
					break;
			case FDCAN_DLC_BYTES_16:
				  Can2Rlen=16;
					break;
			case FDCAN_DLC_BYTES_20:
				  Can2Rlen=20;
					break;
			case FDCAN_DLC_BYTES_24:
				  Can2Rlen=24;
					break;
			case FDCAN_DLC_BYTES_32:
				  Can2Rlen=32;
					break;
			case FDCAN_DLC_BYTES_48:
				  Can2Rlen=48;
					break;
			case FDCAN_DLC_BYTES_64:
				  Can2Rlen=64;
					break;
			default:
				Can2Rlen=0;
				break;
		}
	if(Can2Rlen<=0)
	{
		return;
	}
	 RCanDataBuf[index++]=Can2Rlen;
	 for(DataIndex=0;DataIndex<Can2Rlen;DataIndex++)
	 {
				RCanDataBuf[index++]=RxData[DataIndex];
	 }
   Usb_Pro_Pack(0x0801,RCanDataBuf,index);
}

void CAN3_Data_Pack(FDCAN_RxHeaderTypeDef TRX,uint8_t *buf)
{
		uint16_t index=0;
	  uint8_t DataIndex=0;
	  uint8_t Can3Rlen=0;
	
	
	  RCanDataBuf[index++]=1;
		RCanDataBuf[index++]=(uint8_t)((TRX.Identifier>>24)&0xff);
		RCanDataBuf[index++]=(uint8_t)((TRX.Identifier>>16)&0xff);
		RCanDataBuf[index++]=(uint8_t)((TRX.Identifier>>8)&0xff);
		RCanDataBuf[index++]=(uint8_t)(TRX.Identifier&0xff);
	  
	  switch(TRX.DataLength)
		{
			case FDCAN_DLC_BYTES_0:
					Can3Rlen=0;
					break;
      case FDCAN_DLC_BYTES_1:
					Can3Rlen=1;
				  break;
      case FDCAN_DLC_BYTES_2:
					Can3Rlen=2;
					break;
      case FDCAN_DLC_BYTES_3:
					Can3Rlen=3;
					break;
			case FDCAN_DLC_BYTES_4:
				  Can3Rlen=4;
					break;
			case FDCAN_DLC_BYTES_5:
				  Can3Rlen=5;
					break;
			case FDCAN_DLC_BYTES_6:
				  Can3Rlen=6;
					break;
			case FDCAN_DLC_BYTES_7:
				  Can3Rlen=7;
					break;
			case FDCAN_DLC_BYTES_8:
				  Can3Rlen=8;
					break;
			case FDCAN_DLC_BYTES_12:
				  Can3Rlen=12;
					break;
			case FDCAN_DLC_BYTES_16:
				  Can3Rlen=16;
					break;
			case FDCAN_DLC_BYTES_20:
				  Can3Rlen=20;
					break;
			case FDCAN_DLC_BYTES_24:
				  Can3Rlen=24;
					break;
			case FDCAN_DLC_BYTES_32:
				  Can3Rlen=32;
					break;
			case FDCAN_DLC_BYTES_48:
				  Can3Rlen=48;
					break;
			case FDCAN_DLC_BYTES_64:
				  Can3Rlen=64;
					break;
			default:
				Can3Rlen=0;
				break;
		}
	 
		if(Can3Rlen==0)return;
	 RCanDataBuf[index++]=Can3Rlen;
	 for(DataIndex=0;DataIndex<Can3Rlen;DataIndex++)
	 {
				RCanDataBuf[index++]=RxData[DataIndex];
	 }
   Usb_Pro_Pack(0x0802,RCanDataBuf,index);
}



uint8_t USER_CAN2_Send(FDCAN_TxHeaderTypeDef TXHead,uint8_t *Buf )
{
	
	uint8_t result=0;
	

	/*TTHead2.StdId = 0x323;
  TTHead2.ExtId = 0x01;
  TTHead2.RTR = CAN_RTR_DATA;
  TTHead2.IDE = CAN_ID_STD;
  TTHead2.DLC = 8;*/
  //TTHead2.TransmitGlobalTime = ENABLE;	
	//HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY);//????
	//uint8_t TxData[] = {0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22};
	/*if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TTHead2, TxData) != HAL_OK)
  {
		
  }*/
  /*TxHeader.Identifier = 0x326;
  TxHeader.IdType = FDCAN_STANDARD_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.DataLength = FDCAN_DLC_BYTES_8;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  TxHeader.MessageMarker = 0;*/
	 if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TXHead, Buf) != HAL_OK)
	 {
				result=1;
	 }
	/*if (HAL_CAN_AddTxMessage(&hcan2, &TTHead2, Buf, &TxMailbox) != HAL_OK)
	{
		Error_Handler();
		result=1;
	}	*/	
	return result;
	//HAL_Delay(100);
	//TxData[0]++;
}



uint8_t USER_CAN3_Send(FDCAN_TxHeaderTypeDef TXHead,uint8_t *Buf)
{
	
	
 uint8_t result=0;
	/*TTHead1.StdId = 0x321;
  TTHead1.ExtId = 0x01;
  TTHead1.RTR = CAN_RTR_DATA;
  TTHead1.IDE = CAN_ID_STD;
  TTHead1.DLC = 8;*/
 // TTHead1.TransmitGlobalTime = ENABLE;	
	//HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY);//????
	
	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TXHead, Buf) != HAL_OK)
  {
			result=1;
  }
	/*if (HAL_CAN_AddTxMessage(&hcan1, &TTHead1, Buf, &TxMailbox) != HAL_OK)
	{
		//Error_Handler();
		result=1;
	}	*/	
  return result;
}

