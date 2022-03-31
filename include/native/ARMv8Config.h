#pragma once
#include <map>

#define ARM_v8_FLAG_FP 0X00
#define ARM_v8_FLAG_ASIMD 0X01
#define ARM_v8_FLAG_EVTSTRM 0X02
#define ARM_v8_FLAG_AES 0X03
#define ARM_v8_FLAG_PMULL 0X04
#define ARM_v8_FLAG_SHA1 0X05
#define ARM_v8_FLAG_SHA2 0X06
#define ARM_v8_FLAG_CRC32 0X07

#define ARM_v8_FLAG_ATOMICS 0X08
#define ARM_v8_FLAG_FPHP 0X09
#define ARM_v8_FLAG_ASIMDHP 0X0A
#define ARM_v8_FLAG_CPUID 0X0B
#define ARM_v8_FLAG_ASIMDRDM 0X0C
#define ARM_v8_FLAG_JSCVT 0X0D
#define ARM_v8_FLAG_FCMA 0X0E
#define ARM_v8_FLAG_LRCPC 0X0F

#define ARM_v8_FLAG_DCPOP 0X10
#define ARM_v8_FLAG_SHA3 0X11
#define ARM_v8_FLAG_SM3 0X12
#define ARM_v8_FLAG_SM4 0X13
#define ARM_v8_FLAG_ASIMDDP 0X14
#define ARM_v8_FLAG_SHA512 0X15
#define ARM_v8_FLAG_SVE 0X16
#define ARM_v8_FLAG_ASIMDFHM 0X17

#define ARM_v8_FLAG_DIT 0X18
#define ARM_v8_FLAG_USCAT 0X19
#define ARM_v8_FLAG_ILRCPC 0X1A
#define ARM_v8_FLAG_FLAGM 0X1B
#define ARM_v8_FLAG_SSBS 0X1C
#define ARM_v8_FLAG_SB 0X1D
#define ARM_v8_FLAG_PACA 0X1E
#define ARM_v8_FLAG_PACG 0X1F

static std::map<uint8_t, const char *> ARMv8VendorNameMap{/* NOLINT */
                                                          {0x41, "ARM"},
                                                          {0x42, "Broadcom"},
                                                          {0x43, "Cavium"},
                                                          {0x44, "DigitalEquipment"},
                                                          {0x48, "HiSilicon"},
                                                          {0x49, "Infineon"},
                                                          {0x4D, "Freescale"},
                                                          {0x4E, "NVIDIA"},
                                                          {0x50, "APM"},
                                                          {0x51, "Qualcomm"},
                                                          {0x65, "Marvell"},
                                                          {0x69, "Intel"}};