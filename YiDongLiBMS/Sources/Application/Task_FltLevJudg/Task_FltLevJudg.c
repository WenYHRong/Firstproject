/*=======================================================================
 *Subsystem:   ���
 *File:        Task_FltLevJudg.c
 *Author:      Wenming
 *Description: 
 ========================================================================
 * History:    �޸���ʷ��¼�б��޸ļ�¼�����޸����ڡ��޸��߼��޸�����
 * 1. Date:
      Author:
      Modification:
========================================================================*/

#include  "Task_FltLevJudg.h"
#include  "WorkModeJudge.h"
#include  "Task_DataProcess.h"
#include  "Task_InsulDetect.h"
#include  "FltLevcfg.h"
#include  "Task_UpMonitor.h"

/*=======================================================================
                                  ����
========================================================================*/ 
Fault_DisCharge_T g_Flt_DisChg; 
Fault_Charge_T    g_Flt_Charge;
State_Offline_T   State_Offline;

 //�ŵ�����ж�
 static uint8 Fault_DisChg_VoltS(uint32 Volt,uint8 Temp); 
 static uint8 Fault_DisChg_VoltC(uint16 Volt,uint8 Temp);
 static uint8 Fault_DisChg_VoltCD(uint16 V_max,uint16 V_min);
 static uint8 Fault_DisChg_TempH(uint8 Temp);
 static uint8 Fault_DisChg_TempL(uint8 Temp);
 static uint8 Fault_DisChg_TempD(uint8 T_max,uint8 T_min);
 static uint8 Fault_DisChg_CurrH(float Current);
 static uint8 Fault_DisChg_Insul(uint16 Insul);
 //�������ж�
 static uint8 Fault_Charge_VoltS(uint32 Volt);
 static uint8 Fault_Charge_VoltCH(uint16 Volt);
 static uint8 Fault_Charge_VoltCD(uint16 V_max,uint16 V_min);
 static uint8 Fault_Charge_TempH(uint8 Temp);
 static uint8 Fault_Charge_TempL(uint8 Temp);
 static uint8 Fault_Charge_TempD(uint8 T_max,uint8 T_min);
 static uint8 Fault_Charge_CurrH(float Current);
 static uint8 Fault_Charge_Insul(uint16 Insul);

 /*=======================================================================
                              �����жϺ���0x00
 ========================================================================
 *������1:     Init_TaskFltLevJudg(void)
 *����:        
 *����:        ��    
 *���أ�       ��
 
 *˵����       
========================================================================*/ 
void Init_TaskFltLevJudg(void)
{
  //���ϵȼ��жϱ�������
  memset(&g_Flt_DisChg,           0, sizeof(Fault_DisCharge_T));  //�ŵ���ϵȼ�����
  memset(&g_Flt_Charge,           0, sizeof(Fault_Charge_T));        //�����ϵȼ�����
  memset(&State_Offline,          0, sizeof(State_Offline_T));        //���߹���״̬����
  memset(&HeartBeat,              0, sizeof(HeartBeat_T));        //�����ź�����
}



