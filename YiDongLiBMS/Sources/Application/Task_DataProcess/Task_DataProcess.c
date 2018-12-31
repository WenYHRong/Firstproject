/*=======================================================================
 *Subsystem:   ���
 *File:        Task_DataProcess.c
 *Author:      WenYuhao
 *Description: 
 ========================================================================
 * History:    
 * 1. Date:
      Author:
      Modification:  
========================================================================*/

#include  "Task_DataProcess.h"
#include  "LTC6811_ConnectType.h"
#include  "LTC6811_VoltCollect.h"
#include  "LTC6811_TempCollect.h"
#include  "LTC6811_OpWire.h" 
  
VoltInfo_T        g_VoltInfo;
TempInfo_T        g_TempInfo;
DataColletInfo_T  g_DataColletInfo; 
OpenWireInfo_T    g_OpenWireInfo;
InsulInfo_T       g_InsulInfo;

static void DataProcess_Volt(void);
static void DataProcess_Temp(void);
static void DataProcess_OpenWire(void);

/*=======================================================================
 *������:      Init_TaskDataProcess(void) 
 *����:        �ɼ���������ѹ���¶ȵ���Ϣ
 *����:        ��       
 *���أ�       ��
 *˵����       ����SOCʱ�����õĵ���ֵ,����Ҫ���ǻ����������Ƿ���һ����
========================================================================*/
void Init_TaskDataProcess(void)                       
{
  //�����Ϣ��������
  memset(&g_VoltInfo,   0, sizeof(VoltInfo_T));                  //��ص�ѹ��������
  memset(&g_TempInfo,   0, sizeof(TempInfo_T));                  //����¶���������
  memset(&g_InsulInfo,     0, sizeof(InsulInfo_T));        //��Ե��Ϣ����
  memset(&g_OpenWireInfo,    0, sizeof(OpenWireInfo_T));  //���߿�·��Ϣ����
  memset(&g_DataColletInfo,    0, sizeof(DataColletInfo_T));  //�ɼ���������
  
}


/*=======================================================================
 *������:      Task_DataProcess(void) 
 *����:        �ɼ���������ѹ���¶ȵ���Ϣ
 *����:        ��       
 *���أ�       ��
 *˵����       ����SOCʱ�����õĵ���ֵ,����Ҫ���ǻ����������Ƿ���һ����
========================================================================*/
void Task_DataProcess(void)                       
{
  DataProcess_Volt();
  DataProcess_Temp();
  DataProcess_OpenWire();
}

/*=======================================================================
 *������:      DataProcess_Volt()  
 *����:        �ɼ���ѹ��Ϣ
 *����:        ��       
 *���أ�       ��
 
 *˵����        
========================================================================*/
static
void DataProcess_Volt(void)
{
  uint8   i;
  uint16  max_v=0,min_v=0xFFFFFFFF;
  uint32  total_v=0;
  uint8   max_vnub=0,min_vnub=0;
  
  //��ȡ0~24�ڵ�صĵ�ѹ
  for(i=0;i<NUM_Battery;i++) 
  {
    g_VoltInfo.CellVolt[i] = g_LTC6811_VoltInfo.CellVolt[i];  
  }
  
  //��ȡ����ѹ�Լ�����λ��
  if (g_LTC6811_VoltInfo.CellVolt_Max > max_v) 
  {
    max_v    = g_LTC6811_VoltInfo.CellVolt_Max;
    max_vnub = g_LTC6811_VoltInfo.CellVolt_MaxNode;
  } 
  if (g_LTC6811_VoltInfo.CellVolt_Max > max_v) 
  {
    max_v    = g_VoltInfo.CellVolt_Max;
    max_vnub = g_VoltInfo.CellVolt_MaxNode;
  } 
  
  //��ȡ��С��ѹ�Լ�����λ��
  if (g_LTC6811_VoltInfo.CellVolt_Min < min_v) 
  {
    min_v     = g_LTC6811_VoltInfo.CellVolt_Min;
    min_vnub  = g_LTC6811_VoltInfo.CellVolt_MinNode;
  }  
  if (g_LTC6811_VoltInfo.CellVolt_Min < min_v) 
  {
    min_v     = g_VoltInfo.CellVolt_Min;
    min_vnub  = g_VoltInfo.CellVolt_MinNode;
  }  
      
  total_v = 0.5*(g_LTC6811_VoltInfo.CellVolt_Total + g_DataColletInfo.SysVolt_Total);                                 //ϵͳ��ѹ

  
  g_VoltInfo.CellVolt_Max         = max_v;
  g_VoltInfo.CellVolt_Min         = min_v; 
  g_VoltInfo.CellVolt_MaxNode     = max_vnub;
  g_VoltInfo.CellVolt_MinNode     = min_vnub;
  g_DataColletInfo.SysVolt_Total  = total_v;
  g_VoltInfo.CellVolt_Diff        = max_v - min_v;
  g_VoltInfo.CellVolt_Ave         = g_DataColletInfo.SysVolt_Total / SYS_SERIES_YiDongLi;
} 

