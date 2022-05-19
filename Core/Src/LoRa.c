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

LoRaTypedef_STATUS LoRaE22_sendStruct(void *structureM, uint16_t size)
{
  if (size > MAX_SIZE_TX_PACKET + 2)
  {
    return ERR_E22_PACKET_TOO_BIG;
  }
  if (HAL_UART_Transmit(initConfig.uartDevice, (uint8_t*) structureM, size,
  UART_DELAY) != HAL_OK)
  {
    return ERR_E22_NO_RESPONSE_FROM_DEVICE;
  }
  return LoRaE22_waitCompleteResponse(RESPONSE_DELAY);
}

LoRaTypedef_STATUS LoRaE22_receiveStruct(void *structureM, uint16_t size)
{
  uint8_t buf[MAX_SIZE_TX_PACKET] = {'\0'};
  HAL_StatusTypeDef a = HAL_UART_Receive(initConfig.uartDevice, buf, MAX_SIZE_TX_PACKET, UART_DELAY);
  if (a != HAL_OK)
    return ERR_E22_NO_RESPONSE_FROM_DEVICE;

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

LoRaResponseContainer LoRaE22_getConfiguration()
{
  LoRaResponseContainer rc;
  LoRaTypedef_MODES prevMode = _mode;

  rc.status.code = LoRaE22_SetMode(E22_CONFIG_MODE);
  if (rc.status.code != E22_SUCCESS)
    return rc;

  if (LoRaE22_writeProgramCommand(E22_READ_CONFIGURATION, E22_REG_ADDRESS_CFG, E22_PL_CONFIGURATION) == HAL_OK)

  {
    rc.data = malloc(sizeof(LoRaConfiguration));
    rc.status.code = LoRaE22_receiveStruct((uint8_t*) rc.data, sizeof(LoRaConfiguration));
  }
  else
  {
    rc.status.code = ERR_E22_NO_RESPONSE_FROM_DEVICE;
    return rc;
  }

  if (rc.status.code != E22_SUCCESS)
  {
    LoRaE22_SetMode(prevMode);
    return rc;
  }

  rc.status.code = LoRaE22_SetMode(prevMode);
  if (rc.status.code != E22_SUCCESS)
    return rc;

  if (E22_WRONG_FORMAT == ((LoRaConfiguration*) rc.data)->COMMAND)
  {
    rc.status.code = ERR_E22_WRONG_FORMAT;
  }
  if (E22_RETURNED_COMMAND != ((LoRaConfiguration*) rc.data)->COMMAND
      || E22_REG_ADDRESS_CFG != ((LoRaConfiguration*) rc.data)->STARTING_ADDRESS
      || E22_PL_CONFIGURATION != ((LoRaConfiguration*) rc.data)->LENGTH)
  {
    rc.status.code = ERR_E22_HEAD_NOT_RECOGNIZED;
  }

  return rc;
}

struct _LoRaResponseStatus LoRaE22_setConfiguration(LoRaConfiguration configuration, LoRaTypedef_PROG_CMD saveType)
{
  struct _LoRaResponseStatus rc;

  LoRaTypedef_MODES prevMode = _mode;

  rc.code = LoRaE22_SetMode(E22_CONFIG_MODE);
  if (rc.code != E22_SUCCESS)
    return rc;

  configuration.COMMAND = saveType;
  configuration.STARTING_ADDRESS = E22_REG_ADDRESS_CFG;
  configuration.LENGTH = E22_PL_CONFIGURATION;

  rc.code = LoRaE22_sendStruct((uint8_t*) &configuration, sizeof(LoRaConfiguration));
  if (rc.code != E22_SUCCESS)
  {
    LoRaE22_SetMode(prevMode);
    return rc;
  }

  rc.code = LoRaE22_receiveStruct((uint8_t*) &configuration, sizeof(LoRaConfiguration));

  rc.code = LoRaE22_SetMode(prevMode);
  if (rc.code != E22_SUCCESS)
    return rc;

  if (E22_WRONG_FORMAT == ((LoRaConfiguration*) &configuration)->COMMAND)
  {
    rc.code = ERR_E22_WRONG_FORMAT;
  }
  if (E22_RETURNED_COMMAND != ((LoRaConfiguration*) &configuration)->COMMAND
      || E22_REG_ADDRESS_CFG != ((LoRaConfiguration*) &configuration)->STARTING_ADDRESS
      || E22_PL_CONFIGURATION != ((LoRaConfiguration*) &configuration)->LENGTH)
  {
    rc.code = ERR_E22_HEAD_NOT_RECOGNIZED;
  }

  return rc;
}

LoRaResponseContainer LoRaE22_getModuleInformation()
{
  LoRaResponseContainer rc;

  LoRaTypedef_MODES prevMode = _mode;

  rc.status.code = LoRaE22_SetMode(E22_CONFIG_MODE);
  if (rc.status.code != E22_SUCCESS)
    return rc;

  LoRaE22_writeProgramCommand(E22_READ_CONFIGURATION, E22_REG_ADDRESS_PID, E22_PL_PID);

  rc.data = malloc(sizeof(LoRaModuleInformation));

  rc.status.code = LoRaE22_receiveStruct((uint8_t*) rc.data, sizeof(LoRaModuleInformation));
  if (rc.status.code != E22_SUCCESS)
  {
    LoRaE22_SetMode(prevMode);
    return rc;
  }

  rc.status.code = LoRaE22_SetMode(prevMode);
  if (rc.status.code != E22_SUCCESS)
    return rc;

  if (E22_WRONG_FORMAT == ((LoRaModuleInformation*) rc.data)->COMMAND)
  {
    rc.status.code = ERR_E22_WRONG_FORMAT;
  }
  if (E22_RETURNED_COMMAND != ((LoRaModuleInformation*) rc.data)->COMMAND
      || E22_REG_ADDRESS_PID != ((LoRaModuleInformation*) rc.data)->STARTING_ADDRESS
      || E22_PL_PID != ((LoRaModuleInformation*) rc.data)->LENGTH)
  {
    rc.status.code = ERR_E22_HEAD_NOT_RECOGNIZED;
  }

  return rc;
}

LoRaResponseContainer LoRaE22_receiveMessage(const uint8_t size, uint8_t rssiEnabled)
{
  LoRaResponseContainer rc;
  rc.data = malloc(size);
  rc.status.code = LoRaE22_receiveStruct((uint8_t*) rc.data, size);
  if (rc.status.code != E22_SUCCESS)
  {
    return rc;
  }

  if (rssiEnabled)
  {

    uint8_t rssi[1];
    HAL_UART_Receive(initConfig.uartDevice, rssi, 1, UART_DELAY);
    rc.rssi = rssi[0];
  }

  return rc;
}

struct _LoRaResponseStatus LoRaE22_sendMessage(const void *message, const uint8_t size)
{
  struct _LoRaResponseStatus status;
  status.code = LoRaE22_sendStruct((uint8_t*) message, size);
  return status;
}

typedef struct fixedStransmission
{
  uint8_t ADDH;
  uint8_t ADDL;
  uint8_t CHAN;
  uint8_t message[];
} FixedStransmission;

FixedStransmission* init_stack(uint16_t m)
{
  FixedStransmission *st = (FixedStransmission*) malloc(sizeof(FixedStransmission) + m * sizeof(uint16_t));
  return st;
}

struct _LoRaResponseStatus LoRaE22_sendFixedMessage(
    uint8_t ADDH,
    uint8_t ADDL,
    uint8_t CHAN,
    const void *message,
    const uint16_t size)
{

  FixedStransmission *fixedStransmission = init_stack(size);

  fixedStransmission->ADDH = ADDH;
  fixedStransmission->ADDL = ADDL;
  fixedStransmission->CHAN = CHAN;

  memcpy(fixedStransmission->message, (unsigned char*) message, size);

  struct _LoRaResponseStatus status;
  status.code = LoRaE22_sendStruct((uint8_t*) fixedStransmission, (uint16_t) (size + 3));
  free(fixedStransmission);

  return status;
}

struct _LoRaResponseStatus LoRaE22_sendBroadcastFixedMessage(uint8_t CHAN, const void *message, const uint16_t size)
{
  return LoRaE22_sendFixedMessage(0xFF, 0xFF, CHAN, message, size);
}
