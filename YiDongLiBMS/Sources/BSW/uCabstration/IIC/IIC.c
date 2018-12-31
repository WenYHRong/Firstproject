/*=======================================================================
 *Subsystem:   ���
 *File:        IIC.C
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
#include  "IIC.h"


/*=======================================================================
 *������:      uint8 IIC_Init(void) 
 *����:        IIC��ʼ������
 *����:        ��       
 *���أ�       ��
 
 *˵����       
========================================================================*/
uint8 IIC_Init(void) 
{

  SCL_dir = 1;
  SDA_dir = 1;
  SCL = 1;
  SDA = 1;
  
  IIC0_IBFD = 0x94;   //����ʱ��32MHz,����SCL��ƵΪ100KHz
  IIC0_IBCR = 0x80;   //ʹ��IICģ��,��ֹ�ж�
  IIC0_IBSR_IBAL = 1; //���IBAL��־λ  
  
  return(Init_Normal_IIC);
}

/* IIC������д���� */
uint8 IIC_write(uint8 addr,uint8 writeaddr,uint8 data) 
{
  uint8 cnt[6];
  
  IIC0_IBCR_TXAK = 0;               // ���յ����ݺ���Ӧ��
  IIC0_IBCR_TX_RX = 1;              // ���õ�Ƭ��Ϊ����ģʽ
  IIC0_IBCR_MS_SL = 1;              // ���õ�Ƭ��Ϊ����ģʽ��������ʼ�ź�

  IIC0_IBDR = addr;
  do
  {
    if(++cnt[0]==200)
    {
      return(Fault_IICWrite_IBIF1);
    }
  }
  while(IIC0_IBSR_IBIF == 0); 
  
  IIC0_IBSR_IBIF = 1;
  do
  {
    if(++cnt[1]==200)
    {
      return(Fault_IICWrite_RXAK1);
    }
  }
  while(IIC0_IBSR_RXAK);
  
  IIC0_IBDR = writeaddr;
  do
  {
    if(++cnt[2]==200)
    {
      return(Fault_IICWrite_IBIF2);
    }
  }
  while(IIC0_IBSR_IBIF == 0); 
  
  IIC0_IBSR_IBIF = 1;
  do
  {
    if(++cnt[3]==200)
    {
      return(Fault_IICWrite_RXAK2);
    }
  }
  while(IIC0_IBSR_RXAK);
  
  IIC0_IBDR =data;
  do
  {
    if(++cnt[4]==200)
    {
      return(Fault_IICWrite_IBIF3);
    }
  }
  while(IIC0_IBSR_IBIF == 0);
   
  IIC0_IBSR_IBIF = 1;
  do
  {
    if(++cnt[5]==200)
    {
      return(Fault_IICWrite_RXAK3);
    }
  }
  while(IIC0_IBSR_RXAK);

  IIC0_IBCR_MS_SL = 0;

  return(Normal_IICWrite);
}

/*IIC�����Ķ�����*/
uint8 IIC_read(uint8 addr,uint8 readaddr) 
{
  uint8 data;
  IIC0_IBCR_TXAK = 0;               // ���յ����ݺ���Ӧ��
  IIC0_IBCR_TX_RX = 1;              // ���õ�Ƭ��Ϊ����ģʽ
  IIC0_IBCR_MS_SL = 1;              // ���õ�Ƭ��Ϊ����ģʽ��������ʼ�ź�

  IIC0_IBDR = addr;                 // ���ʹ������ĵ�ַģʽΪд��
  while(IIC0_IBSR_IBIF == 0); 
  IIC0_IBSR_IBIF = 1;
  while(IIC0_IBSR_RXAK);
  
  IIC0_IBDR = readaddr;             // ���ʹ洢����ַ
  while(IIC0_IBSR_IBIF == 0); 
  IIC0_IBSR_IBIF = 1;
  while(IIC0_IBSR_RXAK);
  
  IIC0_IBCR_RSTA=1;                  
  
  IIC0_IBCR_TXAK = 0;               // ���յ����ݺ���Ӧ��
  IIC0_IBCR_TX_RX = 1;              // ���õ�Ƭ��Ϊ����ģʽ
  IIC0_IBCR_MS_SL = 1;              // ���õ�Ƭ��Ϊ����ģʽ��������ʼ�ź�

  IIC0_IBDR = addr+1;               // ���ʹ������ĵ�ַģʽΪ�������Լ�1��
  while(IIC0_IBSR_IBIF == 0); 
  IIC0_IBSR_IBIF = 1;
  while(IIC0_IBSR_RXAK);

  IIC0_IBCR_TX_RX = 0;             // ���õ�Ƭ��Ϊ����ģʽ
  IIC0_IBCR_TXAK = 1;              // ���յ����ݺ���Ӧ��

  data = IIC0_IBDR;                // ���IICD�Ĵ�����׼�����գ�

  while(IIC0_IBSR_IBIF == 0); 
  IIC0_IBSR_IBIF = 1;
  IIC0_IBCR_MS_SL = 0;

  data = IIC0_IBDR;                // ��ȡ���յ�������
  return(data);
}

  