/*=======================================================================
                              �����жϺ���0x00
 ========================================================================
 *������1:     Task_FaultLevelJudge(uint8 workstate)
 *����:        ����ϵͳ��ѹ���¶ȡ������жϹ��ϵȼ�
 *����:        ��    
 *���أ�       ��
 
 *˵����       
========================================================================*/ 
void Task_FltLevJudg(uint8 workstate)
{
   switch(workstate)
   {
    case MODE_DISCHARGE: //�ŵ�״̬
      //������״̬����
      g_Flt_Charge.Level_Charge_SwitchOff_flag = 0; 
      g_Flt_Charge.Level_Charge_BalanceOff_Flag = 0;
      
      //�жϷŵ�״̬����
      g_Flt_DisChg.Level_Volt_Sys_Low           = Fault_DisChg_VoltS(g_DataColletInfo.SysVolt_Total, g_TempInfo.CellTemp_Ave);
      g_Flt_DisChg.Level_Volt_Cell_Low          = Fault_DisChg_VoltC(g_VoltInfo.CellVolt_Min, g_TempInfo.CellTemp_Ave);
      g_Flt_DisChg.Level_Volt_Cell_Diff_High    = Fault_DisChg_VoltCD(g_VoltInfo.CellVolt_Max, g_VoltInfo.CellVolt_Min);
      g_Flt_DisChg.Level_Temp_High              = Fault_DisChg_TempH(g_TempInfo.CellTemp_Max);
      g_Flt_DisChg.Level_Temp_Low               = Fault_DisChg_TempL(g_TempInfo.CellTemp_Min);
      g_Flt_DisChg.Level_Temp_Diff_High         = Fault_DisChg_TempD(g_TempInfo.CellTemp_Max, g_TempInfo.CellTemp_Min);
      g_Flt_DisChg.Level_Current_DisCharge_High = Fault_DisChg_CurrH(g_DataColletInfo.DataCollet_Current_Filter);
      g_Flt_DisChg.Level_Insul                  = Fault_DisChg_Insul(g_IsoDetect.insulation_resist);
      //�Ͽ��̵����Ķ������ϱ��
      if((g_Flt_DisChg.Level_Volt_Sys_Low==2) || \
         (g_Flt_DisChg.Level_Volt_Cell_Low == 2)||\
         (g_Flt_DisChg.Level_Temp_High == 2)||\
         (g_Flt_DisChg.Level_Temp_Low == 2) ||\
         (g_Flt_DisChg.Level_Current_DisCharge_High == 2) ||\
         (g_Flt_DisChg.Level_Insul == 2))
      {
        g_Flt_DisChg.Level_DisCharge_SwitchOff_flag = 1;  
      }
      
    break;
    
    case MODE_CHARGE:   //���״̬
      //����ŵ�״̬����
      g_Flt_DisChg.Level_Volt_Sys_Low           = 0;
      g_Flt_DisChg.Level_Volt_Cell_Low          = 0;
      g_Flt_DisChg.Level_DisCharge_SwitchOff_flag = 0;
      
      //�жϳ��״̬����
      g_Flt_Charge.Level_Volt_Sys_High        = Fault_Charge_VoltS(g_DataColletInfo.SysVolt_Total);
      g_Flt_Charge.Level_Volt_Cell_High       = Fault_Charge_VoltCH(g_VoltInfo.CellVolt_Max);
      g_Flt_Charge.Level_Volt_Cell_Diff_High  = Fault_Charge_VoltCD(g_VoltInfo.CellVolt_Max, g_VoltInfo.CellVolt_Min);
      g_Flt_Charge.Level_Temp_High            = Fault_Charge_TempH(g_TempInfo.CellTemp_Max);
      g_Flt_Charge.Level_Temp_Low             = Fault_Charge_TempL(g_TempInfo.CellTemp_Min);
      g_Flt_Charge.Level_Temp_Diff_High       = Fault_Charge_TempD(g_TempInfo.CellTemp_Max, g_TempInfo.CellTemp_Min);
      g_Flt_Charge.Level_Current_Charge_High  = Fault_Charge_CurrH(g_DataColletInfo.DataCollet_Current_Filter);
      g_Flt_Charge.Level_Insul                = Fault_Charge_Insul(g_IsoDetect.insulation_resist);
      //�Ͽ��̵����Ķ������ϱ��
      if((g_Flt_Charge.Level_Volt_Sys_High==2) || \
         (g_Flt_Charge.Level_Volt_Cell_High==2) || \
         (g_Flt_Charge.Level_Temp_High == 2)||\
         (g_Flt_Charge.Level_Temp_Low == 2)||\
         (g_Flt_Charge.Level_Insul == 2))
      {
        g_Flt_Charge.Level_Charge_SwitchOff_flag = 1;  
      }
      //����
      if((g_Flt_Charge.Level_Volt_Sys_High!=0) || \
         (g_Flt_Charge.Level_Volt_Cell_High != 0)||\
         (g_Flt_Charge.Level_Volt_Cell_Diff_High != 0)||\
         (g_Flt_Charge.Level_Temp_High != 0)||\
         (g_Flt_Charge.Level_Temp_Low != 0) ||\
         (g_Flt_Charge.Level_Temp_Diff_High != 0) ||\
         (g_Flt_Charge.Level_Current_Charge_High != 0) ||\
         (g_Flt_Charge.Level_Insul != 0))
      {
        g_Flt_Charge.Level_Charge_BalanceOff_Flag = 1;  
      }
      else
      {
        g_Flt_Charge.Level_Charge_BalanceOff_Flag = 0;
      }
      
    break;
   }
   
   //�������м������
  //Task_Flag_Cnt.F_FaultLevelJudge++;
  
}
/*============================�����жϺ���===============================*/

