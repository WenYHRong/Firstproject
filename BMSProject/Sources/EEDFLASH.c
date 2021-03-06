/*=======================================================================
 *Subsystem:   裸机
 *File:        EEDFLASH.C
 *Author:      Wenming
 *Description: 存储过程为：首先读取EEP中的EEprom_Data.pEErom_base值，确定好
               存储位置，再利用此值搜索变量存储区，再读取变量值
/* ========================================================================
 * History:    修改历史记录列表；
 * 1. Date:    2018 - 01 - 03
      Author:  Wenm
      Modification: 
========================================================================*/

#include"EEDFLASH.h" 
#include"Task_SOC.h"
#include"FaultLevelConfig.h"
#include"ADC_Config.h"
#include"Task_BmsAndUpMonitor.h"
  
EEprom_Data_T EEprom_Data; 

/*=======================================================================
 *函数名:      Get_EEprom_Value(void)
 *功能:        初始化获取EEprom的值
 *参数:        无       
 *返回：       无
 *说明：       在0x0D后的16个地址中存储地址基数，存储位置按此值来定位（0x0D00~0x0D10）
               如果需要进行初始化处理时，将Get_EEprom_Value中的预编译改为1。
========================================================================*/ 
void Get_EEprom_Value() 
{
    uint8 j,addrError=0;
    uint8*ptr=(uint8*)EEprom_Ptr;
    
    for(j=0; j<EEprom_Length; j++)//搜索标记位存储区的值
    {
      if(ptr[j] != 0xFF)        //判断标记位存储区里的值的有效性
      {
         addrError++; 
         EEprom_Data.pEErom_base = ptr[j];  
      }            
    } 
    #if(EEprom_Reset==1)   //此处决定是否需要进行数值初始化
       addrError = 0;
    #endif         
            
    if(addrError == 1) //判断EEprom存储的正确性，只有一次读到非0xff值为正确
    {
      EEprom_Data.EE_Value = 1;            
      EEprom_read(EEprom_Baseadrr, EEprom_Data.pEErom_base); //读取本次的值
    } 
    else //读到全是0xff的值时（即芯片第一次写入），或出现故障，需要进行数据初始化
    {
      EEprom_Data.EE_Value = 2;
      memset((uint8*)EEprom_Ptr, 0xFF, EEprom_Length*sizeof(uint8));
      memset((uint8*)EEprom_Baseadrr, 0xFF, EEprom_Length*Elem_Num*sizeof(uint8));
      //FlashErr = LaunchFlashCommand(2 ,FULL_PARTITION_D_FLASH, 0, DFPART, EEE_RAM, 0, 0, 0, 0, 0);
      EEprom_Init();        //数据重新初始化
      EEprom_Data.pEErom_base = EEprom_Length-1;  //将存储标记位定位至15个地址中，首次保存在0x08OO中
    } 
} 
 
    
 /*=======================================================================
 *函数名:      EEprom_Init
 *功能:        EEprom内存数据初始化
 *参数:        无                
 *返回：       无
 *说明：       将一些存储变量全部进行初始化处理（慎用），包括时钟初始化（按需要）
========================================================================*/ 

 void EEprom_Init() 
 {
   //经常存储的字节   
   EEprom_Data.Charge_Times = 0;
   EEprom_Data.BmsRunTime_min = 0; 
   EEprom_Data.SOC_Counter_Time = 0;
   EnergyInfo.Energy_Total_Charge = 0;
   EnergyInfo.Energy_Total_DisCharge = 0;
   SOCInfo.SOC_LowestVoltGet = 0;
   SOCInfo.SOC_HighestVoltGet = 0;
   //SOC_DATA.SOC_Float = 0;
   SOCInfo.SOC_ValueRead = 0;
   SOCInfo.SOC_ValueVoltGet = 0;
   
   //非经常存储的字节
   //0x1810C0F4 电池电压保护阈值信息
   //单体过压 00
   *(uint16*)0x0CA0 = F1_CHARGE_VOLTCH;   //单体电压过压一级阀值， 单位 0.0001V/Bit
   *(uint16*)0x0CA2 = F2_CHARGE_VOLTCH;   //单体电压过压二级阀值， 单位 0.0001V/Bit
   //*(uint16*)0x0CA4 = FaultL3_CellOverVolt;   //单体电压过压三级阀值， 单位 0.0001V/Bit
   
   //单体欠压 01
   *(uint16*)0x0CA8 = F1_DISCHG_VOLTCL_NT;  //单体电压欠压一级阀值， 单位 0.0001V/Bit
   *(uint16*)0x0CAA = F2_DISCHG_VOLTCL_NT;  //单体电压欠压二级阀值， 单位 0.0001V/Bit
   //*(uint16*)0x0CAC = FaultL3_CellUnderVolt;  //单体电压欠压三级阀值， 单位 0.0001V/Bit
   
   //系统过压 02
   *(uint16*)0x0CB0 = F1_CHARGE_VOLTSH*10; //电池组过压阀值1级
   *(uint16*)0x0CB2 = F2_CHARGE_VOLTSH*10; //电池组过压阀值2级
   //*(uint16*)0x0CB4 = FaultL3_SysOverVolt*10; //电池组过压阀值3级
   
   //系统欠压 03
   *(uint16*)0x0CB8 = F1_DISCHG_VOLTSL_NT*10;  //电池组欠压阀值1级
   *(uint16*)0x0CBA = F2_DISCHG_VOLTSL_NT*10;  //电池组欠压阀值2级
   //*(uint16*)0x0CBC = FaultL3_SysUnderVolt*10;  //电池组欠压阀值3级
   
   //单体压差 04
   *(uint16*)0x0CC0 = F1_DISCHG_VOLTCD;    //单体压差一级阀值， 单位 0.0001V/Bit;
   //*(uint16*)0x0CC2 = FaultL2_VoltDiff;    //单体压差二级阀值， 单位 0.0001V/Bit;
   //*(uint16*)0x0CC4 = FaultL3_VoltDiff;    //单体压差三级阀值， 单位 0.0001V/Bit;
   
   //系统压差 05
   //*(uint16*)0x0CC8 = FaultL1_PackCSSUDiffVolt*10;    //系统压差一级阀值， 单位 0.1V/Bit;
   //*(uint16*)0x0CCA = FaultL2_PackCSSUDiffVolt*10;    //系统压差二级阀值， 单位 0.1V/Bit;
   //*(uint16*)0x0CCC = FaultL3_PackCSSUDiffVolt*10;    //系统压差三级阀值， 单位 0.1V/Bit;
   
   //0x1811C0F4 电池温度保护阈值
   //（充电）电芯温度阈值 00
   //过高
   *(uint8*)0x0CD0 = F1_CHARGE_TEMPH;    //单体温度过温一级阀值， 单位 1℃/Bit  偏移量：-40
   *(uint8*)0x0CD1 = F2_CHARGE_TEMPH;    //单体温度过温二级阀值， 单位 1℃/Bit  偏移量：-40
   //*(uint8*)0x0CD2 = FaultL3_OverTemp + 40;    //单体温度过温三级阀值， 单位 1℃/Bit  偏移量：-40
   //过低
   *(uint8*)0x0CD3 = F1_CHARGE_TEMPL;   //单体温度低温一级阀值， 单位 1℃/Bit  偏移量：-40
   *(uint8*)0x0CD4 = F2_CHARGE_TEMPL;   //单体温度低温二级阀值， 单位 1℃/Bit  偏移量：-40
   //*(uint8*)0x0CD5 = FaultL3_UnderTemp + 40;   //单体温度低温三级阀值， 单位 1℃/Bit  偏移量：-40
   
   //（放电）电芯温度阈值 01
   //过高
   *(uint8*)0x0CD8 = F1_DISCHG_TEMPH;    //单体温度过温一级阀值， 单位 1℃/Bit   偏移量：-40
   *(uint8*)0x0CD9 = F2_DISCHG_TEMPH;    //单体温度过温二级阀值， 单位 1℃/Bit   偏移量：-40
   //*(uint8*)0x0CDA = FaultL3_OverTemp + 40;    //单体温度过温三级阀值， 单位 1℃/Bit   偏移量：-40
   //过低
   *(uint8*)0x0CDB = F1_DISCHG_TEMPL;   //单体温度低温一级阀值， 单位 1℃/Bit   偏移量：-40
   *(uint8*)0x0CDC = F2_DISCHG_TEMPL;   //单体温度低温二级阀值， 单位 1℃/Bit   偏移量：-40
   //*(uint8*)0x0CDD = FaultL3_UnderTemp + 40;   //单体温度低温三级阀值， 单位 1℃/Bit   偏移量：-40
   
   //电池温差 02
   //充电
   *(uint8*)0x0CE0 = F1_CHARGE_TEMPD;   //单体充电温差一级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CE1 = FaultL2_TempDiff + 40;   //单体充电温差二级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CE2 = FaultL3_TempDiff + 40;   //单体充电温差三级阀值， 单位 1℃/Bit    偏移量：-40
   //放电
   *(uint8*)0x0CE3 = F1_DISCHG_TEMPD;   //单体放电温差一级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CE4 = FaultL2_TempDiff + 40;   //单体放电温差二级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CE5 = FaultL3_TempDiff + 40;   //单体放电温差三级阀值， 单位 1℃/Bit    偏移量：-40
   
   //充电枪&BMS过温阈值  03
   //充电枪
   //*(uint8*)0x0CE8 = FaultL1_ChagerOverTemp + 40; //充电枪高温一级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CE9 = FaultL2_ChagerOverTemp + 40; //充电枪高温二级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CEA = FaultL3_ChagerOverTemp + 40; //充电枪高温三级阀值， 单位 1℃/Bit    偏移量：-40
   //BMU
   //*(uint8*)0x0CEB = FaultL1_BMUOverTemp + 40;   //BMS芯片高温一级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CEC = FaultL2_BMUOverTemp + 40;   //BMS芯片高温二级阀值， 单位 1℃/Bit    偏移量：-40
   //*(uint8*)0x0CED = FaultL3_BMUOverTemp + 40;   //BMS芯片高温三级阀值， 单位 1℃/Bit    偏移量：-40
   
   //0x1812C0F4 电池电流阈值及SOC阈值信息
   //系统过流（放电） 00
   *(uint16*)0x0CF0 = (F1_DISCHG_CURRH + 750)*10;      //放电过流阈值1级，0.1A/位     偏移量:-750
   *(uint16*)0x0CF2 = (F2_DISCHG_CURRH + 750)*10;      //放电过流阈值2级，0.1A/位     偏移量:-750
   //*(uint16*)0x0CF4 = (FaultL3_DiscCur + 750)*10;      //放电过流阈值3级，0.1A/位     偏移量:-750
   
   //系统过流（充电） 01
   *(uint16*)0x0CF8 = (F1_CHARGE_CURRH + 750)*10;      //充电过流阈值1级，0.1A/位      偏移量:-750
   *(uint16*)0x0CFA = (F2_CHARGE_CURRH + 750)*10;      //充电过流阈值2级，0.1A/位      偏移量:-750
   //*(uint16*)0x0CFC = (FaultL3_ChagCur + 750)*10;     //充电过流阈值3级，0.1A/位      偏移量:-750
   
   //SOC阈值  02
   //低SOC   
   //*(uint8*)0x0D00 = FaultL1_LowSOC;   //低SOC1级，1%/位    偏移量0
   //*(uint8*)0x0D01 = FaultL2_LowSOC;   //低SOC2级，1%/位    偏移量0
   //*(uint8*)0x0D02 = FaultL3_LowSOC;   //低SOC3级，1%/位    偏移量0
   //高SOC
   //*(uint8*)0x0D03 = FaultL1_HighSOC;   //高SOC1级，1%/位    偏移量0
   //*(uint8*)0x0D04 = FaultL2_HighSOC;   //高SOC2级，1%/位    偏移量0
   //*(uint8*)0x0D05 = FaultL3_HighSOC;   //高SOC3级，1%/位    偏移量0
   
   //0x1813C0F4 绝缘故障阈值
   //*(uint16*)0x0D08 = FAULT1_INSUL*10;       // 绝缘电阻一级阀值， 单位 0.1KΩ/Bit;
   *(uint16*)0x0D0A = F2_DISCHG_INSUL*10;       // 绝缘电阻二级阀值， 单位 0.1KΩ/Bit;
   //*(uint16*)0x0D0C = FAULT3_INSUL*10;       // 绝缘电阻三级阀值， 单位 0.1KΩ/Bit;
   
   //0x1915F4C0 特殊类信息
   *(uint16*)0x0D10 = 0;                          //SOC_init       单位 1%/Bit
   *(uint16*)0x0D12 = Stand_Volt*10000;           //Ref_Volt       参考电压标定(0~65536)
   *(uint16*)0x0D14 = Hall_Range;                 //Ref_Current    电流传感器的斜率
   
   //*(uint16*)0x0D18 = Trans_Volt*100;             //Volt_Resis     电压检测中分压电阻的标定
   *(uint32*)0x0D1A = 2000*SYS_CAPACITY*10;        //Pack_Total_Capacity     标定SOH中的总容量(2000*额定容量)
   
   *(uint16*)0x0D20 = EEprom_Data.BmsRunTime_min;        //上位机下标单位为小时存储为分钟
   
   //0x1810C0F4 电池电压保护阈值
   //单体过压 00
   BMSMonitor_Volt.Volt_Cell_High1 = (*(uint16*)(0x0CA0));
   BMSMonitor_Volt.Volt_Cell_High2 = (*(uint16*)(0x0CA2));
   //Monitor_BMS_Volt.Single_Over3_Volt = (*(uint16*)(0x0CA4)); 
   
   //单体欠压 01
   BMSMonitor_Volt.Volt_Cell_Low1 = (*(uint16*)(0x0CA8));
   BMSMonitor_Volt.Volt_Cell_Low2 = (*(uint16*)(0x0CAA));
   //Monitor_BMS_Volt.Single_Under3_Volt = (*(uint16*)(0x0CAC));

   //系统过压 02
   BMSMonitor_Volt.Volt_Sys_High1 = (*(uint16*)(0x0CB0));
   BMSMonitor_Volt.Volt_Sys_High2 = (*(uint16*)(0x0CB2));
   //Monitor_BMS_Volt.Pack_Over3_Volt = (*(uint16*)(0x0CB4));
   
   //系统欠压 03 
   BMSMonitor_Volt.Volt_Sys_Low1 = (*(uint16*)(0x0CB8));
   BMSMonitor_Volt.Volt_Sys_Low2 = (*(uint16*)(0x0CBA));
   //Monitor_BMS_Volt.Pack_Under3_Volt = (*(uint16*)(0x0CBC));
   
   //单体压差 04
   BMSMonitor_Volt.Volt_Cell_Diff1 = (*(uint16*)(0x0CC0));
   //Monitor_BMS_Volt.Single_Diff2_Volt = (*(uint16*)(0x0CC2));
   //Monitor_BMS_Volt.Single_Diff3_Volt = (*(uint16*)(0x0CC4));
   
   //系统压差 05
   //Monitor_BMS_Volt.System_Diff1_Volt = (*(uint16*)(0x0CC8));    //系统压差一级阀值， 单位 0.1V/Bit;
   //Monitor_BMS_Volt.System_Diff2_Volt = (*(uint16*)(0x0CCA));    //系统压差二级阀值， 单位 0.1V/Bit;
   //Monitor_BMS_Volt.System_Diff3_Volt = (*(uint16*)(0x0CCC));    //系统压差三级阀值， 单位 0.1V/Bit;
      
   //0x1811C0F4 电池温度保护阈值
   //充电电芯温度 00
   //过高
   BMSMonitor_Temp.Temp_Charge_High1 = (*(uint8*)(0x0CD0));
   BMSMonitor_Temp.Temp_Charge_High2 = (*(uint8*)(0x0CD1));
   //Monitor_BMS_Temp.Charge_Over3_Temp = (*(uint8*)(0x0CD2));
   //过低
   BMSMonitor_Temp.Temp_Charge_Low1 = (*(uint8*)(0x0CD3));
   BMSMonitor_Temp.Temp_Charge_Low2 = (*(uint8*)(0x0CD4));
   //Monitor_BMS_Temp.Charge_Under3_Temp = (*(uint8*)(0x0CD5));
   
   //放电电芯温度 01
   //过高
   BMSMonitor_Temp.Temp_DisCharge_High1 = (*(uint8*)(0x0CD8));
   BMSMonitor_Temp.Temp_DisCharge_High2 = (*(uint8*)(0x0CD9));
   //Monitor_BMS_Temp.DisCharge_Over3_Temp = (*(uint8*)(0x0CDA));
   //过低
   BMSMonitor_Temp.Temp_DisCharge_Low1 = (*(uint8*)(0x0CDB));
   BMSMonitor_Temp.Temp_DisCharge_Low2 = (*(uint8*)(0x0CDC));
   //Monitor_BMS_Temp.DisCharge_Under3_Temp = (*(uint8*)(0x0CDD));
   
   //电池温差 02
   //充电
   BMSMonitor_Temp.Temp_Charge_Diff1 = (*(uint8*)(0x0CE0));
   //Monitor_BMS_Temp.Charge_Diff2_Temp = (*(uint8*)(0x0CE1));
   //Monitor_BMS_Temp.Charge_Diff3_Temp = (*(uint8*)(0x0CE2));
   //放电
   BMSMonitor_Temp.Temp_DisCharge_Diff1 = (*(uint8*)(0x0CE3));
   //Monitor_BMS_Temp.DisCharge_Diff2_Temp = (*(uint8*)(0x0CE4));
   //Monitor_BMS_Temp.DisCharge_Diff3_Temp = (*(uint8*)(0x0CE5));
   
   //充电枪&BMS过温阈值  03
   //充电枪
   //Monitor_BMS_Temp.ChargeGun_Over1_Temp = (*(uint8*)(0x0CE8));
   //Monitor_BMS_Temp.ChargeGun_Over2_Temp = (*(uint8*)(0x0CE9));
   //Monitor_BMS_Temp.ChargeGun_Over3_Temp = (*(uint8*)(0x0CEA));
   //BMU
   //Monitor_BMS_Temp.Chip_Over1_Temp = (*(uint8*)(0x0CEB));
   //Monitor_BMS_Temp.Chip_Over2_Temp = (*(uint8*)(0x0CEC));
   //Monitor_BMS_Temp.Chip_Over3_Temp = (*(uint8*)(0x0CED));
   
   //0x1812C0F4 电池电流阈值信息
   //系统放电过流 00
   BMSMonitor_Curr.Current_DisCharge_High1 = (*(uint16*)(0x0CF0));
   BMSMonitor_Curr.Current_DisCharge_High2 = (*(uint16*)(0x0CF2));
   //Monitor_BMS_Current.DisCharge_Over3_Current = (*(uint16*)(0x0CF4));
   
   //系统充电过流 01
   BMSMonitor_Curr.Current_Charge_High1 = (*(uint16*)(0x0CF8));
   BMSMonitor_Curr.Current_Charge_High2 = (*(uint16*)(0x0CFA));
   //Monitor_BMS_Current.Charge_Over3_Current = (*(uint16*)(0x0CFC));
   
   //SOC阈值  02
   //低SOC   
   //Monitor_BMS_Current.SOC_Low1 = (*(uint8*)(0x0D00));   //低SOC1级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_Low2 = (*(uint8*)(0x0D01));   //低SOC2级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_Low3 = (*(uint8*)(0x0D02));   //低SOC3级，1%/位    偏移量0
   //高SOC
   //Monitor_BMS_Current.SOC_High1 = (*(uint8*)(0x0D03));   //高SOC1级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_High2 = (*(uint8*)(0x0D04));   //高SOC2级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_High3 = (*(uint8*)(0x0D05));   //高SOC3级，1%/位    偏移量0
   
   
   //0x1813C0F4 绝缘故障阈值信息
   BMSMonitor_Insu.Insulation_Resis1 = (*(uint16*)(0x0D08));
   BMSMonitor_Insu.Insulation_Resis2 = (*(uint16*)(0x0D0A));
   BMSMonitor_Insu.Insulation_Resis3 = (*(uint16*)(0x0D0C));
   
   //0x1915F4C0 特殊类的信息标定 
   //00
   MonitorBMS_SOC.SOC_Init = (*(uint16*)(0x0D10));
   SOCInfo.SOC_Init = MonitorBMS_SOC.SOC_Init;
   MonitorBMS_SOC.Ref_Volt = (*(uint16*)(0x0D12));
   MonitorBMS_SOC.Ref_Current = (*(uint16*)(0x0D14));
   
   //01
   MonitorBMS_SOC.Volt_Resis = (*(uint16*)(0x0D18));
   MonitorBMS_SOC.Pack_Total_Capacity = (*(uint32*)(0x0D1A)); 
   
   //02
   MonitorBMS_SOC.BMS_Running_Time = *(uint16*)(0x0D20);   //上位机下标单位为小时存储为分钟
   
 }  
 
   
 /*=======================================================================
 *函数名:      EEprom_read
 *功能:        从EEprom中读取值
 *参数:        addr：存储的起始位置
               addrbase：地址的偏移量
 *返回：       无
 *说明：       读取数据,地址为：0x0D10~0x0E70
========================================================================*/ 
static
void EEprom_read(uint16 addr, uint8 addrbase) 
{     
   //经常存储的字节
   EEprom_Data.Charge_Times =  *(uint16*)(addr + (addrbase%EEprom_Length)*Elem_Num); //充电次数
   EEprom_Data.BmsRunTime_min =  *(uint32*)(addr + (addrbase%EEprom_Length)*Elem_Num +2);
   EEprom_Data.SOC_Counter_Time = *(uint32*)(addr + (addrbase%EEprom_Length)*Elem_Num + 6); //SOC静置时长（h）
   EnergyInfo.Energy_Total_Charge = (*(uint32*)(addr + (addrbase%EEprom_Length)*Elem_Num + 10))/100.0;
   EnergyInfo.Energy_Total_DisCharge =  (*(uint32*)(addr + (addrbase%EEprom_Length)*Elem_Num + 14))/100.0;
   SOCInfo.SOC_LowestVoltGet =  (*(uint16*)(addr + (addrbase%EEprom_Length)*Elem_Num + 18))/10000.0;
   SOCInfo.SOC_HighestVoltGet = (*(uint16*)(addr + (addrbase%EEprom_Length)*Elem_Num + 20))/10000.0;       
   //SOC_DATA.SOC_Float =  (*(uint16*)(addr + (addrbase%EEprom_Length)*Elem_Num + 22))/10000.0;
   SOCInfo.SOC_ValueRead =   (*(uint16*)(addr + (addrbase%EEprom_Length)*Elem_Num + 22))/10000.0;
   SOCInfo.SOC_ValueVoltGet =   (*(uint16*)(addr + (addrbase%EEprom_Length)*Elem_Num + 24))/10000.0;
   
   //非经常存储的字节      
   //0x1810C0F4 电池电压保护阈值
   //单体过压 00
   BMSMonitor_Volt.Volt_Cell_High1 = (*(uint16*)(0x0CA0));
   BMSMonitor_Volt.Volt_Cell_High2 = (*(uint16*)(0x0CA2));
   //Monitor_BMS_Volt.Single_Over3_Volt = (*(uint16*)(0x0CA4)); 
   
   //单体欠压 01
   BMSMonitor_Volt.Volt_Cell_Low1 = (*(uint16*)(0x0CA8));
   BMSMonitor_Volt.Volt_Cell_Low2 = (*(uint16*)(0x0CAA));
   //Monitor_BMS_Volt.Single_Under3_Volt = (*(uint16*)(0x0CAC));

   //系统过压 02
   BMSMonitor_Volt.Volt_Sys_High1 = (*(uint16*)(0x0CB0));
   BMSMonitor_Volt.Volt_Sys_High2 = (*(uint16*)(0x0CB2));
   //Monitor_BMS_Volt.Pack_Over3_Volt = (*(uint16*)(0x0CB4));
   
   //系统欠压 03 
   BMSMonitor_Volt.Volt_Sys_Low1 = (*(uint16*)(0x0CB8));
   BMSMonitor_Volt.Volt_Sys_Low2 = (*(uint16*)(0x0CBA));
   //Monitor_BMS_Volt.Pack_Under3_Volt = (*(uint16*)(0x0CBC));
   
   //单体压差 04
   BMSMonitor_Volt.Volt_Cell_Diff1 = (*(uint16*)(0x0CC0));
   //Monitor_BMS_Volt.Single_Diff2_Volt = (*(uint16*)(0x0CC2));
   //Monitor_BMS_Volt.Single_Diff3_Volt = (*(uint16*)(0x0CC4));
   
   //系统压差 05
   //Monitor_BMS_Volt.System_Diff1_Volt = (*(uint16*)(0x0CC8));    //系统压差一级阀值， 单位 0.1V/Bit;
   //Monitor_BMS_Volt.System_Diff2_Volt = (*(uint16*)(0x0CCA));    //系统压差二级阀值， 单位 0.1V/Bit;
   //Monitor_BMS_Volt.System_Diff3_Volt = (*(uint16*)(0x0CCC));    //系统压差三级阀值， 单位 0.1V/Bit;
      
   //0x1811C0F4 电池温度保护阈值
   //充电电芯温度 00
   //过高
   BMSMonitor_Temp.Temp_Charge_High1 = (*(uint8*)(0x0CD0));
   BMSMonitor_Temp.Temp_Charge_High2 = (*(uint8*)(0x0CD1));
   //Monitor_BMS_Temp.Charge_Over3_Temp = (*(uint8*)(0x0CD2));
   //过低
   BMSMonitor_Temp.Temp_Charge_Low1 = (*(uint8*)(0x0CD3));
   BMSMonitor_Temp.Temp_Charge_Low2 = (*(uint8*)(0x0CD4));
   //Monitor_BMS_Temp.Charge_Under3_Temp = (*(uint8*)(0x0CD5));
   
   //放电电芯温度 01
   //过高
   BMSMonitor_Temp.Temp_DisCharge_High1 = (*(uint8*)(0x0CD8));
   BMSMonitor_Temp.Temp_DisCharge_High2 = (*(uint8*)(0x0CD9));
   //Monitor_BMS_Temp.DisCharge_Over3_Temp = (*(uint8*)(0x0CDA));
   //过低
   BMSMonitor_Temp.Temp_DisCharge_Low1 = (*(uint8*)(0x0CDB));
   BMSMonitor_Temp.Temp_DisCharge_Low2 = (*(uint8*)(0x0CDC));
   //Monitor_BMS_Temp.DisCharge_Under3_Temp = (*(uint8*)(0x0CDD));
   
   //电池温差 02
   //充电
   BMSMonitor_Temp.Temp_Charge_Diff1 = (*(uint8*)(0x0CE0));
   //Monitor_BMS_Temp.Charge_Diff2_Temp = (*(uint8*)(0x0CE1));
   //Monitor_BMS_Temp.Charge_Diff3_Temp = (*(uint8*)(0x0CE2));
   //放电
   BMSMonitor_Temp.Temp_DisCharge_Diff1 = (*(uint8*)(0x0CE3));
   //Monitor_BMS_Temp.DisCharge_Diff2_Temp = (*(uint8*)(0x0CE4));
   //Monitor_BMS_Temp.DisCharge_Diff3_Temp = (*(uint8*)(0x0CE5));
   
   //充电枪&BMS过温阈值  03
   //充电枪
   //Monitor_BMS_Temp.ChargeGun_Over1_Temp = (*(uint8*)(0x0CE8));
   //Monitor_BMS_Temp.ChargeGun_Over2_Temp = (*(uint8*)(0x0CE9));
   //Monitor_BMS_Temp.ChargeGun_Over3_Temp = (*(uint8*)(0x0CEA));
   //BMU
   //Monitor_BMS_Temp.Chip_Over1_Temp = (*(uint8*)(0x0CEB));
   //Monitor_BMS_Temp.Chip_Over2_Temp = (*(uint8*)(0x0CEC));
   //Monitor_BMS_Temp.Chip_Over3_Temp = (*(uint8*)(0x0CED));
   
   //0x1812C0F4 电池电流阈值信息
   //系统放电过流 00
   BMSMonitor_Curr.Current_DisCharge_High1 = (*(uint16*)(0x0CF0));
   BMSMonitor_Curr.Current_DisCharge_High2 = (*(uint16*)(0x0CF2));
   //Monitor_BMS_Current.DisCharge_Over3_Current = (*(uint16*)(0x0CF4));
   
   //系统充电过流 01
   BMSMonitor_Curr.Current_Charge_High1 = (*(uint16*)(0x0CF8));
   BMSMonitor_Curr.Current_Charge_High2 = (*(uint16*)(0x0CFA));
   //Monitor_BMS_Current.Charge_Over3_Current = (*(uint16*)(0x0CFC));
   
   //SOC阈值  02
   //低SOC   
   //Monitor_BMS_Current.SOC_Low1 = (*(uint8*)(0x0D00));   //低SOC1级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_Low2 = (*(uint8*)(0x0D01));   //低SOC2级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_Low3 = (*(uint8*)(0x0D02));   //低SOC3级，1%/位    偏移量0
   //高SOC
   //Monitor_BMS_Current.SOC_High1 = (*(uint8*)(0x0D03));   //高SOC1级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_High2 = (*(uint8*)(0x0D04));   //高SOC2级，1%/位    偏移量0
   //Monitor_BMS_Current.SOC_High3 = (*(uint8*)(0x0D05));   //高SOC3级，1%/位    偏移量0
   
   
   //0x1813C0F4 绝缘故障阈值信息
   BMSMonitor_Insu.Insulation_Resis1 = (*(uint16*)(0x0D08));
   BMSMonitor_Insu.Insulation_Resis2 = (*(uint16*)(0x0D0A));
   BMSMonitor_Insu.Insulation_Resis3 = (*(uint16*)(0x0D0C));
   
   //0x1915F4C0 特殊类的信息标定 
   //00
   MonitorBMS_SOC.SOC_Init = (*(uint16*)(0x0D10));
   SOCInfo.SOC_Init = MonitorBMS_SOC.SOC_Init;
   MonitorBMS_SOC.Ref_Volt = (*(uint16*)(0x0D12));
   MonitorBMS_SOC.Ref_Current = (*(uint16*)(0x0D14));
   
   //01
   MonitorBMS_SOC.Volt_Resis = (*(uint16*)(0x0D18));
   MonitorBMS_SOC.Pack_Total_Capacity = (*(uint32*)(0x0D1A)); 
   
   //02
   MonitorBMS_SOC.BMS_Running_Time = *(uint16*)(0x0D20);   //上位机下标单位为小时存储为分钟
    
}


