#ifndef COMMANDREGISTER_H
#define COMMANDREGISTER_H

#include <cstdint>

enum RegisterEnum : uint32_t
{
    // AISC ADC registers
    WRITE_ADC_SAMPLE_PERIOD_CFG_REG_32BIT = 0x00000001,    // 写：ADC采样周期配置寄存器
    WRITE_ADC_SAMPLE_CHANNEL_CFG_REG_8X32BIT = 0x00000002, // 写：ADC每个周期内32个采样点采样通道号配置寄存器
    WRITE_ADC_ENABLE_REG_32BIT = 0x00000003,               // 写：ADC使能寄存器
    WRITE_ADC_OFFSETCAL_ENABLE_REG_32BIT = 0x00000004,     // 写：ADC offsetcal 使能寄存器
    WRITE_ADC_ACQ_TIME_CFG_REG_32BIT = 0x00000005,         // 写：ADC ACQ time 配置寄存器
    READ_ADC_SAMPLE_PERIOD_CFG_REG_32BIT = 0x80000001,     // 读：ADC采样周期状态寄存器
    READ_ADC_SAMPLE_CHANNEL_CFG_REG_8X32BIT = 0x80000002,  // 读：ADC每个周期内32个采样点采样通道号状态寄存器
    READ_ADC_ENABLE_REG_32BIT = 0x80000003,                // 读：ADC使能状态寄存器
    READ_ADC_ACQ_TIME_CFG_REG_32BIT = 0x80000005,          // 读：ADC ACQ time 状态寄存器

    // ASIC CONTROL registers
    WRITE_ASIC_CTRL_REG_32BIT = 0x01000001,                     // 写：ASIC控制寄存器, 设置工作模式
    WRITE_ASIC_POWER_CTRL_REG_32BIT = 0x01000002,               // 写：ASIC POWER 控制寄存器
    WRITE_ASIC_VCM_EN_CTRL_REG_32BIT = 0x01000003,              // 写：ASIC VCM_EN 控制寄存器
    WRITE_ASIC_FC_VCOM_EN_CTRL_REG_32BIT = 0x01000004,          // 写：ASIC FC_VCOM_EN 控制寄存器
    WRITE_ASIC_CHANNEL_STATE_CFG_REG_32X32BIT = 0x01000005,     // 写：ASIC 1024个通道开关状态配置寄存器
    WRITE_ASIC_CHANNEL_STATE_SINGLE_CFG_REG_32BIT = 0x01000006, // 写：ASIC 单个通道开关状态配置寄存器
    WRITE_ASIC_CHANNEL_UNBLOCK_CFG_REG_32BIT = 0x01000007,      // 写：ASIC unblock通道号配置寄存器
    READ_ASIC_STATUS_REG_32BIT = 0x81000001,                    // 读：ASIC状态寄存器

    // ASIC cooling control registers
    WRITE_HEATER_VOLTAGE_CFG_REG_32BIT = 0x02000001,   // 加热器电压配置寄存器, 写
    WRITE_ASIC_TEMP_SET_REG_32BIT = 0x03000001,        // ASIC 温度设定寄存器, 写
    READ_ASIC_TEMP_SENSOR_REG_32BIT = 0x83000001,      // ASIC 温度传感器返回值, 读
    READ_HEATER_VOLTAGE_STATUS_REG_32BIT = 0x82000001, // 加热器电压状态寄存器, 读
    // ASIC_VT control registers
    WRITE_ASIC_VT_SINGLE_OUTPUT_REG_32BIT =  0x04000001,   // ASIC_VT 单次输出寄存器, 写
    WRITE_ASIC_VT_CONT_OUTPUT_CFG_REG_32BIT =  0x04000002, // ASIC_VT 连续输出配置寄存器, 写
    WRITE_ASIC_VT_MODE_CONTROL_REG_32BIT =  0x04000003,    // ASIC_VT 模式控制寄存器, 写
    READ_ASIC_VT_CURRENT_VALUE_REG_32BIT =  0x84000001,    // ASIC_VT 当前值, 读

    // FC_VCOM control registers
    WRITE_FC_VCOM_FIXED_OUTPUT_REG_32BIT =  0x05000001,   // FC_VCOM 单次输出寄存器, 写
    WRITE_FC_VCOM_CONTINUOUS_OUTPUT_CFG_REG_32BIT =  0x05000002, // FC_VCOM 连续输出配置寄存器, 写
    WRITE_FC_VCOM_MODE_CONTROL_REG_32BIT =  0x05000003,    // FC_VCOM 模式控制寄存器, 写
    READ_FC_VCOM_CURRENT_VALUE_REG_32BIT =  0x85000001,    // FC_VCOM 当前值, 读

    // LED control registers
    READ_LED_CURRENT_VALUE_REG_32BIT = 0x84000001,  // LED 电流值寄存器 32BIT[]
    WRITE_LED_OUTPUT_STATUS_REG_32BIT = 0x06000001, // LED 输出状态, 写
    READ_LED_STATUS_REG_32BIT = 0x86000001         // LED 状态寄存器, 读
};

struct StateBitsRange {
    int low;
    int high;
};

// 读写：ADC采样周期状态寄存器
struct  ADC_SAMPLE_PERIOD_CFG {
    StateBitsRange samplePeriod;
    ADC_SAMPLE_PERIOD_CFG() {
        samplePeriod = {0, 31};
    }
};

