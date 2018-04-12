/**
 * @file msg.c
 * @brief TODO.
 *
 */

#include <string.h>
#include <stdint.h>
#include <avr/io.h>

#include "board_def.h"
#include "protocol.h"
#include "transport.h"
#include "msg.h"

void msg_init(
        const uint16_t error_cnt,
        proto_msg_s * const msg)
{
    (void) memset(msg, 0, sizeof(*msg));
    msg->preamble = PROTO_MSG_PREAMBLE;
    msg->error_cnt = error_cnt;
    msg->running_proc = 0xFF;
}

// updates counter/etc
void msg_send(
        proto_msg_s * const msg)
{
    msg->cnt += 1;

    msg->checksum = protocol_crc16(msg);

    const uint8_t err = transport_send(msg);

    if(err != 0)
    {
        msg->error_cnt += 1;
    }
}
