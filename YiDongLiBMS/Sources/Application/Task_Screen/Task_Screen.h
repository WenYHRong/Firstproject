/*=======================================================================
 *Subsystem:   ���
 *File:        Task_Screen.h
 *Author:      WenMing
 *Description: ͨ�ţ�SCI2.
               �ӿڣ�PJ0��RXD����PJ1��TXD��
               �����ʣ�
 ========================================================================
 * History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ���
 * 1. Date:
      Author:
      Modification:
========================================================================*/


#ifndef _TASK_SCREEN_H_
#define _TASK_SCREEN_H_

#include  "TypeDefinition.h"
#include  "MC9S12XEP100.h"


/*����ͷ�ļ����궨��*/
  
  #define Array_couple              22 //�궨�巢��˫�ֽ��������Ϊ22
  #define Array_single              12  //�궨�巢�͵��ֽ��������Ϊ11

  #define RS485_Enable       PORTA_PA6
  #define RS485_EnableDir    DDRA_DDRA6


  /*======��ʾ����ʾ����˫�ֽ�======*/

  
  typedef union
  { 
    uint8 SCI_Content2[Array_couple];
    struct
    {       
        uint16 RunningTime;           //����ʱ��
        uint16 BMS_Current;           //��������
      	uint16 BMS_SOC;               //SOC
      	uint16 BMS_SOH;               //SOH
      	uint16 Pack_Hightemp;         //���������¶�
      	uint16 Pack_Lowtemp;          //���������¶�
      	uint16 Pack_Volt;             //�������ѹ
      	uint16 Single_Maxvolt;        //������ߵ�ѹ
      	uint16 Single_Lowvolt;        //������͵�ѹ
      	uint16 iso_resistance;        //��Ե������ֵ
      	uint16 Lease_Time;            //����ʱ�� 
    }TX2;
  }RS485_couple;                  //˫�ֽ�SCI����   
  
  
  /*======��ʾ����ʾ���ݵ��ֽ�======*/
  typedef union
  {
    uint8 SCI_Content1[Array_single];
    struct
    {
      	uint8	Alam_SOC;                //SOC�澯
      	uint8	Alam_Hightemp;           //��������¶ȸ澯
      	uint8	Alam_Lowtemp;            //�������¶ȸ澯
      	uint8	Alam_Pack_highvolt;      //������ѹ�澯
      	uint8	Alam_Pack_Lowvolt;       //�����Ƿѹ�澯
      	uint8 Alam_single_highvolt;    //�����ѹ�澯
      	uint8 Alam_single_lowvolt;     //����Ƿѹ�澯
      	uint8 Alam_Charge_highcurr;    //�����������澯
      	uint8 Alam_Discharge_highcurr; //�����ŵ�����澯
      	uint8 Alam_communi_fail;       //���ͨ�Ź���
      	uint8 Alam_tenancy;            //������Ϣ����
      	uint8 Acc_offline;
    }TX1;     
  }RS485_single;                   //���ֽ�SCI����
             
  typedef struct
  {
      RS485_couple TxData_couple;
      RS485_single TxData_single;   
  }RS485;
  //extern RS485  RS485_Receive; 
      
  uint8 Init_Screen(void); 
  void Task_ScreenTransfer(void);
   
  
 #endif 

