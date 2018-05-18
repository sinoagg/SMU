#include "para_init.h"
#include "prj_typedef.h"

uint8_t silenttime_past=0;							              //��Ĭʱ���ȥ��־λ��0��ʾδ��
uint8_t ADCread_status=0;								              //ADC�Ƿ��ȡ�ı�־λ
uint32_t sample_count=0;								              //sample number counter
uint32_t sample_totalval=0;							              //���в���ֵ�ĺ�
Float_Union_Data Adj_OutputOffset;                    //�����ѹУ׼ϵ����ƫ����
Float_Union_Data Adj_OutputLinear;                    //�����ѹУ׼ϵ�������Զ�
Float_Union_Data Adj_MeasIOffset;                     //У׼ADC��ص���ƫ����
Float_Union_Data Adj_MeasILinear;                     //У׼ADC��ص������Զ�
Float_Union_Data Adj_MeasVOffset;                     //У׼ADC��ص�ѹƫ����
Float_Union_Data Adj_MeasVLinear;                     //У׼ADC��ص�ѹ���Զ�
Float_Union_Data Adj_RelayOffset[10];                 //У׼�ɼ�����λ����ƫ����
Float_Union_Data Adj_PosRelayLinear[10];              //У׼�������ɼ����Զ�  
Float_Union_Data Adj_NegRelayLinear[10];              //У׼�������ɼ����Զ�

extern uint8_t UART2_RxBuf[17];


/***********************************************
*��ʼ�����Բ���
************************************************/
void InitTestPara(TestPara_TypeDef* pTestPara, TestResult_TypeDef* pResultPara)
{ 
	pTestPara->test_status=OFF;		                       //����״̬�ر�
	
	pResultPara->ADC_sample_count=0;                     //����������
	pResultPara->ADC_sample_sum_I=0;                     //�ɼ�����ֵ�ۼ��ܺ�
	pResultPara->ADC_sample_sum_V=0;                     //�ɼ���ѹֵ�ۼ��ܺ�   
	pResultPara->test_result_I_sum.number_float=0;       //�ɼ�����ֵ
	pResultPara->test_result_V_sum.number_float=0;
	
	#ifdef MANUAL_ADJUSTMENT
		Adj_OutputOffset.number_float=0;                    //У׼�����ƫ����
		Adj_OutputLinear.number_float=1;                    //У׼��������Զ�
	
		Adj_MeasIOffset.number_float=0;                     //У׼ADC��ص���ƫ����
		Adj_MeasILinear.number_float=1;                     //У׼ADC��ص������Զ�
	
		Adj_MeasVOffset.number_float=0;                     //У׼ADC��ص�ѹƫ����
		Adj_MeasVLinear.number_float=1;                     //У׼ADC��ص�ѹ���Զ�
		
		Adj_RelayOffset[0].number_float=0;                  //У׼�ɼ�����λ����ƫ����                                    
		Adj_RelayOffset[1].number_float=0;                                          
		Adj_RelayOffset[2].number_float=0;                                         
		Adj_RelayOffset[3].number_float=0;                                         
		Adj_RelayOffset[4].number_float=0;                                           
		Adj_RelayOffset[5].number_float=0;                                        
		Adj_RelayOffset[6].number_float=0;                                         
		Adj_RelayOffset[7].number_float=0;                                        
		Adj_RelayOffset[8].number_float=0;                                          
		
		Adj_PosRelayLinear[0].number_float=1;               //У׼�������ɼ����Զ�            
		Adj_PosRelayLinear[1].number_float=1;                             
		Adj_PosRelayLinear[2].number_float=1;                    
		Adj_PosRelayLinear[3].number_float=1;                               
		Adj_PosRelayLinear[4].number_float=1;                               
		Adj_PosRelayLinear[5].number_float=1;                                
		Adj_PosRelayLinear[6].number_float=1;                               
		Adj_PosRelayLinear[7].number_float=1;                             
		Adj_PosRelayLinear[8].number_float=1;                                 
		
		Adj_NegRelayLinear[0].number_float=1;               //У׼�������ɼ����Զ�              
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
    Adj_MeasIOffset.number_float=0;                     //У׼ADC��ص���ƫ����
		Adj_MeasILinear.number_float=1;                     //У׼ADC��ص������Զ�
		Adj_MeasVOffset.number_float=0;                     //У׼ADC��ص�ѹƫ����
		Adj_MeasVLinear.number_float=1;                     //У׼ADC��ص�ѹ���Զ�
		ClearArray(Adj_RelayOffset,10);
		SetArray(Adj_PosRelayLinear,10);
		SetArray(Adj_NegRelayLinear,10);
	#endif
	
}

