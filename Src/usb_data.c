
/********************************************************************************
* Include Headers
********************************************************************************/

#include "usb_data.h"

#include "usbd_cdc_if.h"
//#include "driver_ucan.h"
#include "tool.h"
#include "inter_flash.h"
#include "usb_device.h"



//#include "main.h"



_UsbData UsbData;
_ProDeal ProDeal;


uint8_t TTDataBuf[512];
void Usb_Data_Init()
{
	UsbData.head=0;
	UsbData.tail=0;
	memset(UsbData.buf,0x000,sizeof(UsbData.buf));
	ProDeal.cmd=0;
	ProDeal.index=0;
	ProDeal.len=0;
  memset(ProDeal.buf,0x000,sizeof(ProDeal.buf));
	

	

}

void Usb_Buf_In(uint8_t * buf,uint32_t length)
{
	uint32_t i=0;
	for(i=0;i<length;i++)
	{
		UsbData.buf[UsbData.head++]=buf[i];
		if(UsbData.head>=USB_BUF_SIZE)
		{
				UsbData.head=0;
		}
	}
}

void USB_Data_Out()
{
	
	uint8_t TTData=0;
	static uint8_t ProStep=0;
	static uint8_t CheckSum=0;
   
	if(UsbData.head==UsbData.tail)
	{
		return;
	}
	
	 TTData = UsbData.buf[UsbData.tail++];

	 if(UsbData.tail >= USB_BUF_SIZE)
	 {
			UsbData.tail = 0;
	 }
	 //CDC_Transmit_FS((uint8_t *)&TTData,1);
	  //MyPrintf("%02X",TTData);
	 //return;
	 switch(ProStep)
	 {
		 case 0:
					if(TTData == 0x23)
					{
						ProStep++;
					}
					break;
		 	case 1:
					ProDeal.cmd=TTData;
			    ProStep++;
					break;
			case 2:
				  ProStep++;
				  ProDeal.cmd=(ProDeal.cmd*0x100+TTData);
					break;
			case 3:
				  ProDeal.len=TTData;
			    ProStep++;
					break;
			case 4:
					ProDeal.len=(ProDeal.len*0x100+TTData);
			    ProDeal.index=0;
					ProStep++;
					CheckSum=0;
					break;
			case 5:
				  CheckSum+=TTData;
				  ProDeal.buf[ProDeal.index++]=TTData;
			    if(ProDeal.index>=ProDeal.len)
					{
							ProStep++;
					}
				  break;
			case 6:
				   if(TTData==CheckSum)
					 {
							ProStep++;
					 }
					 else
					 {
							MyPrintf("check sum error\r\n");
						 	ProStep=0;
					 }
					 break;
			case 7:
				  if(TTData == 0x24)
					{
						Pro_Parse(ProDeal);
					}
					ProStep=0;
				  break;
			default:
				  ProStep=0;
				  break;		
	 
	 }


}




void Pro_Parse(_ProDeal TProData)
{
	
	  uint16_t index=0;
	  uint8_t TTbuf[512];
	  uint16_t TIndex=0;
	  uint16_t ResponeId=0;
	
		switch(TProData.cmd)
		{
				case 0x0100:
					   ResponeId=TProData.buf[index++]*0x100;
						 ResponeId+=TProData.buf[index++];
				     switch(ResponeId)
						 {
							 case 0x0800:
										break;
							 case 0x0801:
								    if(TProData.buf[index]==0)
										{
										}
										else
										{
										}
										break;
							 case 0x0802:
								    if(TProData.buf[index]==0)
										{
										}
										else
										{
										}										
										break;
							 case 0x0803:
										UpdateCtrlPara.StartFlag=1;
							      break;
							  case 0x0804:
									 if(TProData.buf[index]==0)
									 {
												UpdateCtrlPara.CRCFlag=1;
									 }
									 else
									 {
												UpdateCtrlPara.CRCFlag=2;
									 }
										break;
							 default:
										break;								 
						 }
					   MyPrintf("Recive Answer\r\n");
						 break;
		    case 0x0101:
					   break;
				case 0x0102: 
					   break;
				case 0x0105:
					    break;
				 case 0x0106:
					    UpdateCtrlPara.TotalBags=TProData.buf[index++]*0x100;
						  UpdateCtrlPara.TotalBags+=TProData.buf[index++];
				      UpdateCtrlPara.FileSize=TProData.buf[index++]*0x1000000;
						  UpdateCtrlPara.FileSize+=TProData.buf[index++]*0x10000;
						  UpdateCtrlPara.FileSize+=TProData.buf[index++]*0x100;
						  UpdateCtrlPara.FileSize+=TProData.buf[index++];
				      //UpdateCtrlPara.StartFlag=1;
				      MyPrintf("FileSize:%u,TotalBags:%u\r\n",UpdateCtrlPara.FileSize,UpdateCtrlPara.TotalBags);
				      TIndex=0;
				      TTbuf[TIndex++]=0;
				      Usb_Pro_PackT(0x0800 ,TProData.cmd,TTbuf,TIndex);
							break;
				  case 0x0107:
					    UpdateCtrlPara.ReciveBag=TProData.buf[index++]*0x100;
						  UpdateCtrlPara.ReciveBag+=TProData.buf[index++];
					    UpdateCtrlPara.ReciveBagSize=TProData.buf[index++]*0x100;
						  UpdateCtrlPara.ReciveBagSize+=TProData.buf[index++];
					    memcpy(UpdateCtrlPara.Buf,(char *)&TProData.buf[index], UpdateCtrlPara.ReciveBagSize);		  
              MyPrintf("CurBag:%u,CurBagSize:%u\r\n", UpdateCtrlPara.CurBag, UpdateCtrlPara.ReciveBagSize);					
							break;
					case 0x0108:					
							TTbuf[0]=0;
							Usb_Pro_PackT(0x0800 ,TProData.cmd,TTbuf,1);
							break;
				default:
						 break;
		}

}


