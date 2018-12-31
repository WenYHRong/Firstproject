/*=======================================================================
 *Subsystem:   ���
 *File:        CAN1.C
 *Author:      WenYuhao
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:
      Author:
      Modification:
========================================================================*/
  #include  "CAN.h"
  #include  "MC9S12XEP100.h"
  #include  "Task_Charge.h"
    
/*=======================================================================
 *������:      CAN1_Init(void)
 *����:        ��ʼ��CAN1
 *����:        
               sysclk��ʱ������ʱ��:32MHz
               baud:    
 *���أ�       ��
 *˵����       CAN1���
========================================================================*/
uint8 CAN1_Init(uint16 Baud_Rate) 
{
  uint8 Cnt[3];
  
  if((Baud_Rate != 125)||(Baud_Rate != 250)||(Baud_Rate != 500))
  {
    return(Init_Fault_CAN_BaudRate);
  }
  
  if(CAN1CTL0_INITRQ==0)      // ��ѯ�Ƿ�����ʼ��״̬   
  {
    CAN1CTL0_INITRQ =1;        // �����ʼ��״̬
  }
  else
  {
    return (Init_Fault_CAN_EnterState);
  }
  
  do
  {
    if(++Cnt[0]>=200)
    {
      Cnt[0] = 0;
      return(Init_Fault_CAN_Unready);
    }
  }
  while (CAN1CTL1_INITAK==0);  //�ȴ������ʼ��״̬
  Cnt[0] = 0;

  CAN1BTR0_SJW = 0;            //����ͬ��
  
  switch(Baud_Rate)
  {
    case 500:
      CAN1BTR0_BRP = 3;            //����Ԥ��Ƶֵ 
      CAN1BTR1 = 0x1c;     //����ʱ��1��ʱ��2��Tq���� 
      break;
    
    case 250:
      CAN1BTR0_BRP = 7;            //����Ԥ��Ƶֵ  
      CAN1BTR1 = 0x1c;     //����ʱ��1��ʱ��2��Tq���� 
      break;
      
    case 125:
      CAN1BTR0_BRP = 15;            //����Ԥ��Ƶֵ  
      CAN1BTR1 = 0x1c;     //����ʱ��1��ʱ��2��Tq���� 
      break;
  }
 
//�ر��˲���                                  
  CAN1IDMR0 = 0xFF;
  CAN1IDMR1 = 0xFF;
  CAN1IDMR2 = 0xFF;
  CAN1IDMR3 = 0xFF;
  CAN1IDMR4 = 0xFF;
  CAN1IDMR5 = 0xFF;
  CAN1IDMR6 = 0xFF;
  CAN1IDMR7 = 0xFF; 

  CAN1CTL1 = 0xC0;             //ʹ��MSCANģ��,����Ϊһ������ģʽ��ʹ������ʱ��Դ 

  CAN1CTL0 = 0x00;             //����һ��ģʽ����

  do
  {
    if(++Cnt[1]>=200)
    {
      Cnt[1] = 0;
      return(Init_Fault_CAN_Unready);
    }
  }
  while(CAN1CTL1_INITAK);      //�ȴ��ص�һ������ģʽ
  Cnt[1] = 0;
  
  do
  {
    if(++Cnt[2]>=200)
    {
      Cnt[2] = 0;
      return(Init_Fault_CAN_Unready);
    }
  }
  while(CAN1CTL0_SYNCH==0);    //�ȴ�����ʱ��ͬ��
  Cnt[2] = 0;
  
  CAN1RIER_RXFIE = 1;          //ʹ�ܽ����ж�
  
  return (Init_Normal_CAN);
}

