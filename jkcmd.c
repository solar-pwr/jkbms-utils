// JK BMS Command-line utility v0.9
//
// Jakub Strnad 2025
//
// Endianness conversion not implemented.
// Will only work on a little-endian machine (x86, x64, arm).

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

enum regType {
	T_UINT8,
	T_INT8,
	T_UINT16,
	T_INT16,
	T_UINT32,
	T_INT32,
	T_FLOAT,
	T_CHAR16,
	T_CHAR8
};

struct regSpec {
	uint16_t reg; // modbus register offset
	uint8_t type;
	char name[32];
};

const struct regSpec jkRegs[] = {
	{ 0x1000, T_UINT32, "VolSmartSleep" },
	{ 0x1004, T_UINT32, "VolCellUV" },
	{ 0x1008, T_UINT32, "VolCellUVPR" },
	{ 0x100c, T_UINT32, "VolCellOV" },
	{ 0x1010, T_UINT32, "VolCellOVPR" },
	{ 0x1014, T_UINT32, "VolBalanTrig" },
	{ 0x1018, T_UINT32, "VolSoc100" },
	{ 0x101c, T_UINT32, "VolSoc0" },
	{ 0x1020, T_UINT32, "VolCellRCV" },
	{ 0x1024, T_UINT32, "VolCellRFV" },
	{ 0x1028, T_UINT32, "VolSysPwrOff" },
	{ 0x102c, T_UINT32, "CurBatCOC" },
	{ 0x1030, T_UINT32, "TIMBatCOCPDly" },
	{ 0x1034, T_UINT32, "TIMBatCOCPRDly" },
	{ 0x1038, T_UINT32, "CurBatDcOC" },
	{ 0x103c, T_UINT32, "TIMBatDcOCPDly" },
	{ 0x1040, T_UINT32, "TIMBatDcOCPRDly" },
	{ 0x1044, T_UINT32, "TIMBatSCPRDly" },
	{ 0x1048, T_UINT32, "CurBalanMax" },
	{ 0x104c, T_UINT32, "TMPBatCOT" },
	{ 0x1050, T_INT32, "TMPBatCOTPR" },
	{ 0x1054, T_INT32, "TMPBatDcOT" },
	{ 0x1058, T_INT32, "TMPBatDcOTPR" },
	{ 0x105c, T_INT32, "TMPBatCUT" },
	{ 0x1060, T_INT32, "TMPBatCUTPR" },
	{ 0x1064, T_INT32, "TMPMosOT" },
	{ 0x1068, T_INT32, "TMPMosOTPR" },
	{ 0x106c, T_UINT32, "CellCount" },
	{ 0x1070, T_UINT32, "BatChargeEN" },
	{ 0x1074, T_UINT32, "BatDisChargeEN" },
	{ 0x1078, T_UINT32, "BalanEN" },
	{ 0x107c, T_UINT32, "CapBatCell" },
	{ 0x1080, T_UINT32, "SCPDelay" },
	{ 0x1084, T_UINT32, "VolStartBalan" },
	{ 0x1088, T_UINT32, "CellConWireRes0" },
	{ 0x108c, T_UINT32, "CellConWireRes1" },
	{ 0x1090, T_UINT32, "CellConWireRes2" },
	{ 0x1094, T_UINT32, "CellConWireRes3" },
	{ 0x1098, T_UINT32, "CellConWireRes4" },
	{ 0x109c, T_UINT32, "CellConWireRes5" },
	{ 0x10a0, T_UINT32, "CellConWireRes6" },
	{ 0x10a4, T_UINT32, "CellConWireRes7" },
	{ 0x10a8, T_UINT32, "CellConWireRes8" },
	{ 0x10ac, T_UINT32, "CellConWireRes9" },
	{ 0x10b0, T_UINT32, "CellConWireRes10" },
	{ 0x10b4, T_UINT32, "CellConWireRes11" },
	{ 0x10b8, T_UINT32, "CellConWireRes12" },
	{ 0x10bc, T_UINT32, "CellConWireRes13" },
	{ 0x10c0, T_UINT32, "CellConWireRes14" },
	{ 0x10c4, T_UINT32, "CellConWireRes15" },
	{ 0x10c8, T_UINT32, "CellConWireRes16" },
	{ 0x10cc, T_UINT32, "CellConWireRes17" },
	{ 0x10d0, T_UINT32, "CellConWireRes18" },
	{ 0x10d4, T_UINT32, "CellConWireRes19" },
	{ 0x10d8, T_UINT32, "CellConWireRes20" },
	{ 0x10dc, T_UINT32, "CellConWireRes21" },
	{ 0x10e0, T_UINT32, "CellConWireRes22" },
	{ 0x10e4, T_UINT32, "CellConWireRes23" },
	{ 0x10e8, T_UINT32, "CellConWireRes24" },
	{ 0x10ec, T_UINT32, "CellConWireRes25" },
	{ 0x10f0, T_UINT32, "CellConWireRes26" },
	{ 0x10f4, T_UINT32, "CellConWireRes27" },
	{ 0x10f8, T_UINT32, "CellConWireRes28" },
	{ 0x10fc, T_UINT32, "CellConWireRes29" },
	{ 0x1100, T_UINT32, "CellConWireRes30" },
	{ 0x1104, T_UINT32, "CellConWireRes31" },
	{ 0x1108, T_UINT32, "DevAddr" },
	{ 0x110c, T_UINT32, "TIMProdischarge" },
	{ 0x1114, T_UINT16, "Controls" },
	{ 0x1118, T_UINT8, "TIMSmartSleep" },
	{ 0x1119, T_UINT8, "DataDomainEnableControl" }, // from chinese

	{ 0x1200, T_UINT16, "CellVol0" },
	{ 0x1202, T_UINT16, "CellVol1" },
	{ 0x1204, T_UINT16, "CellVol2" },
	{ 0x1206, T_UINT16, "CellVol3" },
	{ 0x1208, T_UINT16, "CellVol4" },
	{ 0x120a, T_UINT16, "CellVol5" },
	{ 0x120c, T_UINT16, "CellVol6" },
	{ 0x120e, T_UINT16, "CellVol7" },
	{ 0x1210, T_UINT16, "CellVol8" },
	{ 0x1212, T_UINT16, "CellVol9" },
	{ 0x1214, T_UINT16, "CellVol10" },
	{ 0x1216, T_UINT16, "CellVol11" },
	{ 0x1218, T_UINT16, "CellVol12" },
	{ 0x121a, T_UINT16, "CellVol13" },
	{ 0x121c, T_UINT16, "CellVol14" },
	{ 0x121e, T_UINT16, "CellVol15" },
	{ 0x1220, T_UINT16, "CellVol16" },
	{ 0x1222, T_UINT16, "CellVol17" },
	{ 0x1224, T_UINT16, "CellVol18" },
	{ 0x1226, T_UINT16, "CellVol19" },
	{ 0x1228, T_UINT16, "CellVol20" },
	{ 0x122a, T_UINT16, "CellVol21" },
	{ 0x122c, T_UINT16, "CellVol22" },
	{ 0x122e, T_UINT16, "CellVol23" },
	{ 0x1230, T_UINT16, "CellVol24" },
	{ 0x1232, T_UINT16, "CellVol25" },
	{ 0x1234, T_UINT16, "CellVol26" },
	{ 0x1236, T_UINT16, "CellVol27" },
	{ 0x1238, T_UINT16, "CellVol28" },
	{ 0x123a, T_UINT16, "CellVol39" },
	{ 0x123c, T_UINT16, "CellVol30" },
	{ 0x123e, T_UINT16, "CellVol31" },
	{ 0x1240, T_UINT32, "CellSta" },
	{ 0x1244, T_UINT16, "CellVolAve" },
	{ 0x1246, T_UINT16, "CellVdifMax" },
	{ 0x1248, T_UINT8, "MaxVolCellNbr" },
	{ 0x1249, T_UINT8, "MinVolCellNbr" },
	{ 0x124a, T_UINT16, "CellWireRes0" },
	{ 0x124c, T_UINT16, "CellWireRes1" },
	{ 0x124e, T_UINT16, "CellWireRes2" },
	{ 0x1250, T_UINT16, "CellWireRes3" },
	{ 0x1252, T_UINT16, "CellWireRes4" },
	{ 0x1254, T_UINT16, "CellWireRes5" },
	{ 0x1256, T_UINT16, "CellWireRes6" },
	{ 0x1258, T_UINT16, "CellWireRes7" },
	{ 0x125a, T_UINT16, "CellWireRes8" },
	{ 0x125c, T_UINT16, "CellWireRes9" },
	{ 0x125e, T_UINT16, "CellWireRes10" },
	{ 0x1260, T_UINT16, "CellWireRes11" },
	{ 0x1262, T_UINT16, "CellWireRes12" },
	{ 0x1264, T_UINT16, "CellWireRes13" },
	{ 0x1266, T_UINT16, "CellWireRes14" },
	{ 0x1268, T_UINT16, "CellWireRes15" },
	{ 0x126a, T_UINT16, "CellWireRes16" },
	{ 0x126c, T_UINT16, "CellWireRes17" },
	{ 0x126e, T_UINT16, "CellWireRes18" },
	{ 0x1270, T_UINT16, "CellWireRes19" },
	{ 0x1272, T_UINT16, "CellWireRes20" },
	{ 0x1274, T_UINT16, "CellWireRes21" },
	{ 0x1276, T_UINT16, "CellWireRes22" },
	{ 0x1278, T_UINT16, "CellWireRes23" },
	{ 0x127a, T_UINT16, "CellWireRes24" },
	{ 0x127c, T_UINT16, "CellWireRes25" },
	{ 0x127e, T_UINT16, "CellWireRes26" },
	{ 0x1280, T_UINT16, "CellWireRes27" },
	{ 0x1282, T_UINT16, "CellWireRes28" },
	{ 0x1284, T_UINT16, "CellWireRes29" },
	{ 0x1286, T_UINT16, "CellWireRes30" },
	{ 0x1288, T_UINT16, "CellWireRes31" },
	{ 0x128a, T_INT16, "TempMos" },
	{ 0x128c, T_UINT32, "CellWireResSta" },
	{ 0x1290, T_UINT32, "TotBatVol" }, // BatVol duplicate (chinese: total battery voltage)
	{ 0x1294, T_UINT32, "BatWatt" },
	{ 0x1298, T_INT32, "BatCurrent" },
	{ 0x129c, T_INT16, "TempBat1" },
	{ 0x129e, T_INT16, "TempBat2" },
	{ 0x12a0, T_UINT32, "Alarm" },
	{ 0x12a4, T_UINT16, "BalanCurrent" },
	{ 0x12a6, T_UINT8, "BalanSta" },
	{ 0x12a7, T_UINT8, "SOCStateOfCharge" },
	{ 0x12a8, T_INT32, "SOCCapRemain" },
	{ 0x12ac, T_UINT32, "SOCFullChargeCap" },
	{ 0x12b0, T_UINT32, "SOCCycleCount" },
	{ 0x12b4, T_UINT32, "SOCCycleCap" },
	{ 0x12b8, T_UINT8, "SOCSOH" },
	{ 0x12b9, T_UINT8, "Precharge" },
	{ 0x12ba, T_UINT16, "UserAlarm" },
	{ 0x12bc, T_UINT32, "Runtime" },
	{ 0x12c0, T_UINT8, "Charge" },
	{ 0x12c1, T_UINT8, "Discharge" },
	{ 0x12c2, T_UINT16, "UserAlarm2" },
	{ 0x12c4, T_UINT16, "TimeDcOCPR" },
	{ 0x12c6, T_UINT16, "TimeDcSCPR" },
	{ 0x12c8, T_UINT16, "TimeCOCPR" },
	{ 0x12ca, T_UINT16, "TimeCSCPR" },
	{ 0x12cc, T_UINT16, "TimeUVPR" },
	{ 0x12ce, T_UINT16, "TimeOVPR" },
	{ 0x12d0, T_UINT8, "TempSensorAbsent" },
	{ 0x12d1, T_UINT8, "TempSensorHeating" },
	{ 0x12d2, T_UINT16, "Reserved" },
	{ 0x12d4, T_UINT16, "TimeEmergency" },
	{ 0x12d6, T_UINT16, "BatDisCurCorrect" },
	{ 0x12d8, T_UINT16, "VolChargCur" },
	{ 0x12da, T_UINT16, "VolDischargCur" },
	{ 0x12dc, T_FLOAT, "BatVolCorrect" },
	{ 0x12e4, T_UINT16, "BatVol" }, // BatVol duplicate (chinese: battery voltage)
	{ 0x12e6, T_INT16, "HeatCurrent" },
	{ 0x12ee, T_UINT8, "RVD1" },
	{ 0x12ef, T_UINT8, "ChargerPlugged" },
	{ 0x12f0, T_UINT32, "SysRunTicks" },
	{ 0x12f8, T_INT16, "TempBat3" },
	{ 0x12fa, T_INT16, "TempBat4" },
	{ 0x12fc, T_INT16, "TempBat5" },
	{ 0x1300, T_UINT32, "RTCTicks" },
	{ 0x1308, T_UINT32, "TimeEnterSleep" },
	{ 0x130c, T_UINT8, "PCLModuleSta" },
	{ 0x130d, T_UINT8, "RVD2" },

	{ 0x1400, T_CHAR16, "ManufacturerDeviceID" },
	{ 0x1410, T_CHAR8, "HardwareVersion" },
	{ 0x1418, T_CHAR8, "SoftwareVersion" },
	{ 0x1420, T_UINT32, "ODDRunTime" },
	{ 0x1424, T_UINT32, "PWROnTimes" },
	{ 0x1470, T_CHAR16, "Password" },
	{ 0x14b2, T_UINT8, "UART1MPRTOLNbr" },
	{ 0x14b3, T_UINT8, "CANMPRTOLNbr" },
	{ 0x14b4, T_UINT8, "UART1MPRTOLEnable0" },
	{ 0x14b5, T_UINT8, "UART1MPRTOLEnable1" },
	{ 0x14b6, T_UINT8, "UART1MPRTOLEnable2" },
	{ 0x14b7, T_UINT8, "UART1MPRTOLEnable3" },
	{ 0x14b8, T_UINT8, "UART1MPRTOLEnable4" },
	{ 0x14b9, T_UINT8, "UART1MPRTOLEnable5" },
	{ 0x14ba, T_UINT8, "UART1MPRTOLEnable6" },
	{ 0x14bb, T_UINT8, "UART1MPRTOLEnable7" },
	{ 0x14bc, T_UINT8, "UART1MPRTOLEnable8" },
	{ 0x14bd, T_UINT8, "UART1MPRTOLEnable9" },
	{ 0x14be, T_UINT8, "UART1MPRTOLEnable10" },
	{ 0x14bf, T_UINT8, "UART1MPRTOLEnable11" },
	{ 0x14c0, T_UINT8, "UART1MPRTOLEnable12" },
	{ 0x14c1, T_UINT8, "UART1MPRTOLEnable13" },
	{ 0x14c2, T_UINT8, "UART1MPRTOLEnable14" },
	{ 0x14c3, T_UINT8, "UART1MPRTOLEnable15" },
	{ 0x14c4, T_UINT8, "CANMPRTOLEnable0" },
	{ 0x14c5, T_UINT8, "CANMPRTOLEnable1" },
	{ 0x14c6, T_UINT8, "CANMPRTOLEnable2" },
	{ 0x14c7, T_UINT8, "CANMPRTOLEnable3" },
	{ 0x14c8, T_UINT8, "CANMPRTOLEnable4" },
	{ 0x14c9, T_UINT8, "CANMPRTOLEnable5" },
	{ 0x14ca, T_UINT8, "CANMPRTOLEnable6" },
	{ 0x14cb, T_UINT8, "CANMPRTOLEnable7" },
	{ 0x14cc, T_UINT8, "CANMPRTOLEnable8" },
	{ 0x14cd, T_UINT8, "CANMPRTOLEnable9" },
	{ 0x14ce, T_UINT8, "CANMPRTOLEnable10" },
	{ 0x14cf, T_UINT8, "CANMPRTOLEnable11" },
	{ 0x14d0, T_UINT8, "CANMPRTOLEnable12" },
	{ 0x14d1, T_UINT8, "CANMPRTOLEnable13" },
	{ 0x14d2, T_UINT8, "CANMPRTOLEnable14" },
	{ 0x14d3, T_UINT8, "CANMPRTOLEnable15" },
	{ 0x14d4, T_UINT8, "UART2MPRTOLNbr" },
	{ 0x14d5, T_UINT8, "UART2MPRTOLEnable" },
	{ 0x14e4, T_UINT8, "LCDBuzzerTrigger" },
	{ 0x14e5, T_UINT8, "DRY1Trigger" },
	{ 0x14e6, T_UINT8, "DRY2Trigger" },
	{ 0x14e7, T_UINT8, "UARTMPTLVer" },
	{ 0x14e8, T_INT32, "LCDBuzzerTriggerVal" },
	{ 0x14ec, T_INT32, "LCDBuzzerReleaseVal" },
	{ 0x14f0, T_INT32, "DRY1TriggerVal" },
	{ 0x14f4, T_INT32, "DRY1ReleaseVal" },
	{ 0x14f8, T_INT32, "DRY2TriggerVal" },
	{ 0x14fc, T_INT32, "DRY2ReleaseVal" },
	{ 0x1500, T_INT32, "DataStoredPeriod" },
	{ 0x1504, T_UINT8, "RCVTime" },
	{ 0x1505, T_UINT8, "RFVTime" },
	{ 0x1506, T_UINT8, "CANMPTLVer" },
	{ 0x1507, T_UINT8, "RVD3" },

	{ 0x1600, T_UINT16, "VoltageCalibration" },
	{ 0x1604, T_UINT16, "Shutdown" },
	{ 0x1606, T_UINT16, "CurrentCalibration" },
	{ 0x160a, T_UINT16, "LI-ION" },
	{ 0x160c, T_UINT16, "LIFEPO4" },
	{ 0x160e, T_UINT16, "LTO" },
	{ 0x1610, T_UINT16, "Emergency" },
	{ 0x1612, T_UINT32, "Timecalibration" },
};

