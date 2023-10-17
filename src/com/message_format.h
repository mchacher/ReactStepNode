/**
 * @file message_format.h
 * @author mchacher
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef MESSAGE_FORMAT_H
#define MESSAGE_FORMAT_H

#include "Arduino.h"
#include "../reactmagic/event_type.h"

#define DATA_BUFFER_SIZE 252   // UART_BUFFER_SIZE - HEADER_SIZE
#define LOG_MESSAGE_SIZE 128
#define FILE_CHUNK_SIZE  196
#define FILE_NAME_SIZE   16

 /**
  * @brief serial msg types
  *
  */
typedef enum
{
  MSG_TYPE_HEARTBEAT = 0,
  MSG_TYPE_LOG = 1,
  MSG_TYPE_SYS = 2,
  MSG_TYPE_FILE = 3, // for file transfer from a computer
  MSG_TYPE_EVENT = 4
} MSG_TYPE;

/**
 * @brief RF packet
 *
 * @return typedef struct
 */
typedef struct __attribute__((__packed__))
{
  uint8_t payload[255];
} RF_PACKET;

#endif
