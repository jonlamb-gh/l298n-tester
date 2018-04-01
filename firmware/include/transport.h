/**
 * @file transport.h
 * @brief TODO.
 *
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <stdint.h>

#include "protocol.h"

#define TRANSPORT_TX_TIMEOUT (100)

void transport_init(void);

uint8_t transport_ready(void);

uint8_t transport_send(
        const proto_msg_s * const msg);

#endif  /* TRANSPORT_H */
