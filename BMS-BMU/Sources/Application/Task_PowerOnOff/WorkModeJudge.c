/*=======================================================================
 *Subsystem:   ���
 *File:        WorkModeJudge.C
 *Author:      WenYuhao
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
               ֻ���Լ�ɹ�����ܱպ���Ӧ�Ŀ���
/* ========================================================================
 * History:    �޸���ʷ��¼�б���
 * 1. Date:    
      Author:  
      Modification: 
========================================================================*/
#include  "includes.h"  

WorkStateJudge_T  g_WorkStateJudge;

/*=======================================================================
 *������:      ADC_CC2_State
 *����:        ��ذ����������������ļ��
 *����:        ��         
                      
 *���أ�       Current:����������ͨ����ת�����
 *˵����       ADC ʱ��Ƶ�ʣ�2MHz
========================================================================*/
/*static
uint16 ADC_CC2_State(void) 
{
  uint16 cc2_state;
  cc2_state = ADC_Value(QCharge_CC2); 
  return cc2_state;  
}*/

/*=======================================================================
 *������:      WorkMode_DelayTime(uint16)  
 *����:        ����״̬�ж���ʱ
 *����:        ts:��ʱ������       
 *���أ�       ��
 *˵����       ����AD�ɼ�ʱ���ж��ݵ���ʱ
========================================================================*/
/*static
void WorkMode_DelayTime(uint16 ts)
{
  uint16 i;
  for(i=0; i<ts; i++);
}*/ 
/*=======================================================================
 *������:      WokeModeJudgment(void)  
 *����:        �жϳ�绹�Ƿŵ�״̬
 *����:        ��       
 *���أ�       ��
 *˵����       ����ʱ�䵽��־λ��1��10msִ��һ��
========================================================================*/
uint8 WokeModeJudgment(void)                                
{  
  //�жϳ���״̬(���or�ŵ�)
  uint8 state;
  state = Fault_Charge_OffLine(); //�Գ�籨�����жϳ��״̬
  //state = !state;   
  return state;   
  /*uint8  i=0, q=0;
  for(i=0; i<10; i++)
  {
    if(ADC_CC2_State() <= 3000)
    {
      q++;
      WorkMode_DelayTime(20);
    }
  }   
  if(q == 10)//����10������ͬ��״̬
  {
    return MODE_CHARGE;   // ChagMode == 1
  }
  else
  {
    return MODE_DISCHARGE;
  }*/
}