/*=======================================================================
                              �ŵ���ѹ��0x01
 ========================================================================
 *������1:     Fault1_VoltSys_DisCharge(uint8 Temp)
 *����:        �����¶�ѡ��ŵ���ѹ1��������ֵ
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ȼ�1��ѹ��ֵ 
 
 *������2:     Fault2_VoltSys_DisCharge(uint8 Temp)
 *����:        �����¶�ѡ��ŵ���ѹ2��������ֵ
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ȼ�2��ѹ��ֵ 
 
 *������3:     Recover1_VoltSys_DisCharge(uint8 Temp)
 *����:        �����¶�ѡ��ŵ���ѹ1���ָ�������ֵ
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ȼ�1��ѹ��ֵ 
 
 *������4:     Fault_DisChg_VoltS(uint32 Volt,uint8 Temp)
 *����:        ����ϵͳ��ѹ���¶��жϹ��ϵȼ�
 *����:        Volt(ϵͳ��ѹ)
               Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ŵ���ѹ�͵Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//-------------------�����¶�ȷ�����ϵȼ��Լ��ָ��ȼ���ֵ-----------------
//1�����Ϻ���
static
uint32 Fault1_VoltSys_DisCharge(uint8 Temp)
{
  if(Temp>=NTEMP_BOUNDARY)//����(>=0��)
  { 
    return (g_BMSMonitor_Volt.Volt_Sys_Low1*1000);//(28500)*(25)
  }
  else //����
  {
    return (g_BMSMonitor_New_LT.Voll_Sys_Low1_LT*1000);
  }
}
//2�����Ϻ���
static
uint32 Fault2_VoltSys_DisCharge(uint8 Temp)
{
  if(Temp>=NTEMP_BOUNDARY)//����(>=0��)
  { 
    return (g_BMSMonitor_Volt.Volt_Sys_Low2*1000);
  }
  else //����
  {
    return (g_BMSMonitor_New_LT.Voll_Sys_Low2_LT*1000);
  }
}
//1�ָ���0
static
uint32 Recover1_VoltSys_DisCharge(uint8 Temp)
{
  if(Temp>=NTEMP_BOUNDARY)//����(>=0��)
  { 
    return (g_BMSMonitor_Volt.Volt_Sys_Low1*1000 + 2000*SYS_SERIES_YiDongLi);
  }
  else //����
  {
    return (g_BMSMonitor_New_LT.Voll_Sys_Low1_LT*1000 + 1000);//�ɻ��ɱ궨����(�ֱ��ʣ�0.001V/λ)
  }
}

//-----------------���������ϵͳ��ѹ�ж����յĹ��ϵȼ�------------------
static uint8 Fault_DisChg_VoltS(uint32 Volt,uint8 Temp)  //����ϵͳ��ѹ�ͻ����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Volt<=Fault1_VoltSys_DisCharge(Temp))  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Volt<=Fault2_VoltSys_DisCharge(Temp))  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Volt <= Fault2_VoltSys_DisCharge(Temp))      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Volt >= Recover1_VoltSys_DisCharge(Temp))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*============================�ŵ���ѹ��===============================*/