uint8_t jkSign[] = { 0x55, 0xaa, 0xeb, 0x90 };

const uint16_t crcTable[] = {
   0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
   0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
   0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
   0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
   0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
   0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
   0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
   0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
   0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
   0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
   0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
   0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
   0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
   0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
   0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
   0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
   0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
   0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
   0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
   0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
   0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
   0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
   0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
   0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
   0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
   0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
   0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
   0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
   0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
   0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
   0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
   0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

uint8_t jkCmd[] = { 0x00, 0x10, 0x16, 0x20, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00 };

uint16_t regBase;
uint8_t buf[0x300], *dataPtr;
char regVal[128];
int serial;
struct termios tty;

void readRegVal(int i) {
	uint8_t *addr;

	addr = dataPtr + jkRegs[i].reg - regBase;

	switch (jkRegs[i].type) {
	case T_UINT8:
		sprintf(regVal, "%"PRIu8, *(uint8_t*) addr);
		break;
	case T_INT8:
		sprintf(regVal, "%"PRIi8, *(int8_t*) addr);
		break;
	case T_UINT16:
		sprintf(regVal, "%"PRIu16, *(uint16_t*) addr);
		break;
	case T_INT16:
		sprintf(regVal, "%"PRIi16, *(int16_t*) addr);
		break;
	case T_UINT32:
		sprintf(regVal, "%"PRIu32, *(uint32_t*) addr);
		break;
	case T_INT32:
		sprintf(regVal, "%"PRIi32, *(int32_t*) addr);
		break;
	case T_FLOAT:
		sprintf(regVal, "%f", *(float*) addr);
		break;
	case T_CHAR16:
		memcpy(regVal, addr, 16);
		regVal[16] = 0;
		break;
	case T_CHAR8:
		memcpy(regVal, addr, 8);
		regVal[8] = 0;
		break;
	}
}

uint16_t calcCrc(uint8_t *data, size_t len) {
	uint8_t tmp;
	uint16_t crc;
	size_t i;
	
	crc = 0xffff;
	for (i = 0; i < len; i++) {
		tmp = crc ^ data[i];
		crc >>= 8;
		crc ^= crcTable[tmp];
	}
	return crc;
}

int openSerial(char *spath) {
	serial = open(spath, O_RDWR);
	if (serial == -1) {
		fprintf(stderr, "Can't open %s\n", spath);
		close(serial);
		return 1;
	}
	tcgetattr(serial, &tty);
	tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
	tty.c_cflag |= CS8; // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO; // Disable echo
	tty.c_lflag &= ~ECHOE; // Disable erasure
	tty.c_lflag &= ~ECHONL; // Disable new-line echo
	tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

	tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN] = 0;
	cfsetispeed(&tty, B115200);
	cfsetospeed(&tty, B115200);
	tcsetattr(serial, TCSANOW, &tty);

	return 0;
}

