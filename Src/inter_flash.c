/**
  ******************************************************************************
  * @file           : cpuFlash.c
  * @brief          : cpu内部flash
  ******************************************************************************
  * @attention
  *
  * Copyright: https://blog.csdn.net/qq_30155503
  * All rights reserved.
  *
  ******************************************************************************
  */


/*****************************************************************
* 包含头文件
******************************************************************/

#include "inter_flash.h"
#include "tool.h"
#include "usb_data.h"
#include "main.h"
#include <math.h>




uint8_t FlahBootFlag=0;
_UpdateCtrlPara UpdateCtrlPara;
uint8_t FlaBBuf[256];
uint32_t CurretWAdress=0;
uint32_t TCRC=0;
 uint8_t UpdataReturnValue=0xff;


/****************************************************************
* Func 	:
* Desc	:	读取CPU内部flash
* Input	:
* Output:
* Return:
*****************************************************************/
int32_t cpuflash_read(uint32_t unStartAddr, uint8_t *pData, uint16_t usSize)
{

	if(pData == NULL)
		return -1;

	memcpy(pData, (int8_t *)unStartAddr, usSize);

	return 0;
}

/****************************************************************
* Func 	:
* Desc	:	写入CPU内部flash (要先erase才能写)
* Input	:
* Output:
* Return:
*****************************************************************/
int32_t cpuflash_write(uint32_t unStartAddr, uint8_t *pData, uint16_t usSize)
{
	int32_t  cwIndex1= 0;
  int32_t 	cwIndex2 = 0;
	uint16_t nowSize=0;
  int32_t 	nRet = 0;

  uint64_t 	usTempALL = 0;
	
	uint32_t  lowD=0;
	uint32_t highD=0;

	
	 nowSize=usSize;
   if(nowSize%8 != 0)
   {
        nowSize += (8-usSize%8);
    }

	  HAL_FLASH_Unlock();		// unlock
    for(cwIndex1=0; cwIndex1<nowSize/8; cwIndex1++)
    {
			usTempALL=0;
			lowD=0;
			highD=0;
			for(cwIndex2=0;cwIndex2<8;cwIndex2++)
			{
				if(cwIndex1*8+cwIndex2>=usSize)
				{
					break;
				}
				
				switch(cwIndex2)
				{
					case 0:
						lowD+=pData[cwIndex1*8+cwIndex2];
						break;
				  case 1:
						lowD+=pData[cwIndex1*8+cwIndex2]*0x100;
						break;
					case 2:
						lowD+=pData[cwIndex1*8+cwIndex2]*0x10000;
						break;
					case 3:
						lowD+=pData[cwIndex1*8+cwIndex2]*0x1000000;
						break;
					case 4:
						highD+=pData[cwIndex1*8+cwIndex2];
						break;
					case 5:
						highD+=pData[cwIndex1*8+cwIndex2]*0x100;
						break;
					case 6:
						highD+=pData[cwIndex1*8+cwIndex2]*0x10000;
						break;
					case 7:
						highD+=pData[cwIndex1*8+cwIndex2]*0x1000000;
						break;
				}
			  /*if(j==0)
				{
					usTempALL=pData[i*8+j];
				}
				else
				{
						usTempALL+=(uint64_t)((((uint64_t)pData[i*8+j])<<(j*8))&0xffffffffffffffff);
				}*/
			}
		
			usTempALL=(uint64_t)(lowD+highD*0x100000000);
			//MyPrintf("write:%08x-%08X",(uint32_t)((usTempALL>>32)&0xffffffff),(uint32_t)(usTempALL&0xffffffff));
			nRet = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, unStartAddr, usTempALL);					//add by lilei-20021028
			if(nRet != HAL_OK)
			{
						HAL_FLASH_Lock();		// lock
						MyPrintf("ERROR: %08X: program failed\n",unStartAddr);
						return -1;
			}
			unStartAddr += 8;
			//pData += 8;
	 }
   HAL_FLASH_Lock();		// lock

	return 0;
}

