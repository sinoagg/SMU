#include "para_init.h"
#include "prj_typedef.h"

uint8_t silenttime_past=0;							              //静默时间过去标志位，0表示未过
uint8_t ADCread_status=0;								              //ADC是否读取的标志位
uint32_t sample_count=0;								              //sample number counter
uint32_t sample_totalval=0;							              //所有采样值的和
Float_Union_Data Adj_OutputOffset;                    //输出电压校准系数：偏移量
Float_Union_Data Adj_OutputLinear;                    //输出电压校准系数：线性度
Float_Union_Data Adj_MeasIOffset;                     //校准ADC监控电流偏移量
Float_Union_Data Adj_MeasILinear;                     //校准ADC监控电流线性度
Float_Union_Data Adj_MeasVOffset;                     //校准ADC监控电压偏移量
Float_Union_Data Adj_MeasVLinear;                     //校准ADC监控电压线性度
Float_Union_Data Adj_RelayOffset[10];                 //校准采集各档位电流偏移量
Float_Union_Data Adj_PosRelayLinear[10];              //校准正电流采集线性度  
Float_Union_Data Adj_NegRelayLinear[10];              //校准负电流采集线性度

extern uint8_t UART2_RxBuf[17];


/***********************************************
*初始化测试参数
************************************************/
void InitTestPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara)
{ 
	pTestPara->test_status=OFF;		                       //测试状态关闭
	
	pResultPara->ADC_sample_count=0;                     //采样数计数
	pResultPara->ADC_sample_sum_I=0;                     //采集电流值累加总和
	pResultPara->ADC_sample_sum_V=0;                     //采集电压值累加总和   
	pResultPara->test_result_I_sum.number_float=0;       //采集电流值
	pResultPara->test_result_V_sum.number_float=0;
	
	#ifdef MANUAL_ADJUSTMENT
		Adj_OutputOffset.number_float=0;                    //校准输出端偏移量
		Adj_OutputLinear.number_float=1;                    //校准输出端线性度
	
		Adj_MeasIOffset.number_float=0;                     //校准ADC监控电流偏移量
		Adj_MeasILinear.number_float=1;                     //校准ADC监控电流线性度
	
		Adj_MeasVOffset.number_float=0;                     //校准ADC监控电压偏移量
		Adj_MeasVLinear.number_float=1;                     //校准ADC监控电压线性度
		
		Adj_RelayOffset[0].number_float=0;                  //校准采集各档位电流偏移量                                    
		Adj_RelayOffset[1].number_float=0;                                          
		Adj_RelayOffset[2].number_float=0;                                         
		Adj_RelayOffset[3].number_float=0;                                         
		Adj_RelayOffset[4].number_float=0;                                           
		Adj_RelayOffset[5].number_float=0;                                        
		Adj_RelayOffset[6].number_float=0;                                         
		Adj_RelayOffset[7].number_float=0;                                        
		Adj_RelayOffset[8].number_float=0;                                          
		
		Adj_PosRelayLinear[0].number_float=1;               //校准正电流采集线性度            
		Adj_PosRelayLinear[1].number_float=1;                             
		Adj_PosRelayLinear[2].number_float=1;                    
		Adj_PosRelayLinear[3].number_float=1;                               
		Adj_PosRelayLinear[4].number_float=1;                               
		Adj_PosRelayLinear[5].number_float=1;                                
		Adj_PosRelayLinear[6].number_float=1;                               
		Adj_PosRelayLinear[7].number_float=1;                             
		Adj_PosRelayLinear[8].number_float=1;                                 
		
		Adj_NegRelayLinear[0].number_float=1;               //校准负电流采集线性度              
		Adj_NegRelayLinear[1].number_float=1;                                 
		Adj_NegRelayLinear[2].number_float=1;                                 
		Adj_NegRelayLinear[3].number_float=1;                                  
		Adj_NegRelayLinear[4].number_float=1;                                  
		Adj_NegRelayLinear[5].number_float=1;                                   
		Adj_NegRelayLinear[6].number_float=1;                                
		Adj_NegRelayLinear[7].number_float=1;                                
		Adj_NegRelayLinear[8].number_float=1;                                  
	#endif
	
	#ifdef NO_ADJUSTMENT
		Adj_OutputOffset.number_float=0;
		Adj_OutputLinear.number_float=1;
    Adj_MeasIOffset.number_float=0;                     //校准ADC监控电流偏移量
		Adj_MeasILinear.number_float=1;                     //校准ADC监控电流线性度
		Adj_MeasVOffset.number_float=0;                     //校准ADC监控电压偏移量
		Adj_MeasVLinear.number_float=1;                     //校准ADC监控电压线性度
		ClearArray(Adj_RelayOffset,10);
		SetArray(Adj_PosRelayLinear,10);
		SetArray(Adj_NegRelayLinear,10);
	#endif
	
}