/*=======================================================================
                              �ŵ絥���ѹ��0x02
 ========================================================================
 *������1:     Fault1_VoltCell_DisCharge(uint8 Temp)
 *����:        �����¶�ѡ��ŵ絥ѹ1��������ֵ
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ȼ�1��ѹ��ֵ 
 
 *������2:     Fault2_VoltCell_DisCharge(uint8 Temp)
 *����:        �����¶�ѡ��ŵ絥ѹ2��������ֵ
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ȼ�2��ѹ��ֵ 
 
 *������3:     Recover1_VoltCell_DisCharge(uint8 Temp)
 *����:        �����¶�ѡ��ŵ絥ѹ1���ָ�������ֵ
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ȼ�1��ѹ��ֵ 
  
 *������4:     Fault_DisChg_VoltC(uint16 Volt,uint8 Temp)
 *����:        ���ݵ����ѹ���¶��жϻָ�����ϵȼ�
 *����:        Volt(�����ѹ)
               Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ŵ絥ѹ�͵Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//1�����Ϻ���
static
uint16 Fault1_VoltCell_DisCharge(uint8 Temp)
{
  if(Temp>=NTEMP_BOUNDARY)//����(>=0��)
  { 
    return (g_BMSMonitor_Volt.Volt_Cell_Low1);
  }
  else //����
  {
    return (g_BMSMonitor_New_LT.Volt_Cell_Low1_LT);
  }
}
//2�����Ϻ���
static
uint16 Fault2_VoltCell_DisCharge(uint8 Temp)
{
  if(Temp>=NTEMP_BOUNDARY)//����(>=0��)
  { 
    return (g_BMSMonitor_Volt.Volt_Cell_Low2);
  }
  else //����
  {
    return (g_BMSMonitor_New_LT.Volt_Cell_Low2_LT);
  }
}
//�ָ���0
static
uint16 Recover1_VoltCell_DisCharge(uint8 Temp)
{
  if(Temp>=NTEMP_BOUNDARY)//����(>=0��)
  { 
    return (g_BMSMonitor_Volt.Volt_Cell_Low1 + 2000);
  }
  else //����
  {
    return (g_BMSMonitor_New_LT.Volt_Cell_Low1_LT + 1000);
  }
}

//�жϹ��ϵȼ�
static uint8 Fault_DisChg_VoltC(uint16 Volt,uint8 Temp)  //���뵥���ѹ�ͻ����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Volt<=Fault1_VoltCell_DisCharge(Temp))  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Volt<=Fault2_VoltCell_DisCharge(Temp))  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Volt <= Fault2_VoltCell_DisCharge(Temp))      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Volt >= Recover1_VoltCell_DisCharge(Temp))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*============================�ŵ絥���ѹ��===========================*/

/*=======================================================================
                              �ŵ絥��ѹ��0x03
 ========================================================================
 *������1:     Fault_DisChg_VoltCD(uint16 V_max,uint16 V_min)
 *����:        �������/�͵����ѹ�ж�ѹ����ϵȼ�
 *����:        V_max(������ߵ�ѹ)
               V_min(������͵�ѹ)       
 *���أ�       �ŵ�ѹ��Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_DisChg_VoltCD(uint16 V_max,uint16 V_min)  //���뵥�����/�͵�ѹ
{
  static uint16 V_Diff;
  static uint8 cnt[2];      //ʱ�����
  static uint8 FltL;
  
  V_Diff = V_max - V_min;   //�����ѹ��
  
  //�жϹ��ϵȼ�
  if(FltL==0)           //0������
  {
    if(V_Diff>=g_BMSMonitor_Volt.Volt_Cell_Diff1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0; 
    }
    cnt[1] = 0; 
  }
  else     //1������
  {
    if(V_Diff<=(V_Diff>=g_BMSMonitor_Volt.Volt_Cell_Diff1 - 500))    //1��0
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[0] = 0; 
  }
  return(FltL);
}
/*============================�ŵ絥��ѹ��============================*/