int jkRead(uint8_t addr, int regGroup) {
	int nRead, i;

	// send Modbus request
	jkCmd[0] = addr;
	switch (regGroup) {
	case 1: jkCmd[3] = 0x1e; regBase = 0x1000; break;
	case 2: jkCmd[3] = 0x20; regBase = 0x1200; break;
	case 3: jkCmd[3] = 0x1c; regBase = 0x1400; break;
	}
	*(uint16_t*) (jkCmd + sizeof(jkCmd) - 2) = calcCrc(jkCmd, sizeof(jkCmd) - 2);
	write(serial, jkCmd, sizeof(jkCmd));
	
	// recieve data
	i = 0;
	nRead = 0;
	do {
		i += nRead;
		nRead = read(serial, buf + i, sizeof(buf) - i);
	} while (nRead > 0);
	nRead = i;

	if (nRead < 6) {
		fprintf(stderr, "No response or response too short\n");
		return 1;
	}

	for (i = 0; i < nRead - 6; i++) {
		if (memcmp(buf + i, jkSign, sizeof(jkSign)) == 0 &&
			*(uint16_t*) (buf + i + 4) == regGroup) {

			dataPtr = buf + i + 6;
			goto signOk;
		}
	}
	fprintf(stderr, "Invalid response\n");
	return 1;

signOk:
	for (i = 0; i < sizeof(jkRegs) / sizeof(jkRegs[0]); i++) {
		if (jkRegs[i].reg >= regBase && jkRegs[i].reg < regBase + 0x200) {
			if (jkRegs[i].reg - regBase > nRead) {
				fprintf(stderr, "Missing data\n");
				return 1;
			} 
			readRegVal(i);
			printf("%s: %s\n", jkRegs[i].name, regVal);
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int bmsAddr;

	if (argc < 2) {
		fprintf(stderr, "Usage: ./jkcmd SERIAL_PORT [BMS_ADDRESS default=1]\n");
		fprintf(stderr, "example: ./jkcmd /dev/ttyUSB0 2\n");
		return 1;
	}
	
	if (argc < 3) {
		bmsAddr = 1;
	} else {
		sscanf(argv[2], "%i", &bmsAddr);
		if (bmsAddr < 1 || bmsAddr > 15) {
			fprintf(stderr, "Wrong BMS address\n");
			return 1;
		}
	}
		
	if (openSerial(argv[1])) {
		return 1;
	}
	jkRead(bmsAddr, 1);
	jkRead(bmsAddr, 2);
	jkRead(bmsAddr, 3);
	return 0;
}