void Usb_Pro_PackT(uint16_t cmd ,uint16_t ResponeCmd,uint8_t *buf,uint16_t length)
{

    //Usb_Send_Data(buf,length);
    //return;	
		uint16_t ProIndex=0;
	  uint16_t TPackLength=0;
	  uint8_t CheckSum=0;
	  uint16_t PackIndex=0;
	
	  TPackLength=length;
	  TPackLength+=2;
  
	  TTDataBuf[ProIndex++]=0x23;
	  TTDataBuf[ProIndex++]=(uint8_t)((cmd>>8)&0xff);
	  TTDataBuf[ProIndex++]=(uint8_t)(cmd&0xff);
		TTDataBuf[ProIndex++]=(uint8_t)((TPackLength>>8)&0xff);
		TTDataBuf[ProIndex++]=(uint8_t)(TPackLength&0xff);
	
	
	
	
	  TTDataBuf[ProIndex]=(uint8_t)((ResponeCmd>>8)&0xff);
		CheckSum+= TTDataBuf[ProIndex];
		ProIndex++;
	  TTDataBuf[ProIndex]=(uint8_t)(ResponeCmd&0xff);
		CheckSum+= TTDataBuf[ProIndex];
		ProIndex++;
	  
	  for(PackIndex=0;PackIndex<length;PackIndex++)
	  {
			TTDataBuf[ProIndex]=buf[PackIndex];
			CheckSum+=	TTDataBuf[ProIndex];
			ProIndex++;
		
		}
		TTDataBuf[ProIndex++]=CheckSum;
	  TTDataBuf[ProIndex++]=0x24;
		Usb_Send_Data(TTDataBuf,ProIndex);
	 
}

void Usb_Pro_Pack(uint16_t cmd ,uint8_t *buf,uint16_t length)
{

    //Usb_Send_Data(buf,length);
    //return;	
		uint16_t ProIndex=0;
    uint16_t PackIndex=0;
	  uint8_t CheckSum=0;
	
	  TTDataBuf[ProIndex++]=0x23;
	  TTDataBuf[ProIndex++]=(uint8_t)((cmd>>8)&0xff);
	  TTDataBuf[ProIndex++]=(uint8_t)(cmd&0xff);
		TTDataBuf[ProIndex++]=(uint8_t)((length>>8)&0xff);
		TTDataBuf[ProIndex++]=(uint8_t)(length&0xff);
	  
	  for(PackIndex=0;PackIndex<length;PackIndex++)
	  {
			TTDataBuf[ProIndex]=buf[PackIndex];
			CheckSum+=TTDataBuf[ProIndex];
			ProIndex++;
		}
		TTDataBuf[ProIndex++]=CheckSum;
	  TTDataBuf[ProIndex++]=0x24;
		Usb_Send_Data(TTDataBuf,ProIndex);
	 
}

uint8_t Usb_Send_Data(uint8_t *buf,uint16_t length)
{
	uint16_t SendIndex=0;
	uint16_t LessData;
	uint32_t GetDealyTIme=0;
	
	uint8_t SendResult=0;
	
	
	  GetSysTick((uint32_t *)&GetDealyTIme);
		if(length>USB_SEND_SIZE)
		{
				for(SendIndex=0;SendIndex<length;SendIndex+=USB_SEND_SIZE)
			  {
				
						 /*USBD_LL_Transmit(&hUsbDeviceFS,
                             CDC_IN_EP,
                             (uint8_t *)&buf[SendIndex],
                             64);*/
					while(CDC_Transmit_FS(buf,USB_SEND_SIZE))
					{
						 if(CheckSysTick((uint32_t *)&GetDealyTIme,3))
						 {
							 	//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15, GPIO_PIN_SET);
							 USB_POWER_OFF;
							  SendResult=1;
								break;
						 }
					}
				}
				if(length%USB_SEND_SIZE!=0)
				{
					LessData=length%USB_SEND_SIZE;
					while(CDC_Transmit_FS((uint8_t *)&buf[length-LessData],LessData))
					{
						 if(CheckSysTick((uint32_t *)&GetDealyTIme,3))
						 {
							  USB_POWER_OFF;
							  //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15, GPIO_PIN_SET);
							  SendResult=1;
								break;
						 }
					}
				}
		}
		else
		{
				while(CDC_Transmit_FS(buf,length))
				{
					 if(CheckSysTick((uint32_t *)&GetDealyTIme,3))
					 {
						  USB_POWER_OFF;
						  //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15, GPIO_PIN_SET);
						  SendResult=1;
							break;
					 }
				}
		}
		if(SendResult==0)
		{
			USB_POWER_ON;
			//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15, GPIO_PIN_RESET);
		}
    return SendResult;
}