/*=======================================================================
                                �ŵ����0x04
 ========================================================================
 *������1:     Fault_DisChg_TempH(uint8 Temp)
 *����:        �����¶��жϸ��¹��ϵȼ�
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ŵ絥ѹ�͵Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_DisChg_TempH(uint8 Temp)  //�����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Temp>=g_BMSMonitor_Temp.Temp_DisCharge_High1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Temp>=g_BMSMonitor_Temp.Temp_DisCharge_High2)  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Temp>=g_BMSMonitor_Temp.Temp_DisCharge_High2)      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Temp<=(Temp>=g_BMSMonitor_Temp.Temp_DisCharge_High1 - 2))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*==============================�ŵ����==============================*/

/*=======================================================================
                                �ŵ����0x05
 ========================================================================
 *������1:     Fault_DisChg_TempL(uint8 Temp)
 *����:        �����¶��жϵ��¹��ϵȼ�
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �ŵ絥ѹ�͵Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_DisChg_TempL(uint8 Temp)  //�����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Temp<=g_BMSMonitor_Temp.Temp_DisCharge_Low1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Temp<=g_BMSMonitor_Temp.Temp_DisCharge_Low2)  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Temp<=g_BMSMonitor_Temp.Temp_DisCharge_Low2)      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Temp>=(Temp<=g_BMSMonitor_Temp.Temp_DisCharge_Low1 + 3))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*==============================�ŵ����==============================*/

/*=======================================================================
                              �ŵ��²�0x06
 ========================================================================
 *������1:     Fault_DisChg_TempD(uint8 T_max,uint8 T_min)
 *����:        �������/���¶��ж��²���ϵȼ�
 *����:        V_max(��������¶�)
               V_min(��������¶�)       
 *���أ�       �ŵ��²�Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_DisChg_TempD(uint8 T_max,uint8 T_min)  //���뵥�����/�͵�ѹ
{
  static uint8 T_Diff;
  static uint8 cnt[2];      //ʱ�����
  static uint8 FltL;
  
  T_Diff = T_max - T_min;   //�����ѹ��
  
  //�жϹ��ϵȼ�
  if(FltL==0)           //0������
  {
    if(T_Diff>=(g_BMSMonitor_Temp.Temp_DisCharge_Diff1-40))  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0; 
    }
    cnt[1] = 0; 
  }
  else     //1������
  {
    if(T_Diff<=(g_BMSMonitor_Temp.Temp_DisCharge_Diff1-40 - 2))    //1��0
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[0] = 0; 
  }
  return(FltL);
}
/*============================�ŵ絥���²�============================*/

/*=======================================================================
                                �ŵ����0x07
 ========================================================================
 *������1:     Fault_DisChg_CurrH(float Current)
 *����:        ���ݵ����жϹ�ѹ���ϵȼ�
 *����:        Current (�����¶�  ����ƫ����:-40)       
 *���أ�       �ŵ��ѹ�Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_DisChg_CurrH(float Current)//�������
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Current>=(g_BMSMonitor_Curr.Current_DisCharge_High1*0.1-750))  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Current>=(g_BMSMonitor_Curr.Current_DisCharge_High2*0.1-750))  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Current>=(g_BMSMonitor_Curr.Current_DisCharge_High2*0.1-750))      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Current<=(g_BMSMonitor_Curr.Current_DisCharge_High2*0.1-750 - 20))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*================================�ŵ����================================*/

/*=======================================================================
                                �ŵ��Ե����0x08
 ========================================================================
 *������1:     Fault_DisChg_Insul(uint16 Insul)
 *����:        ���ݵ����жϹ�ѹ���ϵȼ�
 *����:        Insul (�����Ե����)       
 *���أ�       �ŵ��Ե���ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_DisChg_Insul(uint16 Insul)
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    
    if(Insul<=(g_BMSMonitor_Insul.Insulation_Resis2*0.1))  //0��2
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[0] = 0;
    }
  }
  return(FltL);
}

/*================================�ŵ��Ե����================================*/


