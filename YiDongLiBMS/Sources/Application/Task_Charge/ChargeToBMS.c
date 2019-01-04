                                              /*=======================================================================
 *Subsystem:   ���
 *File:        ChargeToBMS.C
 *Author:      Wen Yuhao
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
                                              
ChargePileBMS_T         ChargePileBMS;
Charge_State_T   g_Charge_State;
/*=======================================================================
 *������:      ChargePile_to_Bms(pCANFRAME data)
 *����:        ����������BMS
 *����:        ��       
 *���أ�       ��
 *˵����       
========================================================================*/
void CAN_ChargetoBMS(pCANFRAME data)
{
  uint8   Val8;
  uint16  Val;

  Val = (((uint16)(data -> m_data[0]))<<8) + (data -> m_data[1]);   
  if(Val!=0xFFFF) 
  {
     ChargePileBMS.Volt_ChargePileOut = (uint16)(Val);  
  }
  
  Val = (((uint16)(data -> m_data[2]))<<8) + (data -> m_data[3]);  
  if(Val!=0xFFFF) 
  {
     ChargePileBMS.Curr_ChargePileOut = (uint16)(Val);  
  }
  
  Val8 = (data -> m_data[5]); 
  if(Val8!=0xFF) 
  {
     g_Charge_State.Hard             = Val8&0x01; 
     g_Charge_State.TempH_ChargePile = (Val8>>1)&0x01; 
     g_Charge_State.VoltL_ChargePile = (Val8>>2)&0x01; 
     g_Charge_State.On_Line          = (Val8>>3)&0x01; 
     g_Charge_State.GetMsg           = (Val8>>4)&0x01;  
  }
  if(g_Charge_State.Hard || g_Charge_State.TempH_ChargePile ||\
    g_Charge_State.VoltL_ChargePile || g_Charge_State.On_Line ||\
    g_Charge_State.GetMsg)
  {
    g_Charge_State.FltState = 1;
  }
  else
  {
    g_Charge_State.FltState = 0;
  }
   
}         

