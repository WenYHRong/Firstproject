#ifndef  TASK_CREATE_
#define  TASK_CREATE_

  #include  "TypeDefinition.h"
  
  #define ARRAY_SIZE   24                // �����ջ��С�� 

  typedef struct                         // ����ṹ�壻
  {
      uint8 flags;                       // �����ʶ��  
      void (*handle)();                  // ���������ĺ���ָ�룻 
  }TASK;   

  void Task_Handle(void);                //������ѯ
  void Task_Init(void);                  //������Ĵ���

#endif