/*=======================================================================
 *函数名:      Task_EEpromWrite
 *功能:        在EEprom中写值
 *参数:        addr：存储的起始位置
               addrbase：地址的偏移量
 *返回：       无
 *说明：       根据数据标记的基地址中的数据值，将变量值写入变量存储区
========================================================================*/ 
void Task_EEpromWrite(uint16 addr)
{
   uint8*ptr = (uint8*)EEprom_Ptr; 
   EEprom_Data.pEErom_base++;
   if(EEprom_Data.pEErom_base >= EEprom_Length) 
   {
      EEprom_Data.pEErom_base = 0;//重新回位
   }        
   *(ptr+EEprom_Data.pEErom_base) =  EEprom_Data.pEErom_base;         
          
   *(uint16*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num) = (uint16)EEprom_Data.Charge_Times;//充电次数
   delayus(5);
   *(uint32*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 2) = (uint32)EEprom_Data.BmsRunTime_min;//BMS运行时间
   delayus(5);
   *(uint32*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 6) = (uint32)EEprom_Data.SOC_Counter_Time; //SOC静置时间
   delayus(5);
   *(uint32*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 10) = (uint32)(EnergyInfo.Energy_Total_Charge*100);  //累计充电量
   delayus(5);
   *(uint32*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 14) = (uint32)(EnergyInfo.Energy_Total_DisCharge*100);    //累计放电量
   delayus(5);
   *(uint16*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 18) = (uint16)(SOCInfo.SOC_LowestVoltGet*10000);
   delayus(5);
   *(uint16*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 20) = (uint16)(SOCInfo.SOC_HighestVoltGet*10000);
   delayus(5);         
   *(uint16*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 22) = (uint16)(SOCInfo.SOC_ValueRead*10000);    //SOC显示值
   delayus(5);
   *(uint16*)(addr + (EEprom_Data.pEErom_base%EEprom_Length)*Elem_Num + 24) = (uint16)(SOCInfo.SOC_ValueVoltGet*10000);    //SOC查表值  
   delayus(5);
   ptr[(EEprom_Data.pEErom_base+EEprom_Length-1)%16] = 0xFF;//将上次地址标记清零的清为0xFF,不能放在存储之前，以防存储过程中中断。            
}
   

   