/*=======================================================================
                              �����ѹ��0x11
 ========================================================================
 *������1:     Fault_Charge_VoltS(uint32 Volt)
 *����:        ����ϵͳ��ѹ�жϹ��ϵȼ�
 *����:        Volt(ϵͳ��ѹ)
 *���أ�       �����ѹ�ߵĹ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_VoltS(uint32 Volt)  //����ϵͳ��ѹ
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Volt>=g_BMSMonitor_Volt.Volt_Sys_High1*1000)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Volt>=g_BMSMonitor_Volt.Volt_Sys_High2*1000)  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Volt>=g_BMSMonitor_Volt.Volt_Sys_High2*1000)      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Volt<=(Volt>=g_BMSMonitor_Volt.Volt_Sys_High1*1000 - 500*SYS_SERIES_YiDongLi))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*============================�����ѹ��===============================*/

/*=======================================================================
                              ��絥���ѹ��0x12
 ========================================================================
 *������1:     Fault_Charge_VoltCH(uint16 Volt)
 *����:        ���ݵ����ѹ�жϻָ�����ϵȼ�
 *����:        Volt(�����ѹ)
 *���أ�       ��絥���ѹ�ߵĹ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_VoltCH(uint16 Volt)  //���뵥���ѹ�ͻ����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Volt>=g_BMSMonitor_Volt.Volt_Cell_High1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Volt>=g_BMSMonitor_Volt.Volt_Cell_High2)  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Volt>=g_BMSMonitor_Volt.Volt_Cell_High2)      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Volt<=(Volt>=g_BMSMonitor_Volt.Volt_Cell_High1 - 500))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*============================��絥���ѹ��===========================*/

/*=======================================================================
                              ��絥��ѹ��0x13
 ========================================================================
 *������1:     Fault_Charge_VoltCD(uint16 V_max,uint16 V_min)
 *����:        ���ݵ����ѹ�жϳ�絥���ѹ�͹��ϵȼ�
 *����:        V_max(������ߵ�ѹ)
               V_min(������͵�ѹ)      
 *���أ�       ��絥��ѹ��Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_VoltCD(uint16 V_max,uint16 V_min)  //���뵥�����/�͵�ѹ
{
  static uint16 V_Diff;
  static uint8 cnt[2];      //ʱ�����
  static uint8 FltL;
  
  V_Diff = V_max - V_min;   //�����ѹ��
  
  //�жϹ��ϵȼ�
  if(FltL==0)           //0������
  {
    if(V_Diff>=g_BMSMonitor_Volt.Volt_Cell_Diff1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0; 
    }
    cnt[1] = 0; 
  }
  else     //1������
  {
    if(V_Diff<=(V_Diff>=g_BMSMonitor_Volt.Volt_Cell_Diff1 - 500))    //1��0
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[0] = 0; 
  }
  return(FltL);
}
/*=============================��絥��ѹ��=============================*/

/*=======================================================================
                                ������0x14
 ========================================================================
 *������1:     Fault_Charge_TempH(uint8 Temp)
 *����:        �����¶��жϸ��¹��ϵȼ�
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �����µĹ��ϵȼ�

 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_TempH(uint8 Temp)  //�����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Temp>=g_BMSMonitor_Temp.Temp_Charge_High1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Temp>=g_BMSMonitor_Temp.Temp_Charge_High2)  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Temp>=g_BMSMonitor_Temp.Temp_Charge_High2)      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Temp<=(Temp>=g_BMSMonitor_Temp.Temp_Charge_High1 - 2))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*==============================������==============================*/

/*=======================================================================
                                ������0x15
 ========================================================================
 *������1:     Fault_Charge_TempL(uint8 Temp)
 *����:        �����¶��жϵ��¹��ϵȼ�
 *����:        Temp (�����¶�  ����ƫ����:-40)       
 *���أ�       �����µĹ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_TempL(uint8 Temp)  //�����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Temp<=g_BMSMonitor_Temp.Temp_Charge_Low1)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Temp<=g_BMSMonitor_Temp.Temp_Charge_Low2)  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Temp<=g_BMSMonitor_Temp.Temp_Charge_Low2)      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Temp>=(g_BMSMonitor_Temp.Temp_Charge_Low1 + 5))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*==============================������==============================*/

