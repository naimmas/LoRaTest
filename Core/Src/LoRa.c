//
// Created by ajanx on 15.05.2022.
//

#include "LoRa.h"
struct LoRaInitConfig initConfig;

LoRaTypedef_STATUS LoRaE22_Init(UART_HandleTypeDef *huart,
	GPIO_TypeDef *AuxPort, GPIO_TypeDef *M0Port, GPIO_TypeDef *M1Port,
	uint16_t AuxPin, uint16_t M0Pin, uint16_t M1Pin)
    {
    initConfig.AUX_Port = AuxPort;
    initConfig.AUX_Pin = AuxPin;
    initConfig.M0_Port = M0Port;
    initConfig.M0_Pin = M0Pin;
    initConfig.M1_Port = M1Port;
    initConfig.M1_Pin = M1Pin;

    }

LoRaTypedef_STATUS LoRaE22_SetMode(LoRaTypedef_MODES mode)
    {
    managedDelay(10);

    switch (mode)
	{
    case E22_NORMAL_MODE:
	// Mode 0 | normal operation
	HAL_GPIO_WritePin(initConfig.M0_Port, initConfig.M0_Pin, 0);
	HAL_GPIO_WritePin(initConfig.M1_Port, initConfig.M1_Pin, 0);
	break;
    case E22_WOR_MODE:
	HAL_GPIO_WritePin(initConfig.M0_Port, initConfig.M0_Pin, 1);
	HAL_GPIO_WritePin(initConfig.M1_Port, initConfig.M1_Pin, 0);
	break;
    case E22_CONFIG_MODE:
	//		  case MODE_2_PROGRAM:
	HAL_GPIO_WritePin(initConfig.M0_Port, initConfig.M0_Pin, 0);
	HAL_GPIO_WritePin(initConfig.M1_Port, initConfig.M1_Pin, 1);
	break;
    case E22_SLEEP_MODE:
	// Mode 3 | Setting operation
	HAL_GPIO_WritePin(initConfig.M0_Port, initConfig.M0_Pin, 1);
	HAL_GPIO_WritePin(initConfig.M1_Port, initConfig.M1_Pin, 1);
	break;

    default:
	return ERR_E22_INVALID_PARAM;
	}

    // data sheet says 2ms later control is returned, let's give just a bit more time
    // these modules can take time to activate pins
    managedDelay(3);

    // wait until aux pin goes back low
    LoRaTypedef_STATUS res = waitCompleteResponse(1000);

    return res;
    }

LoRaTypedef_STATUS LoRaE22_waitCompleteResponse(unsigned long timeout)
    {

    LoRaTypedef_STATUS result = E22_SUCCESS;

    unsigned long t = HAL_GetTick();

    if (((unsigned long) (t + timeout)) == 0)
	{
	t = 0;
	}
    while (HAL_GPIO_ReadPin(initConfig.AUX_Port, initConfig.AUX_Pin)
	    == GPIO_PIN_RESET)
	{
	if ((HAL_GetTick() - t) > timeout)
	    {
	    result = ERR_E22_TIMEOUT;

	    return result;
	    }
	}

    managedDelay(5);
    return result;
    }

void managedDelay(unsigned long timeout)
    {
    unsigned long t = HAL_GetTick();
    if (((unsigned long) (t + timeout)) == 0)
	{
	t = 0;
	}
    while ((HAL_GetTick() - t) < timeout)
	;
    }
