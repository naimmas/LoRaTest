//
// Created by ajanx on 15.05.2022.
//
#include "stdint.h"
#include "string.h"
#ifndef LORA_STATES_H
#define LORA_STATES_H

typedef enum RESPONSE_STATUS {
    E22_SUCCESS = 1,
    ERR_E22_UNKNOWN,	/* something shouldn't happen */
    ERR_E22_NOT_SUPPORT,
    ERR_E22_NOT_IMPLEMENT,
    ERR_E22_NOT_INITIAL,
    ERR_E22_INVALID_PARAM,
    ERR_E22_DATA_SIZE_NOT_MATCH,
    ERR_E22_BUF_TOO_SMALL,
    ERR_E22_TIMEOUT,
    ERR_E22_HARDWARE,
    ERR_E22_HEAD_NOT_RECOGNIZED,
    ERR_E22_NO_RESPONSE_FROM_DEVICE,
    ERR_E22_WRONG_UART_CONFIG,
    ERR_E22_WRONG_FORMAT,
    ERR_E22_PACKET_TOO_BIG
} LoRaTypedef_STATUS;

enum LoRaTypedef_UART_PARITY
{
    MODE_00_8N1 = 0b00,
    MODE_01_8O1 = 0b01,
    MODE_10_8E1 = 0b10,
    MODE_11_8N1 = 0b11
};

enum LoRaTypedef_UART_BPS_TYPE
{
    UART_BPS_1200 = 0b000,
    UART_BPS_2400 = 0b001,
    UART_BPS_4800 = 0b010,
    UART_BPS_9600 = 0b011,
    UART_BPS_19200 = 0b100,
    UART_BPS_38400 = 0b101,
    UART_BPS_57600 = 0b110,
    UART_BPS_115200 = 0b111
};

enum LoRaTypedef_UART_BPS_RATE
{
    UART_BPS_RATE_1200 = 1200,
    UART_BPS_RATE_2400 = 2400,
    UART_BPS_RATE_4800 = 4800,
    UART_BPS_RATE_9600 = 9600,
    UART_BPS_RATE_19200 = 19200,
    UART_BPS_RATE_38400 = 38400,
    UART_BPS_RATE_57600 = 57600,
    UART_BPS_RATE_115200 = 115200
};

enum LoRaTypedef_AIR_DATA_RATE
{
    AIR_DATA_RATE_000_03 = 0b000,
    AIR_DATA_RATE_001_12 = 0b001,
    AIR_DATA_RATE_010_24 = 0b010,
    AIR_DATA_RATE_011_48 = 0b011,
    AIR_DATA_RATE_100_96 = 0b100,
    AIR_DATA_RATE_101_192 = 0b101,
    AIR_DATA_RATE_110_384 = 0b110,
    AIR_DATA_RATE_111_625 = 0b111
};

enum LoRaTypedef_SUB_PACKET_SETTING {
    SPS_240_00 = 0b00,
    SPS_128_01 = 0b01,
    SPS_064_10 = 0b10,
    SPS_032_11 = 0b11

};

enum LoRaTypedef_RSSI_AMBIENT_NOISE_ENABLE {
    RSSI_AMBIENT_NOISE_ENABLED = 0b1,
    RSSI_AMBIENT_NOISE_DISABLED = 0b0
};

enum LoRaTypedef_WOR_TRANSCEIVER_CONTROL {
    WOR_TRANSMITTER = 0b1,
    WOR_RECEIVER = 0b0
};

enum LoRaTypedef_LBT_ENABLE_BYTE {
    LBT_ENABLED = 0b1,
    LBT_DISABLED = 0b0
};

enum LoRaTypedef_REPEATER_MODE_ENABLE_BYTE {
    REPEATER_ENABLED = 0b1,
    REPEATER_DISABLED = 0b0
};

enum LoRaTypedef_RSSI_ENABLE_BYTE {
    RSSI_ENABLED = 0b1,
    RSSI_DISABLED = 0b0
};

