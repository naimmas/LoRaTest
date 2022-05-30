//
// Created by ajanx on 15.05.2022.
//

#include <LoRa.h>
#include <stdlib.h>
#include <stm32h7xx_hal_def.h>
#include <stm32h7xx_hal_gpio.h>
#include <stm32h7xx_hal_uart.h>
#include <sys/_stdint.h>

struct LoRaInitConfig initConfig;
LoRaTypedef_MODES _mode = E22_NORMAL_MODE;

void close(void *mem)
{
  free(mem);
}

void LoRaE22_Init(
    UART_HandleTypeDef *huart,
    GPIO_TypeDef *AuxPort,
    GPIO_TypeDef *M0Port,
    GPIO_TypeDef *M1Port,
    uint16_t AuxPin,
    uint16_t M0Pin,
    uint16_t M1Pin)
{
  initConfig.uartDevice = huart;
  initConfig.AUX_Port = AuxPort;
  initConfig.AUX_Pin = AuxPin;
  initConfig.M0_Port = M0Port;
  initConfig.M0_Pin = M0Pin;
  initConfig.M1_Port = M1Port;
  initConfig.M1_Pin = M1Pin;
  LoRaE22_SetMode(E22_NORMAL_MODE);
}

LoRaTypedef_STATUS LoRaE22_SetMode(LoRaTypedef_MODES mode)
{
  _managedDelay(10);

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
  _managedDelay(3);

  // wait until aux pin goes back low
  LoRaTypedef_STATUS res = LoRaE22_waitCompleteResponse(1000);

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
  while (HAL_GPIO_ReadPin(initConfig.AUX_Port, initConfig.AUX_Pin) == GPIO_PIN_RESET)
  {
    if ((HAL_GetTick() - t) > timeout)
    {
      result = ERR_E22_TIMEOUT;

      return result;
    }
  }

  _managedDelay(5);
  return result;
}

void _managedDelay(unsigned long timeout)
{
  unsigned long t = HAL_GetTick();
  if (((unsigned long) (t + timeout)) == 0)
  {
    t = 0;
  }
  while ((HAL_GetTick() - t) < timeout);
}

LoRaTypedef_STATUS LoRaE22_sendStruct(uint8_t *structureM, uint16_t size)
{
  if (size > MAX_SIZE_TX_PACKET + 2)
  {
    return ERR_E22_PACKET_TOO_BIG;
  }
  if (HAL_UART_Transmit(initConfig.uartDevice, structureM, size,
  UART_DELAY) != HAL_OK)
  {
    return ERR_E22_NO_RESPONSE_FROM_DEVICE;
  }
  return LoRaE22_waitCompleteResponse(RESPONSE_DELAY);
}

LoRaTypedef_STATUS LoRaE22_receiveStruct(uint8_t* structureM, uint16_t size, uint8_t RSSI)
{
  if (HAL_UART_Receive(initConfig.uartDevice, structureM, MAX_SIZE_TX_PACKET, UART_DELAY) != HAL_OK)
    return ERR_E22_NO_RESPONSE_FROM_DEVICE;

  LoRaE22_waitCompleteResponse(RESPONSE_DELAY);
  if(RSSI)
  {
    uint8_t rssi[1];
    HAL_UART_Receive(initConfig.uartDevice, rssi, 1, UART_DELAY);
    rc.rssi = rssi;
  }
  return LoRaE22_waitCompleteResponse(RESPONSE_DELAY);

}

HAL_StatusTypeDef LoRaE22_writeProgramCommand(
    LoRaTypedef_PROG_CMD cmd,
    LoRaTypedef_REG_ADDRS addr,
    LoRaTypedef_PACKET_LENGHT pl)
{
  uint8_t CMD[3] = { cmd, addr, pl };
  return HAL_UART_Transmit(initConfig.uartDevice, CMD, 3, UART_DELAY);
  _managedDelay(10);  //need ti check
}