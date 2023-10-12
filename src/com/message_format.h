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
#include "reactmagic/event_type.h"

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
  SERIAL_MSG_TYPE_HEARTBEAT = 0,
  SERIAL_MSG_TYPE_LOG = 1,
  SERIAL_MSG_TYPE_SYS = 2,
  SERIAL_MSG_TYPE_FILE = 3, // for file transfer from a computer
  SERIAL_MSG_TYPE_EVENT = 4
} SERIAL_MSG_TYPE;

/**
 * @brief serial packet header
 *
 * @return typedef struct
 */
typedef struct __attribute__((__packed__))
{
  uint16_t packet_id;
  uint8_t type;
  uint8_t data_length;
} PACKET_HEADER;

/**
 * @brief serial packet
 *
 * @return typedef struct
 */
typedef struct __attribute__((__packed__))
{
  PACKET_HEADER header;
  uint8_t payload[DATA_BUFFER_SIZE];
} PACKET;

/**
 * @brief serial packet
 *
 * @return typedef struct
 */
typedef struct __attribute__((__packed__))
{
  PACKET_HEADER header;
  EVENT event;
} PACKET_EVENT;

/**
 * @brief log payload
 *
 * @return typedef struct
 */
typedef struct __attribute__((__packed__))
{
  uint8_t level;
  uint8_t message[LOG_MESSAGE_SIZE];
} PAYLOAD_LOG;

/**
 * @brief file payload
 *
 * @return typedef struct
 */
typedef struct __attribute__((__packed__))
{
  uint8_t chunk_id;
  uint8_t number_of_chunks;
  uint8_t chunk_size;
  char name[FILE_NAME_SIZE];
  uint8_t data[FILE_CHUNK_SIZE];
} PAYLOAD_FILE;


#endif
