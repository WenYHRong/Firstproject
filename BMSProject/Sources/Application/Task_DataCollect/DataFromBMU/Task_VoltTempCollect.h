/*=======================================================================
 *Subsystem:   ���
 *File:        Task_VoltTempCollect.h
 *Author:      WenYuhao
 *Description: 
 ========================================================================
 * History:    
 * 1. Date:
      Author:
      Modification:
========================================================================*/

#ifndef _TASK_VOLTTEMPCOLLECT_H_
#define _TASK_VOLTTEMPCOLLECT_H_ 
  
  #include  "LTC6811_VoltCollect.h"
  #include  "LTC6811_TempCollect.h"
  #include  "LTC6811_OpWire.h"
 
  void Task_VoltCMDSend(void);
  void Task_VoltCollect(void);
  void Task_TempCMDSend(void);
  void Task_TempCollect(void);
  void Task_ChipTempCMDSend(void);
  void Task_ChipTempCollect(void);
  void Task_OpenWireDetect(void);
   
#endif