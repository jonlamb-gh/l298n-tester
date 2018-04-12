/**
 * @file msg.h
 * @brief TODO.
 *
 */

#ifndef MSG_H
#define MSG_H

#include <stdint.h>

#include "protocol.h"

void msg_init(
        const uint16_t error_cnt,
        proto_msg_s * const msg);

// updates counter/etc
void msg_send(
        proto_msg_s * const msg);

#endif /* MSG_H */
