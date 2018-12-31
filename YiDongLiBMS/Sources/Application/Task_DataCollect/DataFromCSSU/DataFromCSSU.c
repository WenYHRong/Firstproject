/*=======================================================================
 *Subsystem:   ���
 *File:        DataFromCSSU.c
 *Author:      WenYuhao
 *Description: ͨ�ţ�
               �ӿڣ�CAN2
               �����ʣ�500kb
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:
      Author:
      Modification:
========================================================================*/
  #include  "DataFromCSSU.h"
  #include  "BattInfoConfig.h"
  #include  "Task_DataProcess.h"     
  #include  "LTC6811_ConnectType.h"
/*=======================================================================
 *������:      DataFromCSSU(pCANFRAME data)  
 *����:        ������������
 *����:        ID������ID��       
 *���أ�       ��
 *˵����       ���հ����Ӱ壬��Ե��������
========================================================================*/
void DataFromCSSU(pCANFRAME data)
{
  uint8 i=0,j=0;
  uint16 Total_Temp;
  switch(data->m_ID) 
  { 
    //���ģ�鵥���ѹ��Ϣ
    case 0x18FF9700:                                      //�Ӱ�ɼ��ĵ����ѹ��Ϣ���ӵ�25�ڿ�ʼ�洢
      j = data->m_data[1];                                //6804�ɼ���ѹ���
      for(i=0; i<3&&(3*j+i<(SYS_SERIES-25)); i++) 
      {
         g_VoltInfo.CellVolt[25+(j*3+i)] = data->m_data[2+i*2] + (((uint16)data->m_data[3+i*2])<<8);
      }
    break;
    
    //���ģ�������С��ѹ��Ϣ  
    case 0x18FF9710:
      g_VoltInfo.CellVolt_Max     = data->m_data[0]+(((uint16)data->m_data[1])<<8);
      g_VoltInfo.CellVolt_MaxNode = data->m_data[2];
      g_VoltInfo.CellVolt_Min     = data->m_data[3]+(((uint16)data->m_data[4])<<8);
      g_VoltInfo.CellVolt_MinNode = data->m_data[5];
    break;
    
    //ÿ��ģ�鵥���¶���Ϣ
    case 0x18FF9800:
      for(i=0; i<SYS_NUMBER_MODULE_TEMP; i++) 
      {
         g_TempInfo.CellTemp[5 + i] = data->m_data[i+1];
      }
    break; 
    
    //ÿ��ģ�������С�¶���Ϣ  
    case 0x18FF9810:
      g_TempInfo.CellTemp_Max     = (data->m_data[0]);
      g_TempInfo.CellTemp_MaxNode = data->m_data[1];
      g_TempInfo.CellTemp_Min     = (data->m_data[2]);
      g_TempInfo.CellTemp_MinNode = data->m_data[3];
      
      Total_Temp   = (data->m_data[4] + (((uint16)data->m_data[5])<<8));
      g_TempInfo.CellTemp_Ave     = Total_Temp / SYS_NUMBER_MODULE_TEMP;
    break;  
    
    //ÿ��ģ����ѹ��Ϣ
    case 0x18FF9900:
      g_DataColletInfo.Flt_CSSU_TempH   = data -> m_data[0];
      g_OpenWireInfo.OpenWire_Status    = data -> m_data[1];
      g_DataColletInfo.SysVolt_Total    = (data -> m_data[2]+(((uint16)data -> m_data[3])<<8)+(((uint32)data -> m_data[4])<<16));
      g_DataColletInfo.InsulVolt_Total  = (data -> m_data[5]+(((uint16)data -> m_data[6])<<8)+(((uint32)data -> m_data[7])<<16));
    break;
      
    //���߿�·��Ϣ
    case 0x19FF9900:         //889
      g_OpenWireInfo.OpenWire_Node[NUM_IC+0]  = (((uint16)data->m_data[1])<<8) + (data->m_data[0]); 
      g_OpenWireInfo.OpenWire_Node[NUM_IC+1]  = (((uint16)data->m_data[3])<<8) + (data->m_data[2]);
      g_OpenWireInfo.OpenWire_Node[NUM_IC+2]  = (((uint16)data->m_data[5])<<8) + (data->m_data[4]);
    break;  
  } 
} 
  
  
  
  
  