/*=======================================================================
                              ����²�0x16
 ========================================================================
 *������1:     Fault_Charge_TempD(uint8 T_max,uint8 T_min)
 *����:        �������/���¶��ж��²���ϵȼ�
 *����:        T_max(��������¶�)
               T_min(��������¶�)       
 *���أ�       ����²�Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_TempD(uint8 T_max,uint8 T_min)  //���뵥�����/�͵�ѹ
{
  static uint8 T_Diff;
  static uint8 cnt[2];      //ʱ�����
  static uint8 FltL;
  
  T_Diff = T_max - T_min;   //�����ѹ��
  
  //�жϹ��ϵȼ�
  if(FltL==0)           //0������
  {
    if(T_Diff>=g_BMSMonitor_Temp.Temp_Charge_Diff1-40)  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0; 
    }
    cnt[1] = 0; 
  }
  else     //1������
  {
    if(T_Diff<=(g_BMSMonitor_Temp.Temp_Charge_Diff1-40 - 2))    //1��0
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[0] = 0; 
  }
  return(FltL);
}
/*================================����²�==============================*/

/*=======================================================================
                                ������0x17
 ========================================================================
 *������1:     Fault_Charge_CurrH(float Current)
 *����:        ���ݵ����жϳ��������ϵȼ�
 *����:        Current       
 *���أ�       �������Ĺ��ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_CurrH(float Current)//�����¶�
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    if(Current>=(g_BMSMonitor_Curr.Current_Charge_High1*0.1-750))  //0��1
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 1;
      }
    }
    else
    {
      cnt[0] = 0;
    }
    
    if(Current>=(g_BMSMonitor_Curr.Current_Charge_High2*0.1-750))  //0��2
    {
      if(++cnt[1]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[1] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[1] = 0;
    }
    cnt[2] = 0; 
    cnt[3] = 0;
  }
  else if(FltL == 1) //1������
  {
    if(Current>=(g_BMSMonitor_Curr.Current_Charge_High2*0.1-750))      //1��2
    {
      if(++cnt[2]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
         cnt[2] = 0;
         FltL = 2;
      }
    }
    else
    {
      cnt[2] = 0;
    }
    
    if(Current<=(g_BMSMonitor_Curr.Current_Charge_High1*0.1-750 - 5))    //1��0
    {
      if(++cnt[3]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[3] = 0;
        FltL = 0;
      }
    }
    else
    {
      cnt[3] = 0;
    }
    cnt[0] = 0; 
    cnt[1] = 0;
  }
  else   //2������
  {
    cnt[0] = 0; 
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0; 
  }
  return(FltL);
}
/*================================������================================*/

/*=======================================================================
                                ����Ե����0x18
 ========================================================================
 *������1:     Fault_Charge_Insul(uint16 Insul)
 *����:        ���ݵ����жϹ�ѹ���ϵȼ�
 *����:        Insul (�����Ե����)       
 *���أ�       �ŵ��Ե���ϵȼ�
 
 *˵����       
========================================================================*/ 
//�жϹ��ϵȼ�
static uint8 Fault_Charge_Insul(uint16 Insul)
{
  static uint8 cnt[4];      //ʱ�����
  static uint8 FltL;
  
  if(FltL==0)           //0������
  {
    
    if(Insul<=g_BMSMonitor_Insul.Insulation_Resis2*0.1)  //0��2
    {
      if(++cnt[0]*PERIOD_DISCHARGE/1000>=DELAYTIME_DANGERLEVEL2)
      {
        cnt[0] = 0;
        FltL = 2;
      }
    }
    else
    {
      cnt[0] = 0;
    }
  }
  return(FltL);
}

/*================================����Ե����================================*/


//==============================���߿�·����===================================
//ֱ�ӵ���g_OpenWireInfo.OpenWire_Status���������ж�
//==============================���߿�·����===================================