enum LoRaTypedef_FIXED_TRANSMISSION
{
    FT_TRANSPARENT_TRANSMISSION = 0b0,
    FT_FIXED_TRANSMISSION = 0b1
};

enum LoRaTypedef_TRANSMISSION_POWER
{
    POWER_30 = 0b00,
    POWER_27 = 0b01,
    POWER_24 = 0b10,
    POWER_21 = 0b11

};

enum LoRaTypedef_WOR_PERIOD {
	WOR_500_000 = 0b000,
	WOR_1000_001 = 0b001,
	WOR_1500_010 = 0b010,
	WOR_2000_011 = 0b011,
	WOR_2500_100 = 0b100,
	WOR_3000_101 = 0b101,
	WOR_3500_110 = 0b110,
	WOR_4000_111 = 0b111

};
/*
static int getResponseDescriptionByParams(uint8_t status){
    switch (status)
    {
        case E22_SUCCESS:
            return F("Success");
            
        case ERR_E22_UNKNOWN:
            return F("Unknown");
            
        case ERR_E22_NOT_SUPPORT:
            return F("Not support!");
            
        case ERR_E22_NOT_IMPLEMENT:
            return F("Not implement");
            
        case ERR_E22_NOT_INITIAL:
            return F("Not initial!");
            
        case ERR_E22_INVALID_PARAM:
            return F("Invalid param!");
            
        case ERR_E22_DATA_SIZE_NOT_MATCH:
            return F("Data size not match!");
            
        case ERR_E22_BUF_TOO_SMALL:
            return F("Buff too small!");
            
        case ERR_E22_TIMEOUT:
            return F("Timeout!!");
            
        case ERR_E22_HARDWARE:
            return F("Hardware error!");
            
        case ERR_E22_HEAD_NOT_RECOGNIZED:
            return F("Save mode returned not recognized!");
            
        case ERR_E22_NO_RESPONSE_FROM_DEVICE:
            return F("No response from device! (Check wiring)");
            
        case ERR_E22_WRONG_UART_CONFIG:
            return F("Wrong UART configuration! (BPS must be 9600 for configuration)");
            
        case ERR_E22_PACKET_TOO_BIG:
            return F("The device support only 240byte of data transmission!");
            
        default:
            return F("Invalid status!");
    }
}

static int getUARTParityDescriptionByParams(uint8_t uartParity){
    switch (uartParity)
    {
        case MODE_00_8N1:
            return F("8N1 (Default)");
        case MODE_01_8O1:
            return F("8O1");
        case MODE_10_8E1:
            return F("8E1");
        case MODE_11_8N1:
            return F("8N1");
        default:
            return F("Invalid UART Parity!");
    }
}

static int getUARTBaudRateDescriptionByParams(uint8_t uartBaudRate)
{
    switch (uartBaudRate)
    {
        case UART_BPS_1200:
            return F("1200bps");
            
        case UART_BPS_2400:
            return F("2400bps");
            
        case UART_BPS_4800:
            return F("4800bps");
            
        case UART_BPS_9600:
            return F("9600bps (default)");
            
        case UART_BPS_19200:
            return F("19200bps");
            
        case UART_BPS_38400:
            return F("38400bps");
            
        case UART_BPS_57600:
            return F("57600bps");
            
        case UART_BPS_115200:
            return F("115200bps");
            
        default:
            return F("Invalid UART Baud Rate!");
    }
}

static int getAirDataRateDescriptionByParams(uint8_t airDataRate)
{
    switch (airDataRate)
    {
        case AIR_DATA_RATE_000_03:
            return F("0.3kbps");
            
        case AIR_DATA_RATE_001_12:
            return F("1.2kbps");
            
        case AIR_DATA_RATE_010_24:
            return F("2.4kbps (default)");
            
        case AIR_DATA_RATE_011_48:
            return F("4.8kbps");
            
        case AIR_DATA_RATE_100_96:
            return F("9.6kbps");
            
        case AIR_DATA_RATE_101_192:
            return F("19.2kbps");
            
        case AIR_DATA_RATE_110_384:
            return F("38.4kbps");
            
        case AIR_DATA_RATE_111_625:
            return F("62.5kbps");
            
        default:
            return F("Invalid Air Data Rate!");
    }
}

static int getSubPacketSettingByParams(uint8_t subPacketSetting)
{
    switch (subPacketSetting)
    {
        case SPS_240_00:
            return F("240bytes (default)");
            
        case SPS_128_01:
            return F("128bytes");
            
        case SPS_064_10:
            return F("64bytes");
        case SPS_032_11:
            return F("32bytes");
        default:
            return F("Invalid Sub Packet Setting!");
    }
}

static int getRSSIAmbientNoiseEnableByParams(uint8_t rssiAmbientNoiseEnabled)
{
    switch (rssiAmbientNoiseEnabled)
    {
        case RSSI_AMBIENT_NOISE_ENABLED:
            return F("Enabled");
            
        case RSSI_AMBIENT_NOISE_DISABLED:
            return F("Disabled (default)");
            
        default:
            return F("Invalid RSSI Ambient Noise enabled!");
    }
}

static int getWORTransceiverControlByParams(uint8_t worTransceiverControl)
{
    switch (worTransceiverControl)
    {
        case WOR_TRANSMITTER:
            return F("WOR Transmitter");
            
        case WOR_RECEIVER:
            return F("WOR Receiver (default)");
            
        default:
            return F("Invalid WOR transceiver control!");
    }
}

static int getLBTEnableByteByParams(uint8_t LBTEnableByte)
{
    switch (LBTEnableByte)
    {
        case LBT_ENABLED:
            return F("Enabled");
            
        case LBT_DISABLED:
            return F("Disabled (default)");
            
        default:
            return F("Invalid LBT enable byte!");
    }
}

static int getRepeaterModeEnableByteByParams(uint8_t repeaterEnableByte)
{
    switch (repeaterEnableByte)
    {
        case REPEATER_ENABLED:
            return F("Enabled");
            
        case REPEATER_DISABLED:
            return F("Disabled (default)");
            
        default:
            return F("Invalid repeater enable byte!");
    }
}

static int getRSSIEnableByteByParams(uint8_t RSSIEnableByte)
{
    switch (RSSIEnableByte)
    {
        case RSSI_ENABLED:
            return F("Enabled");
            
        case RSSI_DISABLED:
            return F("Disabled (default)");
            
        default:
            return F("Invalid RSSI enable byte!");
    }
}

static int getFixedTransmissionDescriptionByParams(uint8_t fixedTransmission)
{
    switch (fixedTransmission)
    {
        case FT_TRANSPARENT_TRANSMISSION:
            return F("Transparent transmission (default)");
            
        case FT_FIXED_TRANSMISSION:
            return F("Fixed transmission (first three bytes can be used as high/low address and channel)");
            
        default:
            return F("Invalid fixed transmission param!");
    }
}

static int getTransmissionPowerDescriptionByParams(uint8_t transmissionPower)
{
    switch (transmissionPower)
    {
        case POWER_30:
            return F("30dBm (Default)");
            
        case POWER_27:
            return F("27dBm");
            
        case POWER_24:
            return F("24dBm");
            
        case POWER_21:
            return F("21dBm");
            
        default:
            return F("Invalid transmission power param");
    }
}

static int getWORPeriodByParams(uint8_t WORPeriod)
{
    switch (WORPeriod)
    {
        case WOR_500_000:
            return F("500ms");

        case WOR_1000_001:
            return F("1000ms");

        case WOR_1500_010:
            return F("1500ms");

        case WOR_2000_011:
            return F("2000ms (default)");

        case WOR_2500_100:
            return F("2500ms");

        case WOR_3000_101:
            return F("3000ms");

        case WOR_3500_110:
            return F("3500ms");

        case WOR_4000_111:
            return F("4000ms");

        default:
            return F("Invalid WOR period!");
    }
}
*/

#endif //LORA_STATES_H
