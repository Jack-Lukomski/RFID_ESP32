#ifndef MRFC552_COM_REGS_H
#define MRFC552_COM_REGS_H
#include <stdint.h>

// Communication Registers
#define MFRC522_ModeReg_ADDRESS         0x11
#define MFRC522_TxModeReg_ADDRESS       0x12
#define MFRC522_RxModeReg_ADDRESS       0x13
#define MFRC522_TxControlReg_ADDRESS    0x14
#define MFRC522_TxASKReg_ADDRESS        0x15
#define MFRC522_TxSelReg_ADDRESS        0x16
#define MFRC522_RxSelReg_ADDRESS        0x17
#define MFRC522_RxThresholdReg_ADDRESS  0x18
#define MFRC522_DemodReg_ADDRESS        0x19
#define MFRC522_MfTxReg_ADDRESS         0x1C
#define MFRC522_MfRxReg_ADDRESS         0x1D
#define MFRC522_SerialSpeedReg_ADDRESS  0x1F

typedef union {
    struct {
        uint8_t CRCPreset : 2;
        uint8_t reserved : 1;
        uint8_t PolMFin : 1;
        uint8_t reserved1 : 1;
        uint8_t TxWaitRF : 1;
        uint8_t reserved2 : 1;
        uint8_t MSBFirst : 1;
    } fields;
    uint8_t cmdByte;
} MRFC552_ModeReg;

typedef union {
    struct {
        uint8_t reserved : 3;
        uint8_t InvMod : 1;
        uint8_t TxSpeed : 3;
        uint8_t TxCRCEn : 1;
    } fields;
    uint8_t cmdByte;
} MRFC552_TxModeReg;

typedef union {
    struct {
        uint8_t reserved : 2;
        uint8_t RxMultiple : 1;
        uint8_t RxNoErr : 1;
        uint8_t RxSpeed : 3;
        uint8_t RxCRCEn : 1;
    } fields;
    uint8_t cmdByte;
} MRFC552_RxModeReg;

typedef union {
    struct {
        uint8_t Tx1RFEn : 1;
        uint8_t Tx2RFEn : 1;
        uint8_t reserved : 1;
        uint8_t Tx2CW : 1;
        uint8_t InvTx1RFOff : 1;
        uint8_t InvTx2RFOff : 1;
        uint8_t InvTx1RFOn : 1;
        uint8_t InvTx2RFOn : 1;
    } fields;
    uint8_t cmdByte;
} MRFC552_TxControlReg;

typedef union {
    struct {
        uint8_t reserved : 6;
        uint8_t Force100ASK : 1;
        uint8_t reserved2 : 1;
    } fields;
    uint8_t regByte;
} MFRC522_TxASKReg;

typedef union {
    struct {
        uint8_t MFOutSel : 4;
        uint8_t DriverSel : 2;
        uint8_t reserved : 2;
    } fields;
    uint8_t regByte;
} MFRC522_TxSelReg;

typedef union {
    struct {
        uint8_t RxWait : 6;
        uint8_t UARTSel : 2;
    } fields;
    uint8_t regByte;
} MFRC522_RxSelReg;

typedef union {
    struct {
        uint8_t CollLevel : 3;
        uint8_t reserved : 1;
        uint8_t MinLevel : 4;
    } fields;
    uint8_t regByte;
} MFRC522_RxThresholdReg;

typedef union {
    struct {
        uint8_t TauSync : 2;
        uint8_t TauRcv : 2;
        uint8_t TPrescalEven : 1;
        uint8_t FixIQ : 1;
        uint8_t AddIQ : 2;
    } fields;
    uint8_t regByte;
} MFRC522_DemodReg;

typedef union {
    struct {
        uint8_t TxWait       : 2;
        uint8_t reserved     : 6;
    } fields;
    uint8_t regByte;
} MFRC522_MfTxReg;

typedef union {
    struct {
        uint8_t reserved     : 4;
        uint8_t ParityDisable : 1;
        uint8_t reserved1 : 3;
    } fields;
    uint8_t regByte;
} MFRC522_MfRxReg;

typedef union {
    struct {
        uint8_t BR_T1     : 5;
        uint8_t BR_T0     : 3;
    } fields;
    uint8_t regByte;
} MFRC522_SerialSpeedReg;

#endif