/***********************************************
*获取传输的测试参数
************************************************/
uint8_t GetTestPara(TestPara_TypeDef* pTestPara, enum MsgType msgtype, enum MsgPort msgport)
{
	if(msgtype==MSG_TYPE_SETTING)
	{
		if(pTestPara->test_status==OFF)																	//如果设备没有测试进行中
		{
			if(msgport==MSG_PORT_RS485)																		//如果通信总线为RS485
			{                        
				pTestPara->test_mode=(enum TestMode)UART2_RxBuf[0];         //获取测量模式FIMV  FVMI
				pTestPara->test_timestep=UART2_RxBuf[9];                    //获取时间间隔
				pTestPara->test_timestep<<=8;
				pTestPara->test_timestep|=UART2_RxBuf[10];
				pTestPara->test_samplerate=UART2_RxBuf[11];                 //采样率默认值2000
				pTestPara->test_samplerate<<=8;
				pTestPara->test_samplerate|=UART2_RxBuf[12];
				pTestPara->test_averagenumber=UART2_RxBuf[13];              //采样数默认值400
				pTestPara->test_averagenumber<<=8;
				pTestPara->test_averagenumber|=UART2_RxBuf[14];  
				pTestPara->test_silenttime=UART2_RxBuf[15];                 //静默时间默认1000
				pTestPara->test_silenttime<<=8;
				pTestPara->test_silenttime|=UART2_RxBuf[16];
				if(pTestPara->test_mode==MODE_FVMI_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
				{
					Relay.relay_test_mode=RELAY_TEST_MODE_FVMI;               //设定继电器工作模式
					pTestPara->test_Vstart=UART2_RxBuf[2];
					pTestPara->test_Vstart<<=8;
					pTestPara->test_Vstart|=UART2_RxBuf[3];
					pTestPara->test_Vcurrent=pTestPara->test_Vstart;
					Relay.relay_change_mode=UART2_RxBuf[8];                    //根据测量模式选定测量档位设置 
				}
				else if(pTestPara->test_mode==MODE_FIMV_SWEEP||pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
				{
					Relay.relay_test_mode=RELAY_TEST_MODE_FIMV;               //设定继电器工作模式
					pTestPara->test_Istart=UART2_RxBuf[2];
					pTestPara->test_Istart<<=8;
					pTestPara->test_Istart|=UART2_RxBuf[3];
					pTestPara->test_Icurrent=pTestPara->test_Istart;
					Relay.relay_range=UART2_RxBuf[8];                          //根据电流大小获得
				}
				pTestPara->test_step=UART2_RxBuf[6];                         //测试步进电压或电流
				pTestPara->test_step<<=8;
				pTestPara->test_step|=UART2_RxBuf[7];
				
				pTestPara->test_startdelay=pTestPara->test_timestep - 1000*pTestPara->test_averagenumber/pTestPara->test_samplerate-10;			//给计算传输留出10ms空闲
				if(pTestPara->test_startdelay<0) pTestPara->test_startdelay=0;
				return 0;
			}
			else if(msgport==MSG_PORT_USB)
			{
				return 0;//未完成
			}
		}
		return 1;
	}
	else if(msgtype==MSG_TYPE_COMMAND)
	{
		pTestPara->test_cmd = (enum TestCMD)UART2_RxBuf[0];
		return 0;
	}
	else return 1;
}

void ClearArray(Float_Union_Data* pArray, uint8_t size)
{
	uint8_t i;
	for(i=0;i<size;i++)
	{
		(*(pArray+i)).number_float=0;
	}
}

void SetArray(Float_Union_Data* pArray, uint8_t size)
{
	uint8_t i;
	for(i=0;i<size;i++)
	{
		(*(pArray+i)).number_float=1;
	}
}



