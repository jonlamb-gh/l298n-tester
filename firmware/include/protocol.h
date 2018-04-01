/**
 * @file protocol.h
 * @brief TODO.
 *
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

#define PROTO_MSG_SIZE (64)
#define PROTO_MSG_PREAMBLE (0xADC4)

typedef struct __attribute__((packed))
{
    uint16_t preamble;
    uint32_t start_time;
    uint32_t end_time;
    uint16_t error_cnt;
    // TODO
    uint8_t reserved[64-14];
    uint16_t checksum;
} proto_msg_s;

uint16_t protocol_crc16(
        const proto_msg_s * const msg);

#endif  /* PROTOCOL_H */
