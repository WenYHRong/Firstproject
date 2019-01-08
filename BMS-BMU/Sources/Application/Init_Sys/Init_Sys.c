/*=======================================================================
 *Subsystem:   ���
 *File:        Init_Sys.C
 *Author:      Wenming
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:
      Author:
      Modification:
========================================================================*/
#include  "includes.h"  

// ����ϵͳ�ĳ�ʼ������  
LIBDEF_MemPtr  MemPtr;
SysInitState_T  g_SysInitState;

static void Physic_Init(void);
/*=======================================================================
 *������:      Init_Sys(void)
 *����:        ϵͳ��ʼ��
 *����:        ��       
 *���أ�       ��
 
 *˵����       
========================================================================*/
void Init_Sys(void) 
{ 
  DisableInterrupts;      
  IVBR=0x7F; //Ӧ�ó����е��жϻ�������ַΪ7F
  
  //ϵͳ��ʼ��״̬����ֵ����
  MemPtr = memset(&g_SysInitState, 0, sizeof(SysInitState_T));
  
  //������ʼ��
  Physic_Init();
  
  
  //DS3231ʱ���ʼ��
  #if(RESET_CLOCK == 1)
  {
    DS3231SN_INIT(0b00011000, 1, 1, 1, 0, 0b01011001); //ʱ�ӳ�ʼ��(18/01/01)��while������ԭ����IIC��ʼ�����ڶ�ȡ����֮ǰ
  }//�˴��õ�BCD��,��4λΪʮ���Ƶ�ʮλ,��4λΪʮ���Ƶĸ�λ
  #endif
  //��ȡϵͳʱ��  
  Time_Init();
  
  //���ݴ�����������
  Init_TaskDataProcess();
  
  //����������������
  Init_TaskCurrLimit();
  
  //���ϵȼ��ж���������
  Init_TaskFltLevJudg(); 
  
  //������ֵ��ʼ��
  Init_UpMonitor();
  
  //��ȡEEE�е�ֵ
  Get_EEprom_Value();
  
  //SOC��ʼ������
  #if(RESET_SOC == 1)
  {
    g_SOCInfo.SOC_Init = 0;    
  }
  #endif
   
  
  // ��ʼ��������������,��flag��0 
  Task_Init();                  
  
  EnableInterrupts;   
}

/*=======================================================================
 *������:      Physic_Init(void)
 *����:        �ײ��ʼ��,���ײ��Ӧ��Ӧ�ý��г�ʼ��
 *����:        ��       
 *���أ�       ��
 *˵����       ��ʼ������㡢ȫ�ֱ�����ϵͳʱ���
========================================================================*/
static
void Physic_Init(void)
{ 
  //�ײ�Ӳ����ʼ��
  g_SysInitState.PLL    = Init_PLL();              //���໷��ʼ��
  g_SysInitState.EEPROM = Init_Flash();            //EEPROM��ʼ��
  g_SysInitState.IIC    = Init_IIC();              //IIC��ʼ��;�ڶ�ʱ��ģ���������֮ǰ��Ҫ��ʼ�� 
  g_SysInitState.ADC    = Init_ADC();              //ADģ���ʼ��
  g_SysInitState.CAN1   = CAN_ToChargeInit();      //���CAN,CAN1 
  g_SysInitState.CAN2   = CAN_UpMonitorInit();     //����CAN,CAN2
  g_SysInitState.Relay_Positvie = Init_Relay();    //�����̵�����ʼ��
  
  g_SysInitState.Insul  = Insulation_Init();       //��Ե���
  g_SysInitState.Screen = Init_Screen();           //��ʾ��SCI1��ʼ��  
  g_SysInitState.SPI    = LTC6804_Init();
  g_SysInitState.PIT0   = Init_PIT();             //PIT0,�ж�����Ϊ10ms  
 
  //���г�ʼ��
  if(g_SysInitState.PLL || g_SysInitState.IIC || g_SysInitState.EEPROM ||\
     g_SysInitState.ADC || g_SysInitState.CAN1 || g_SysInitState.CAN2 ||\
     g_SysInitState.Relay_Positvie || g_SysInitState.PIT0 ||\
     g_SysInitState.Screen || g_SysInitState.SPI||g_SysInitState.Insul)
  {
    g_SysInitState.AllInit_State = 1;
  }
  else
  {
    g_SysInitState.AllInit_State = 0;
  }
}