/*=======================================================================
 *������:      CAN1_SendMsg
 *����:        CAN1��������
 *����:        ��չ֡
               sysclk��ʱ������ʱ��:32MHz
               baud:   
 *���أ�       ��
 *˵����       
========================================================================*/ 
uint8 CAN1_SendMsg(pCANFRAME sendFrame)
{
  uint8 send_buf,i;
  uint8 Cnt[1];
  
  // ������ݳ���
  if(sendFrame->m_dataLen > 8)
    return (SendMsg_Fault_Lenth);

  // �������ʱ��
  if(CAN1CTL0_SYNCH==0)
    return (SendMsg_Fault_NoEmptyNode);

  send_buf = 0;
  do
  {
    // Ѱ�ҿ��еĻ�����
    CAN1TBSEL=CAN1TFLG;
    send_buf=CAN1TBSEL;
    if(++Cnt[0]>=200)
    {
      Cnt[0] = 0;
      return(SendMsg_Fault_NoEmptyNode);
    }
  } 
  while(!send_buf); 
  //д���ʶ��ID
  
  if (sendFrame->m_IDE == 0)  //����׼֡���ID
  {
    CAN1TXIDR0 = (uint8)(sendFrame->m_ID>>3);
    CAN1TXIDR1 = (uint8)(sendFrame->m_ID<<5);
  } 
  else  //����չ֡���ID
  {
    CAN1TXIDR0 = (uint8)(sendFrame->m_ID>>21);
    CAN1TXIDR1 = (((uint8)(sendFrame->m_ID>>13)) & 0xe0)|0x18|(((uint8)(sendFrame->m_ID>>15)) &0x07);
    CAN1TXIDR2 = (uint8)(sendFrame->m_ID>>7);
    CAN1TXIDR3 = (uint8)(sendFrame->m_ID<<1);
  }
  
  if(sendFrame->m_RTR==1)
  {     
      CAN1TXIDR1 |= 0x10;
  }
      
  for (i=0;i<sendFrame->m_dataLen;++i)  
  {
    *((&CAN1TXDSR0) + i) = sendFrame->m_data[i];
  } 
      
  // д�����ݳ���
  CAN1TXDLR = sendFrame->m_dataLen;
  
  // д�����ȼ�
  CAN1TXTBPR = sendFrame->m_priority;
  
  // �� TXx ��־ (������׼������)
  CAN1TFLG = send_buf;
  
  return(SendMsg_Normal);
  
}

/*=======================================================================
 *������:      CAN1_GetMsg(pCANFRAME receiveFrame)
 *����:        CAN1��������
 *����:        ��չ֡
               sysclk��ʱ������ʱ��:32MHz
               baud:  
 *���أ�       ��
 *˵����       
========================================================================*/ 
uint8 CAN1_GetMsg(pCANFRAME receiveFrame)
{
  uint8 i;
	  
	if (!(CAN1RFLG_RXF))                         // �����ձ�־λ��
		return (GetMsg_Fault_RFLG_RXF);
	 
	if (CAN1RXIDR1_IDE == 0)                     // �յ���׼֡��
	{
		receiveFrame->m_ID = (uint32)(CAN1RXIDR0<<3) | (uint32)(CAN1RXIDR1>>5);
		receiveFrame->m_RTR = (CAN1RXIDR1>>4) & 0x01;
    receiveFrame->m_IDE = 0;
	} 
	else                                         // �յ���չ֡��
	{
	 	receiveFrame->m_ID = (((uint32)CAN1RXIDR0)<<21)|((uint32)(CAN1RXIDR1&0xe0)<<13)|((uint32)(CAN1RXIDR1&0x07)<<15)|(((uint32)CAN1RXIDR2)<<7);
    receiveFrame->m_ID = receiveFrame->m_ID | ((uint32)(CAN1RXIDR3&0xfe)>>1);
	}
	 
  receiveFrame->m_dataLen = CAN1RXDLR&0X0F;
  
  for (i=0;i<receiveFrame->m_dataLen;i++)       // ��ȡ����
  {
	   receiveFrame->m_data[i] = *(&(CAN1RXDSR0)+i);
  } 
  
  switch(receiveFrame->m_ID) 
  {
    case 0x112:
      CAN_ChargetoBMS(receiveFrame);
    break;
  }

  CAN1RFLG_RXF = 1;

  return(GetMsg_Normal);
}

/*=======================================================================
 *������:      Interrupt_CAN1(void)
 *����:        CAN1�����жϺ���
 *����:        ��չ֡
 *���أ�       ��
 *˵����       CAN1�Ľ����жϺ���
========================================================================*/
#pragma CODE_SEG __NEAR_SEG NON_BANKED
//CAN1���ճ������� 
interrupt void Interrupt_CAN1()            
{     
    pCANFRAME mgGet1;
    //Task_Flag_Time.CAN1++;    
    if (CAN1_GetMsg(mgGet1));
} 
#pragma CODE_SEG DEFAULT



