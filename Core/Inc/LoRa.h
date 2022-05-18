#include "States.h"
#include "stdint.h"
#include "string.h"
#include "stm32h743xx.h"
#include "stm32h7xx_hal.h"

#define MAX_SIZE_TX_PACKET 240

typedef enum
{
	E22_NORMAL_MODE = 0x01,
	E22_WOR_MODE = 0x02,
	E22_CONFIG_MODE = 0x03,
	E22_SLEEP_MODE = 0x04,
	E22_MODE_INIT = 0xFF
}LoRaTypedef_MODES;

typedef enum
{
	E22_WRITE_CFG_PWR_DWN_SAVE = 0xC0,
	E22_READ_CONFIGURATION = 0xC1,
	E22_WRITE_CFG_PWR_DWN_LOSE = 0xC2,
	E22_WRONG_FORMAT = 0xFF,
	E22_RETURNED_COMMAND = 0xC1,
	E22_SPECIAL_WIFI_CONF_COMMAND = 0xCF
}LoRaTypedef_PROG_CMD;

enum LoRaTypedef_REG_ADDRS
{
	E22_REG_ADDRESS_CFG = 0x00,
	E22_REG_ADDRESS_SERIAL = 0x03,
	E22_REG_ADDRESS_TRANS_MODE = 0x04,
	E22_REG_ADDRESS_CHANNEL = 0x05,
	E22_REG_ADDRESS_OPTION = 0x06,
	E22_REG_ADDRESS_CRYPT = 0x07,
	E22_REG_ADDRESS_PID = 0x80
};

enum LoRaTypedef_PACKET_LENGHT
{
	E22_PL_CONFIGURATION = 0x09,
	E22_PL_SERIAL = 0x01,
	E22_PL_OPTION = 0x01,
	E22_PL_TRANSMISSION_MODE = 0x01,
	E22_PL_CHANNEL = 0x01,
	E22_PL_CRYPT = 0x02,
	E22_PL_PID = 0x07
};

struct LoRaInitConfig
{
	GPIO_TypeDef* AUX_Port;
	GPIO_TypeDef* M0_Port;
	GPIO_TypeDef* M1_Port;
	uint16_t AUX_Pin, M0_Pin, M1_Pin;
} ;

struct _LoRaUartConfig
{
	uint8_t uartParity :2; //bit 3-4
	uint8_t uartBaudRate :3; //bit 5-7
};

struct _LoRaTransmissionMode
{
	uint8_t WORPeriod :3; //bit 2,1,0
	uint8_t WORTransceiverControl :1; //bit 3
	uint8_t enableLBT :1; //bit 4
	uint8_t enableRepeater :1; //bit 5
	uint8_t fixedTransmission :1; //bit 6
	uint8_t enableRSSI :1; //bit 7
};

struct _LoRaOption
{
	uint8_t airDataRate :3; //bit 0-2

	uint8_t transmissionPower :2; //bit 0-1

	uint8_t reserve :3; //bit 2-3
	uint8_t RSSIAmbientNoise :1; //bit 5

	uint8_t subPacketSetting :2; //bit 6-7

};

struct _LoRaCrypt
{
	uint8_t CRYPT_H;
	uint8_t CRYPT_L;
};

struct _LoRaResponseStatus
{
	LoRaTypedef_STATUS code;
};

typedef struct
{
	uint8_t COMMAND;
	uint8_t STARTING_ADDRESS;
	uint8_t LENGTH;
	uint8_t ADDH;
	uint8_t ADDL;
	uint8_t NETID;
	struct _LoRaUartConfig UART;
	struct _LoRaOption OPTION;
	uint8_t CHAN;
	struct _LoRaTransmissionMode TRANSMISSION_MODE;
	struct _LoRaCrypt CRYPT;
} LoRaConfiguration;

typedef struct
{
	uint8_t COMMAND;
	uint8_t STARTING_ADDRESS;
	uint8_t LENGTH;
	uint8_t reserved[3];
	uint8_t model;
	uint8_t version;
	uint8_t features;
	uint8_t end;
} LoRaModuleInformation;

typedef struct
{
	uint8_t data;
	uint8_t rssi;
	struct _LoRaResponseStatus status;
} LoRaResponseContainer;