//读写：ADC每个周期内32个采样点采样通道号状态寄存器
struct ADC_SAMPLE_CHANNEL_CFG {
    StateBitsRange ch[32];
    ADC_SAMPLE_CHANNEL_CFG() {
        for (int i = 0; i < 32; ++i) {
            ch[i] = { (32 - (i+1) ) * 8, (32 - i ) * 8 - 1};
        }
    }
};


// 读写：配置ADC ACQ 时间，最小80ns，配置单位为12.5ns.
struct ADC_ACQ_TIME_CFG {
    StateBitsRange adcAcqTime;
    ADC_ACQ_TIME_CFG() {
        adcAcqTime = {0, 31};
    }
};

// 读写：ADC使能状态寄存器
struct ADC_ENABLE {
    StateBitsRange adcEnable;
    ADC_ENABLE() {
        adcEnable = {0, 0};
    }
};

// 写： ADC offsetcal使能
struct ADC_OFFSETCAL_ENABLE  {
    StateBitsRange adcOffsetcalEnable;
    ADC_OFFSETCAL_ENABLE() {
        adcOffsetcalEnable = {0, 0};
    }
};

struct ASIC_CTRL {
    StateBitsRange mask;
    StateBitsRange ep;
    StateBitsRange lgp;
    StateBitsRange all;
    StateBitsRange unblock;
    ASIC_CTRL() {
        mask = {16, 21}; //mask bit 对应 bit[5：0] 设置
        ep = {5, 5};
        lgp = {3, 4};
        all = {1, 2};
        unblock =  {0, 0};
    }
};

// ASIC POWER 控制寄存器
struct ASIC_POWER_CTRL {
    StateBitsRange powerEnable;
    ASIC_POWER_CTRL() {
        powerEnable = {0, 0};
    }
};

// ASIC VCM_EN 控制寄存器
struct ASIC_VCM_EN {
    StateBitsRange vcmEnable;
    ASIC_VCM_EN() {
        vcmEnable = {0, 0};
    }
};

// ASIC FC_VCOM_EN 控制寄存器
struct ASIC_FC_VCOM_EN {
    StateBitsRange vcomEnable;
    ASIC_FC_VCOM_EN() {
        vcomEnable = {0, 0};
    }
};

// ASIC 1024 个通道开关状态配置寄存器
// 32 * 32bit, 最高bit代表最低通道开关状态
struct ASIC_CHANNEL_STATE {
    StateBitsRange ch[1024];
    ASIC_CHANNEL_STATE() {
        for (int i = 0; i < 1024; ++i) {
            ch[i] = { 1023 - i, 1023 -i};
        }
    }
};

// ASIC 单个通道开关状态配置寄存器
struct ASIC_CHANNEL_STATE_SINGLE {
    StateBitsRange ix;
    StateBitsRange iy;
    StateBitsRange state;
    ASIC_CHANNEL_STATE_SINGLE () {
        ix = {12, 16};
        iy = {4, 8};
        state = {0 ,0};
    }
};

// ASIC  unblock通道号配置寄存器
struct ASIC_CHANNEL_UNBLOCK {
    StateBitsRange ix;
    StateBitsRange iy;
    StateBitsRange unblock;
    ASIC_CHANNEL_UNBLOCK () {
        ix = {12, 16};
        iy = {4, 8};
        unblock = {0 ,0};
    }
};

// ASIC 状态寄存器
struct ASIC_STATUS {
    StateBitsRange adcOverflow;
    StateBitsRange asicRemoveDetected;
    StateBitsRange ep;
    StateBitsRange lgp;
    StateBitsRange ulgp;
    StateBitsRange aoffp;
    StateBitsRange aonp;
    StateBitsRange tep;
    StateBitsRange ASIC_PWR;
    StateBitsRange ASIC_VCM_EN;
    StateBitsRange FC_VCOM_EN;
    StateBitsRange ASIC_DET;
    StateBitsRange ASIC_LOGIC_READY;
    ASIC_STATUS() {
        adcOverflow = {13, 13};
        asicRemoveDetected = {12, 12};
        ep = {11, 11};
        lgp = {10, 10};
        ulgp = {9, 9};
        aoffp = {8, 8};
        aonp = {7, 7};
        tep = {6, 6};
        ASIC_PWR = {5, 5};
        ASIC_VCM_EN = {4, 4};
        FC_VCOM_EN = {3, 3};
        ASIC_DET = {2, 2};
        ASIC_LOGIC_READY = {1, 1};
    }

};

ADC_SAMPLE_PERIOD_CFG _AdcSamplePeriod;
ADC_SAMPLE_CHANNEL_CFG _AdcSampleChannel;
ADC_ENABLE _AdcEnable;
ADC_ACQ_TIME_CFG _AdcAcqTime;

ASIC_CTRL _AsicControl;
ASIC_POWER_CTRL _AsicPower;
ASIC_VCM_EN _AsicVcmEn;
ASIC_FC_VCOM_EN _asicVcomEn;
ASIC_CHANNEL_STATE _asicChannelState;
ASIC_CHANNEL_STATE_SINGLE _asicChannelStateSingle;
ASIC_CHANNEL_UNBLOCK _asicChannelUnblcok;
ASIC_STATUS _asicStatus;








#endif // COMMANDREGISTER_H
