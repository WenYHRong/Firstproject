/*=======================================================================
 *Subsystem:   裸机
 *File:        PIT.h
 *Author:      WenYuhao
 *Description: 
 ========================================================================
 * History:    
 * 1. Date:
      Author:
      Modification:
========================================================================*/

#ifndef _PIT_H_
#define _PIT_H_  

  #include  "TypeDefinition.h"
  #include  "MC9S12XEP100.h"

//开放或禁止PIT0溢出中断
    #define EnablePIT(channel)           PITINTE |= (1<<channel)    //开放定时器溢出中断
    #define DisablePIT(channel)          PITINTE &= ~(1<<channel)  //禁止定时器溢出中断

  //PIT初始化
  enum 
  {
    //PIT初始化正常
    Init_Normal_PIT = 0,
    Init_Fault_PIT_Channel,
    Init_Fault_PIT_Period
  };

  typedef struct 
  {
    //uint16 T10ms;       // 10ms任务时间标志;
    uint16 T500ms;      // 500ms任务时间标志;
    //uint16 T1s;         // 1s任务时间标志; 
  }PIT_TimePeriod_T;
  extern PIT_TimePeriod_T PIT_TimePeriod;  

  void PITInit(uint8 channel,uint8 MUXSEL,uint8 MTLD,uint16 LD);

#endif                                                                           
