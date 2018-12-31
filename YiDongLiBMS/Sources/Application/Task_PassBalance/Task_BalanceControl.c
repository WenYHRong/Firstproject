/*=======================================================================
 *Subsystem:   ���
 *File:        Task_BalanceControl.C
 *Author:      WenM
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:   
      Author:  
      Modification:  
      
========================================================================*/

#include  "Task_BalanceControl.h"
#include  "WorkModeJudge.h" 
#include  "LTC6811_ConnectType.h"
#include  "Task_DataProcess.h"
#include  "Task_FltLevJudg.h"
#include  "LTC6811_VoltCollect.h"

#include  "LTC6811_PassBalance.h"

/*=======================================================================
 *������:      BalanceControl_Strategy(flaot, uint8, uint16, uint32, uint8, float)
 *����:        �Ե������б�������
               ������Ʋ���:����������ƽ����ѹbalancevoltʱ���о���
 *����:        curr���жϵ����Ĵ�С,ֻ������С��3A(��̬)���о���
               faultflg:  �ڲ����ֹ��ϵ�����²Ž��о���
               voltmax��  ����ѹ
               totalvolt��25����ص���ѹ,������ƽ����ѹ
               balacenod: ����Ľڵ�                 
 
 *���أ�       uint8:     0��ʾ��������
                          1��ʾ������ִ��������ֹͣ����
                          2��ʾδ�ﵽ�������������о���
 *˵����       �������⺯��
========================================================================*/
static
uint8 BalanceControl_Strategy(float curr, uint8 faultflg, uint16 voltmax, uint32 totalvolt, uint8 balacenod, uint16 balancevolt)
{
  uint8 tskstate=2;                   //����2��ʾδ���о���
  static uint16 cnt;
  uint8 balanceNum;
  //static uint8 balanceflag; //���⿪��ָ��ȷ��ֻ��һ��
  
  if(abs(curr)>=10 && (faultflg == 0) && (g_WorkStateJudge.WorkState == MODE_CHARGE)) //ֻ���ڳ��Ĺ����е�������5A�ſ���
  {
    if((voltmax - (totalvolt/25.0)) > balancevolt)
    {
       if(++cnt*balanceperio/1000>2)//����2s,�����������Ƿ�����?
       {
         if(balacenod <= NUM1_Batper_true)
         {
            tskstate   = LTC6811_BalanceControl(balacenod, 0x00, 0x00, 1); 
         }
         else if(balacenod <= (NUM1_Batper_true+NUM2_Batper_true))
         {
            balanceNum = balacenod-NUM1_Batper_true;
            tskstate   = LTC6811_BalanceControl(0x00, balanceNum, 0x00, 1);
         }
         else if(balacenod <= (NUM1_Batper_true+NUM2_Batper_true+NUM3_Batper_true))
         {
            balanceNum = balacenod-NUM1_Batper_true-NUM2_Batper_true;
            tskstate   = LTC6811_BalanceControl(0x00, 0x00, balanceNum, 1);
         }
         else
         {
            return 1;
         }
         //balanceflag = 1;
       }
    }
  }
  else
  {
     //balanceflag = 0;
     tskstate = LTC6811_BalanceControl(0x00, 0x00, 0x00, 1); //δ�������������о��� 
     return 2;
  }
  return tskstate;
}

/*=======================================================================
 *������:      Task_BalanceControl_ON(void)
 *����:        ���⿪����������
 *����:        ��       
 *���أ�       ��
 *˵����       ���ݾ�����Ʋ��Կ����������,ֻ�ڵ�ѹ�ɼ����֮��������
========================================================================*/
void Task_BalanceControl_ON(void)
{
   uint8 balancestate;
   balancestate = BalanceControl_Strategy(g_DataColletInfo.DataCollet_Current_Filter, g_Flt_Charge.Level_Charge_BalanceOff_Flag,\
                                          g_LTC6811_VoltInfo.CellVolt_Max, g_LTC6811_VoltInfo.CellVolt_Total, g_LTC6811_VoltInfo.CellVolt_MaxNode, 500);
}
/*=======================================================================
 *������:      Task_BalanceControl_OFF(void)
 *����:        ����رտ�������
 *����:        ��       
 *���أ�       ��
 *˵����       �ڵ�ѹ�ɼ�֮ǰ�رվ��⿪��
========================================================================*/
void Task_BalanceControl_OFF(void)
{
   LTC6811_BalanceControl(0x00, 0x00, 0x00, 1);//�رվ��⹦��
}