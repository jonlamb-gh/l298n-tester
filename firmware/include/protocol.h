/**
 * @file protocol.h
 * @brief TODO.
 *
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#include "input.h"
#include "driver.h"

#define PROTO_MSG_SIZE (64)
#define PROTO_MSG_PREAMBLE (0xADC4)

#define PROTO_MSG_LOG_MSG_SIZE (20)

typedef struct __attribute__((packed))
{
    uint16_t preamble;
    uint32_t start_time;
    uint32_t end_time;
    uint16_t error_cnt;
    uint16_t cnt;
    input_s input_state;
    driver_s driver_state;
    uint8_t log_msg_size;
    uint8_t log_msg[PROTO_MSG_LOG_MSG_SIZE];
    uint16_t checksum;
} proto_msg_s;

uint16_t protocol_crc16(
        const proto_msg_s * const msg);

#endif  /* PROTOCOL_H */