/****************************************************************
* Func 	:
* Desc	:	擦除CPU内部flash(整页)
* Input	:
* Output:
* Return:
*****************************************************************/
int32_t cpuflash_erase(uint32_t unStartAddr, uint32_t unEndAddr)
{
	FLASH_EraseInitTypeDef	stEraseInit;
	uint32_t		ucPageErr = 0;
	int32_t		nRet = 0;

  uint32_t FirstPage = 0; 
	uint32_t BankNumber = 0;
	
	
	HAL_FLASH_Unlock();		// unlock
   FirstPage = GetPage(unStartAddr);
	BankNumber = GetBank(unStartAddr);
	BankNumber=1;
	MyPrintf("firstpage:%d\r\n",FirstPage);
	MyPrintf("BankNumber:%d\r\n",BankNumber);
	MyPrintf("Nbpages:%d\r\n",(unEndAddr-unStartAddr)/FLASH_PAGE_SIZE);
	//for(unTempAddr=ADDR_FLASH_PAGE_64; unTempAddr<=unEndAddr; unTempAddr+=FLASH_PAGE_SIZE)
	//{
		stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	  stEraseInit.Banks       = BankNumber;
     stEraseInit.Page        = FirstPage;
		stEraseInit.NbPages=(unEndAddr-unStartAddr)/FLASH_PAGE_SIZE;
		nRet = HAL_FLASHEx_Erase(&stEraseInit, &ucPageErr);
		if(nRet != HAL_OK)
		{
			HAL_FLASH_Lock();
			MyPrintf("Erase error-%08X\r\n",FLASH_PAGE_SIZE*FirstPage);
			return -1;
		}
		else
		{
			MyPrintf("Erase pagesaddress-%08X\r\n",FirstPage*FLASH_PAGE_SIZE);
		}
		//GPIO_feedDog();
	//}

    HAL_FLASH_Lock();		// lock

	return 0;
}

/****************************************************************
* Func 	:
* Desc	:	使能读保护函数
* Input	:
* Output:
* Return:
*****************************************************************/
void cpuflash_enableReadProtect(void)
{
  FLASH_OBProgramInitTypeDef OBInit;
  
  __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
  
  HAL_FLASHEx_OBGetConfig(&OBInit);
  if(OBInit.RDPLevel == OB_RDP_LEVEL_0)
  {
  	MyPrintf("%s: ------------ set ----------\n", __FUNCTION__);
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel = OB_RDP_LEVEL_1;
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    HAL_FLASHEx_OBProgram(&OBInit);
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
	//HAL_FLASH_OB_Launch();
  }
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();

}

/****************************************************************
* Func 	:
* Desc	:	失能读保护函数
* Input	:
* Output:
* Return:
*****************************************************************/
void cpuflash_disableReadProtect(void)
{
  FLASH_OBProgramInitTypeDef OBInit;
  
  __HAL_FLASH_PREFETCH_BUFFER_DISABLE();
  
  HAL_FLASHEx_OBGetConfig(&OBInit);
  if(OBInit.RDPLevel == OB_RDP_LEVEL_1)
  {
	  MyPrintf("%s: ------------ set ----------\n", __FUNCTION__);
    OBInit.OptionType = OPTIONBYTE_RDP;
    OBInit.RDPLevel = OB_RDP_LEVEL_0;
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    HAL_FLASHEx_OBProgram(&OBInit);
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
	//HAL_FLASH_OB_Launch();
  }
  __HAL_FLASH_PREFETCH_BUFFER_ENABLE();

}

/****************************************************************
* Func 	:
* Desc	:	WriteFlag=1   update flag       writeFlag=2  update SN length=12
* Input	:
* Output:
* Return:
*****************************************************************/
int cpuflash_Init(void)
{

	/* ?a???á±￡?¤-?úbootloader?a???′?é */
	//cpuflash_enableReadProtect();
	memset(&UpdateCtrlPara,0x00,sizeof(UpdateCtrlPara));
	UpdataReturnValue=0xff;
  //UpdateCtrlPara
	return 0;
}










void Flash_Boot_Init()
{
	 memset((int8_t *)&UpdateCtrlPara,0x00,sizeof(_UpdateCtrlPara));

}

uint8_t Read_Update_Para()
{
     int ReturnV=0xff;
	   _UpdatePara *Updatepara=NULL;
	
	   Updatepara=(_UpdatePara *)UPDATE_PARA_ADDRESS;
	MyPrintf("upateFlag:%08X",Updatepara->UpdateFlag);
	   if(Updatepara->UpdateFlag==PAGE_USEFLAG)
		 {
				ReturnV=1;
		 
		 }
	   return ReturnV;
}