/*=======================================================================
 *������:      DataProcess_Temp(void)  
 *����:        �ɼ���ѹ��Ϣ
 *����:        ��       
 *���أ�       ��
 
 *˵����        
========================================================================*/
static
void DataProcess_Temp(void) 
{
  uint8   i;
  uint16  max_t=0,min_t=0xFFFFFFFF;
  uint32  ave_t;
  uint8   max_tnub=0,min_tnub=0;
  
  //��ȡ0~24�ڵ���¶���Ϣ
  for(i=0;i<NUM_Battery;i++)
  {
    g_TempInfo.CellTemp[i] = g_LTC6811_TempInfo.CellTemp[i];
  }
  
  //��ȡ����¶ȼ�λ��
  if (g_LTC6811_TempInfo.CellTemp_Max > max_t) 
  {
    max_t     = g_LTC6811_TempInfo.CellTemp_Max;
    max_tnub  = g_LTC6811_TempInfo.CellTemp_MaxNode;
  } 
  if (g_TempInfo.CellTemp_Max > max_t) 
  {
    max_t     = g_TempInfo.CellTemp_Max;
    max_tnub  = g_TempInfo.CellTemp_MaxNode;
  }
  
  //��ȡ��С�¶ȼ�λ��
  if (g_LTC6811_TempInfo.CellTemp_Min < min_t) 
  {
    min_t     = g_LTC6811_TempInfo.CellTemp_Min;
    min_tnub  = g_LTC6811_TempInfo.CellTemp_MinNode;
  } 
  if (g_TempInfo.CellTemp_Min < min_t) 
  {
    min_t     = g_TempInfo.CellTemp_Min;
    min_tnub  = g_TempInfo.CellTemp_MinNode;
  }    
       
  ave_t = 0.5*(g_LTC6811_TempInfo.CellTemp_Ave + g_TempInfo.CellTemp_Ave);                                 //ϵͳƽ���¶�

  
  g_TempInfo.CellTemp_Max         = max_t;
  g_TempInfo.CellTemp_Min         = min_t; 
  g_TempInfo.CellTemp_MaxNode     = max_tnub;
  g_TempInfo.CellTemp_MinNode     = min_tnub;
  g_TempInfo.CellTemp_Ave         = ave_t;
  g_TempInfo.CellTemp_Diff        = max_t - min_t;
}

/*=======================================================================
 *������:      DataProcess_OpenWire(void)  
 *����:        �ɼ���ѹ��Ϣ
 *����:        ��       
 *���أ�       ��
 
 *˵����        
========================================================================*/
static
void DataProcess_OpenWire(void) 
{
  uint8   i;
  
  //��ȡ����ɼ��ĵ��߿�·��Ϣ
  for(i=0;i<NUM_IC;i++) 
  {
    g_OpenWireInfo.OpenWire_Node[i] = g_LTC6811_OpwireInfo.OpenwireLocation[i];
    
  }
  for(i=0;i<NUM_OPENWIRE;i++)
  {
    if(g_OpenWireInfo.OpenWire_Node[i] != 0)
    {
      g_OpenWireInfo.OpenWire_Status = 1;  //���߿�·����
      break;
    }
    else
    {
      g_OpenWireInfo.OpenWire_Status = 0;
    }
  }
  
}





