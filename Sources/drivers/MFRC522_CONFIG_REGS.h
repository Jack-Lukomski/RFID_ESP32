#ifndef MFRC522_CONFIG_REGS_H
#define MFRC522_CONFIG_REGS
#include <stdint.h>

// Configuration Registers
#define MFRC522_CRCResultReg1_ADDRESS       0x21
#define MFRC522_CRCResultReg2_ADDRESS       0x22
#define MFRC522_ModWidthReg_ADDRESS         0x24
#define MFRC522_TxASKReloadReg_ADDRESS      0x25
#define MFRC522_RFCfgReg_ADDRESS            0x26
#define MFRC522_GsNReg_ADDRESS              0x27
#define MFRC522_CWGsCfgReg_ADDRESS          0x28
#define MFRC522_ModGsCfgReg_ADDRESS         0x2A
#define MFRC522_TModeReg_ADDRESS            0x2B
#define MFRC522_TPrescalerReg_ADDRESS       0x2C
#define MFRC522_TReloadRegH_ADDRESS         0x2C
#define MFRC522_TReloadRegL_ADDRESS         0x2D
#define MFRC522_TCounterValueRegH_ADDRESS   0x2E
#define MFRC522_TCounterValueRegL_ADDRESS   0x2F

typedef union {
    struct {
        uint8_t CRCResultMSB   : 8;
    } fields;
    uint8_t regByte;
} MFRC522_CRCResultRegMSB;

typedef union {
    struct {
        uint8_t CRCResultLSB   : 8;
    } fields;
    uint8_t regByte;
} MFRC522_CRCResultRegLSB;

typedef union {
    struct {
        uint8_t ModWidth : 8;
    } fields;
    uint8_t regByte;
} MFRC522_ModWidthReg;

typedef union {
    struct {
        uint8_t reserved     : 4;
        uint8_t RxGain     : 3;
        uint8_t reserved1    : 1;
    } fields;
    uint8_t regByte;
} MFRC522_RFCfgReg;

typedef union {
    struct {
        uint8_t ModGsN : 4;
        uint8_t CWGsN : 4;
    } fields;
    uint8_t regByte;
} MFRC522_GsNReg;

typedef union {
    struct {
        uint8_t CWGsP : 6;
        uint8_t reserved : 2;
    } fields;
    uint8_t regByte;
} MFRC522_CWGsPReg;

typedef union {
    struct {
        uint8_t TPrescaler_Hi : 4;
        uint8_t TAutoRestart : 1;
        uint8_t TGated : 2;
        uint8_t TAuto : 1;
    } fields;
    uint8_t regByte;
} MFRC522_TModeReg;

typedef union {
    struct {
        uint8_t TPrescaler_Lo : 8;
    } fields;
    uint8_t regByte;
} MFRC522_TPrescalerReg;

typedef union {
    struct {
        uint8_t TReloadVal_Hi : 8;
    } fields;
    uint8_t regByte;
} MFRC522_TReloadRegH;

typedef union {
    struct {
        uint8_t TReloadVal_Lo : 8;
    } fields;
    uint8_t regByte;
} MFRC522_TReloadRegL;

#endif