uint8_t Update_Task_Deal()
{


     static uint8_t step=0;
	   static uint32_t UpdataTime;
	   //static uint32_t TotalTime=0;
	   int index=0;
	   int CrcIndex=0;
	   int TTTindex=0;
	   int lessSize=0;
    
	   
	   //UpdataReturnValue=Read_Update_Para();
         
			 switch(step)
			 {
				 case UPDATE_BEGINFLAG:
					    UpdataReturnValue=Read_Update_Para();
					    if(UpdataReturnValue==1)
							{
								step=UPDATE_REQ;
						
								Flash_Boot_Init();
							}
					    break;
					case UPDATE_REQ:
							 FlaBBuf[0]=0;
					     	MyPrintf("BootVer:%s\r\n",BOOTVER);
					     MyPrintf("Device Send Request\r\n");
							 Usb_Pro_Pack(0x0803,FlaBBuf,1);
					     step=REPLY_UPDATE;
							 GetSysTick((uint32_t *)&UpdataTime);
							 break;						 
					case REPLY_UPDATE:
							if(CheckSysTick((uint32_t *)&UpdataTime, 1000))
							{
								 step=UPDATE_REQ;
							}
							if(UpdateCtrlPara.StartFlag==1)
							{
									step=REPLY_UPDATEPARA;
								  UpdataReturnValue=2;
								  MyPrintf("Device begin Get_UPDATEPARA\r\n");
							}
							break;
					case REPLY_UPDATEPARA:
							 if(UpdateCtrlPara.TotalBags>0&&UpdateCtrlPara.FileSize>0)
							 {
									step=BEGIN_UPDATE;
								  cpuflash_erase(APP_START_ADDRESS,APP_START_ADDRESS+APP_KB*1024);						//36KB+92KB
								  CurretWAdress=APP_START_ADDRESS;
								  UpdataReturnValue=3;
								  TCRC=0;
								 Read_Update_Para();
							 }
							 break;						 
					case BEGIN_UPDATE:
							 if(UpdateCtrlPara.CurBag==UpdateCtrlPara.ReciveBag&&UpdateCtrlPara.ReciveBagSize>0)
							 {	
									UpdataReturnValue=4;
								  //MyPrintf("recive bagsize:%08X\r\n",UpdateCtrlPara.ReciveBagSize);
								 for(TTTindex=0;TTTindex<UpdateCtrlPara.ReciveBagSize;TTTindex++)
								 {	    
									  TCRC+=UpdateCtrlPara.Buf[TTTindex];
									   //MyPrintf("%02X-",UpdateCtrlPara.Buf[TTTindex]);
								 }
								 //MyPrintf("\r\nreciveA:%08X,d0:%02X,d1:%02X,d2:%02X,d3:%02X\r\n",CurretWAdress,UpdateCtrlPara.Buf[0],UpdateCtrlPara.Buf[1],UpdateCtrlPara.Buf[UpdateCtrlPara.ReciveBagSize-2],UpdateCtrlPara.Buf[UpdateCtrlPara.ReciveBagSize-1]);
								 cpuflash_write(CurretWAdress,UpdateCtrlPara.Buf,UpdateCtrlPara.ReciveBagSize);
								 index=0;
								 FlaBBuf[index++]=0;
								 FlaBBuf[index++]=(uint8_t)((UpdateCtrlPara.ReciveBag>>8)&0xff);
								 FlaBBuf[index++]=(uint8_t)(UpdateCtrlPara.ReciveBag&0xff);
								 Usb_Pro_PackT(0x0800,0x0107,FlaBBuf,index);	
								 if(UpdateCtrlPara.ReciveBag>=UpdateCtrlPara.TotalBags-1)
								 {
										MyPrintf("Recive FileBag Success-CRC=%08X\r\n",TCRC);
										step=UPDATE_SUCCESS;
										UpdateCtrlPara.RecviCRCSUM=0;
									  for(index=0;index<UpdateCtrlPara.FileSize/DEF_EVERY_BAGSIZE;index++)
										{
												memset(UpdateCtrlPara.Buf,0x00,sizeof(UpdateCtrlPara.Buf));
												cpuflash_read(APP_START_ADDRESS+index*DEF_EVERY_BAGSIZE,UpdateCtrlPara.Buf,DEF_EVERY_BAGSIZE);
											// MyPrintf("read begin\r\n");
											//MyPrintf("address:%08X,data0:%02X,data1:%02X,data2:%02X,data3:%02X\r\n",APP_START_ADDRESS+index*DEF_EVERY_BAGSIZE,
											//UpdateCtrlPara.Buf[0],UpdateCtrlPara.Buf[1],UpdateCtrlPara.Buf[DEF_EVERY_BAGSIZE-2],UpdateCtrlPara.Buf[DEF_EVERY_BAGSIZE-1]);
												for(CrcIndex=0;CrcIndex<DEF_EVERY_BAGSIZE;CrcIndex++)
												{
														UpdateCtrlPara.RecviCRCSUM+=UpdateCtrlPara.Buf[CrcIndex];			
														//MyPrintf("-%02X-",UpdateCtrlPara.Buf[CrcIndex]);
												}
												//MyPrintf("read end\r\n");
									  }
										  MyPrintf("UpdateCtrlPara.RecviCRCSUM -CRC=%08X\r\n",UpdateCtrlPara.RecviCRCSUM);
									  if(UpdateCtrlPara.FileSize%DEF_EVERY_BAGSIZE!=0)
									  {
											  lessSize=UpdateCtrlPara.FileSize%DEF_EVERY_BAGSIZE;
											MyPrintf("lesssize:%d\r\n",lessSize);
												memset(UpdateCtrlPara.Buf,0x00,sizeof(UpdateCtrlPara.Buf));
												cpuflash_read(APP_START_ADDRESS+index*DEF_EVERY_BAGSIZE,UpdateCtrlPara.Buf,lessSize);
											/*MyPrintf("last-address:%08X,data0:%02X,data1:%02X,data2:%02X,data3:%02X\r\n",APP_START_ADDRESS+index*DEF_EVERY_BAGSIZE,
											UpdateCtrlPara.Buf[0],UpdateCtrlPara.Buf[1],UpdateCtrlPara.Buf[lessSize-2],UpdateCtrlPara.Buf[lessSize-1]);*/
												for(CrcIndex=0;CrcIndex<UpdateCtrlPara.FileSize%DEF_EVERY_BAGSIZE;CrcIndex++)
												{
													UpdateCtrlPara.RecviCRCSUM+=UpdateCtrlPara.Buf[CrcIndex];
													//MyPrintf("-%02X-",UpdateCtrlPara.Buf[CrcIndex]);
												}
											  MyPrintf("UpdateCtrlPara.RecviCRCSUM -CRC=%08X\r\n",UpdateCtrlPara.RecviCRCSUM);
												step=UPDATE_CRC;
												break;		 
										}		
								 }
								 CurretWAdress+=UpdateCtrlPara.ReciveBagSize;
								 UpdateCtrlPara.CurBag+=1;
								 UpdateCtrlPara.ReciveBagSize=0;									
							 }
							 break;
					case UPDATE_CRC:
						    UpdataReturnValue=5;
							  index=0;
								FlaBBuf[index++]=(uint8_t)((UpdateCtrlPara.RecviCRCSUM>>24)&0xff);
								FlaBBuf[index++]=(uint8_t)((UpdateCtrlPara.RecviCRCSUM>>16)&0xff);
							  FlaBBuf[index++]=(uint8_t)((UpdateCtrlPara.RecviCRCSUM>>8)&0xff);
							  FlaBBuf[index++]=(uint8_t)(UpdateCtrlPara.RecviCRCSUM&0xff);
							  Usb_Pro_Pack(0x0804,FlaBBuf,index);
							 	GetSysTick((uint32_t *)&UpdataTime);
					      step=UPDATE_CRC_WAIT;
						   break;
					case UPDATE_CRC_WAIT:
						  if(CheckSysTick((uint32_t *)&UpdataTime, 1000))
							{
								 step=UPDATE_CRC;
							}
							if(UpdateCtrlPara.CRCFlag==1)
							{
									step=UPDATE_SUCCESS;
								  MyPrintf("update success\r\n");
							}
							else if(UpdateCtrlPara.CRCFlag==2)
							{
									step=UPDATE_FAIL;
								  MyPrintf("update fail-CRC CheckSum error\r\n");
							}
							 break;
					case UPDATE_SUCCESS:
					     MyPrintf("update success\r\n");
						   UpdataReturnValue=0;
							 break;
					case UPDATE_FAIL:
						   step=UPDATE_BEGINFLAG;
							 UpdataReturnValue=0x7f;
							 break;
					default:
							 break;
					
				}

				return UpdataReturnValue;

}




uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;

  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
    /* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
    /* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }
  
	
	if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  return bank;
}


uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  return page;
}

