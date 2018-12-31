/*=======================================================================
 *Subsystem:   ���
 *File:        Init_PIT.C
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

#include "Init_PIT.h"

/*=======================================================================
 *������:      Init_PIT(uint8 channel, uint8 perio)
 *����:        ��ʱ����ʼ������
 *����:        channel:ͨ��
               perio:����(ֻ��ѡ��1,5,10)      
 *���أ�       ��
 
 *˵����       
========================================================================*/
uint8 Init_PIT(uint8 channel, uint8 perio)      
{
  if(channel>7)
  {
    return (Init_Fault_PIT_Channel);
  }
  if((perio!=1)||(perio!=5)||(perio!=10))
  {
    return Init_Fault_PIT_Period;
  }
  switch (perio)
  {
    case 1:
      PITInit(channel, 1, 249, 129);
    break;
    
    case 5:
      PITInit(channel, 1, 249, 639);
    break;
    
    case 10:
      PITInit(channel, 1, 249, 1279);
    break;
      
  }
  return (Init_Normal_PIT);
}