/***********************************************
*��ȡ����Ĳ��Բ���
************************************************/
uint8_t GetTestPara(TestPara_TypeDef* pTestPara, enum MsgType msgtype, enum MsgPort msgport)
{
	if(msgtype==MSG_TYPE_SETTING)
	{
		if(pTestPara->test_status==OFF)																	//����豸û�в��Խ�����
		{
			if(msgport==MSG_PORT_RS485)																		//���ͨ������ΪRS485
			{                        
				pTestPara->test_mode=(enum TestMode)UART2_RxBuf[0];         //��ȡ����ģʽFIMV  FVMI
				pTestPara->test_timestep=UART2_RxBuf[9];                    //��ȡʱ����
				pTestPara->test_timestep<<=8;
				pTestPara->test_timestep|=UART2_RxBuf[10];
				pTestPara->test_samplerate=UART2_RxBuf[11];                 //������Ĭ��ֵ2000
				pTestPara->test_samplerate<<=8;
				pTestPara->test_samplerate|=UART2_RxBuf[12];
				pTestPara->test_averagenumber=UART2_RxBuf[13];              //������Ĭ��ֵ400
				pTestPara->test_averagenumber<<=8;
				pTestPara->test_averagenumber|=UART2_RxBuf[14];  
				pTestPara->test_silenttime=UART2_RxBuf[15];                 //��Ĭʱ��Ĭ��1000
				pTestPara->test_silenttime<<=8;
				pTestPara->test_silenttime|=UART2_RxBuf[16];
				if(pTestPara->test_mode==MODE_FVMI_SWEEP||pTestPara->test_mode==MODE_FVMI_NO_SWEEP)
				{
					Relay.relay_test_mode=RELAY_TEST_MODE_FVMI;               //�趨�̵�������ģʽ
					pTestPara->test_Vstart=UART2_RxBuf[2];
					pTestPara->test_Vstart<<=8;
					pTestPara->test_Vstart|=UART2_RxBuf[3];
					pTestPara->test_Vcurrent=pTestPara->test_Vstart;
					Relay.relay_change_mode=UART2_RxBuf[8];                    //���ݲ���ģʽѡ��������λ���� 
				}
				else if(pTestPara->test_mode==MODE_FIMV_SWEEP||pTestPara->test_mode==MODE_FIMV_NO_SWEEP)
				{
					Relay.relay_test_mode=RELAY_TEST_MODE_FIMV;               //�趨�̵�������ģʽ
					pTestPara->test_Istart=UART2_RxBuf[2];
					pTestPara->test_Istart<<=8;
					pTestPara->test_Istart|=UART2_RxBuf[3];
					pTestPara->test_Icurrent=pTestPara->test_Istart;
					Relay.relay_range=UART2_RxBuf[8];                          //���ݵ�����С���
				}
				pTestPara->test_step=UART2_RxBuf[6];                         //���Բ�����ѹ�����
				pTestPara->test_step<<=8;
				pTestPara->test_step|=UART2_RxBuf[7];
				
				pTestPara->test_startdelay=pTestPara->test_timestep - 1000*pTestPara->test_averagenumber/pTestPara->test_samplerate-10;			//�����㴫������10ms����
				if(pTestPara->test_startdelay<0) pTestPara->test_startdelay=0;
				return 0;
			}
			else if(msgport==MSG_PORT_USB)
			{
				return 0;//δ���
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



