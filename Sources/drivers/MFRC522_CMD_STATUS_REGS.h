#ifndef MFRC522_STATUS_REGS_H
#define MFRC522_STATUS_REGS_H
#include <stdint.h>

#define MFRC522_CMDReg_ADDRESS          0x01
#define MFRC522_ComIEnReg_ADDRESS       0x02
#define MFRC522_DivIEnReg_ADDRESS       0x03
#define MFRC522_ComIrqReg_ADDRESS       0x04
#define MFRC522_DivIrqReg_ADDRESS       0x05
#define MFRC522_ErrorReg_ADDRESS        0x06
#define MFRC522_Status1Reg_ADDRESS      0x07
#define MFRC522_Status2Reg_ADDRESS      0x08
#define MFRC522_FIFODataReg_ADDRESS     0x09
#define MFRC522_FIFOLevelReg_ADDRESS    0x0A
#define MFRC522_WaterLevelReg_ADDRESS   0x0B
#define MFRC522_ControlReg_ADDRESS      0x0C
#define MFRC522_BitFramingReg_ADDRESS   0x0D
#define MFRC522_CollReg_ADDRESS         0x0E

typedef union {
    struct {
        uint8_t cmd        : 4;
        uint8_t powerDown  : 1;
        uint8_t rcvOff     : 1;
        uint8_t reserved   : 2;
    } fields;
    uint8_t cmdByte;
} MFRC522_CmdReg;

typedef union {
    struct {
        uint8_t timerlEn    : 1;
        uint8_t errlEn      : 1;
        uint8_t loAlertlEn  : 1;
        uint8_t hiAlertlEn  : 1;
        uint8_t ldlelEn     : 1;
        uint8_t rxlEn       : 1;
        uint8_t txlEn       : 1;
        uint8_t lRqlnv      : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_ComIEnReg;

typedef union {
    struct {
        uint8_t reserved    : 2;
        uint8_t CRClEn      : 1;
        uint8_t reserved2   : 1;
        uint8_t MfinActIEn  : 1;
        uint8_t reserved3   : 2;
        uint8_t IRQPushPull : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_DivIEnReg;

typedef union {
    struct {
        uint8_t TimerIRq    : 1;
        uint8_t ErrIRq      : 1;
        uint8_t LoAlertIRq  : 1;
        uint8_t HiAlertIRq  : 1;
        uint8_t IdleIRq     : 1;
        uint8_t RxIRq       : 1;
        uint8_t TxIRq       : 1;
        uint8_t Set1        : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_ComIrqReg;

typedef union {
    struct {
        uint8_t reserved    : 2;
        uint8_t CRCIRq      : 1;
        uint8_t reserved1   : 1;
        uint8_t MfinActIRq  : 1;
        uint8_t reserved2   : 2;
        uint8_t Set2        : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_DivIrqReg;

typedef union {
    struct {
        uint8_t ProtocolErr : 1;
        uint8_t ParityErr   : 1;
        uint8_t CRCErr      : 1;
        uint8_t CollErr     : 1;
        uint8_t BufferOvfl  : 1;
        uint8_t reserved    : 1;
        uint8_t TempErr     : 1;
        uint8_t WrErr       : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_ErrorReg;

typedef union {
    struct {
        uint8_t LoAlert     : 1;
        uint8_t HiAlert     : 1;
        uint8_t reserved    : 1;
        uint8_t TRunning    : 1;
        uint8_t IRq         : 1;
        uint8_t CRCReady    : 1;
        uint8_t CRCOk       : 1;
        uint8_t reserved2   : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_Status1Reg;

typedef union {
    struct {
        uint8_t ModemState  : 3;
        uint8_t MFCrypto1On : 1;
        uint8_t reserved    : 2;
        uint8_t I2CForceHS  : 1;
        uint8_t TempSensClear : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_Status2Reg;

typedef union {
    struct {
        uint8_t FIFOData    : 8;
    } fields;
    uint8_t cmdByte;
} MFRC522_FIFODataReg;

typedef union {
    struct {
        uint8_t FIFOLevel   : 7;
        uint8_t FlushBuffer : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_FIFOLevelReg;

typedef union {
    struct {
        uint8_t WaterLevel  : 6;
        uint8_t reserved    : 2;
    } fields;
    uint8_t cmdByte;
} MFRC522_WaterLevelReg;

typedef union {
    struct {
        uint8_t RxLastBits  : 3;
        uint8_t reserved    : 3;
        uint8_t TStartNow   : 1;
        uint8_t TStopNow    : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_ControlReg;

typedef union {
    struct {
        uint8_t TxLastBits  : 3;
        uint8_t reserved    : 1;
        uint8_t RxAlign     : 3;
        uint8_t StartSend   : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_BitFramingReg;

typedef union {
    struct {
        uint8_t CollPos         : 5;
        uint8_t CollPosNotValid : 1;
        uint8_t reserved        : 1;
        uint8_t ValuesAfterColl : 1;
    } fields;
    uint8_t cmdByte;
} MFRC522_CollReg;


typedef struct {
    MFRC522_CmdReg cmdReg;
    MFRC522_ComIEnReg comIEnReg;
    MFRC522_DivIEnReg divIEnReg;
    MFRC522_ComIrqReg comIrqReg;
    MFRC522_DivIrqReg divIrqReg;
    MFRC522_ErrorReg errorReg;
    MFRC522_Status1Reg status1Reg;
    MFRC522_Status2Reg status2Reg;
    MFRC522_FIFODataReg fifoDataReg;
    MFRC522_FIFOLevelReg fifoLevelReg;
    MFRC522_WaterLevelReg waterLevelReg;
    MFRC522_ControlReg controlReg;
    MFRC522_BitFramingReg bitFramingReg;
    MFRC522_CollReg collReg;
} MFRC522_StatusRegs;

#endif