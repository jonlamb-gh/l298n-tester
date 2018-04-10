/**
 * @file main.c
 * @brief Main.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "hid.h"
#include "protocol.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

int main(int argc, char **argv)
{
    int ret;
    int bytes_read;
    unsigned long local_msg_cnt;
    char buffer[PROTO_MSG_SIZE + 1];
    char log_msg[PROTO_MSG_LOG_MSG_SIZE + 1];

    ret = rawhid_open(
            1,
            0x16C1,
            0x0489,
            0xFF12,
            0x0212);

    if(ret != 1)
    {
        printf("failed to open\n");
        exit(1);
    }

    while(1)
    {
        bytes_read = rawhid_recv(
                0,
                buffer,
                PROTO_MSG_SIZE,
                10 * 1000UL);

        if(bytes_read < 0)
        {
            printf("failed to read\n");
            exit(1);
        }

        if(bytes_read == PROTO_MSG_SIZE)
        {
            local_msg_cnt += 1;

            const proto_msg_s * const msg =
                    (proto_msg_s * const) buffer;

            printf("(local_cnt: %lu)\n", local_msg_cnt);

            printf("preamble: 0x%04lX\n", (unsigned long) msg->preamble);
            printf("start_time: %lu\n", (unsigned long) msg->start_time);
            printf("end_time: %lu\n", (unsigned long) msg->end_time);
            printf("error_cnt: %lu\n", (unsigned long) msg->error_cnt);
            printf("cnt: %lu\n", (unsigned long) msg->cnt);

            printf("input_state\n");
            printf("  bt0: %lu\n", (unsigned long) msg->input_state.bt0);
            printf("  bt1: %lu\n", (unsigned long) msg->input_state.bt1);
            printf("  bt2: %lu\n", (unsigned long) msg->input_state.bt2);
            printf("  sw0: %lu\n", (unsigned long) msg->input_state.sw0);
            printf("  pt0: %lu\n", (unsigned long) msg->input_state.pt0);
            printf("  pt1: %lu\n", (unsigned long) msg->input_state.pt1);

            printf("driver_state\n");
            printf("  in1: %lu\n", (unsigned long) msg->driver_state.in1);
            printf("  in2: %lu\n", (unsigned long) msg->driver_state.in2);
            printf("  en: %lu\n", (unsigned long) msg->driver_state.en);
            printf("  cs: %lu\n", (unsigned long) msg->driver_state.cs);
            printf(
                    "  pwm_duty: %lu = %% %.2f\n",
                    (unsigned long) msg->driver_state.pwm_duty,
                    (double) msg->driver_state.pwm_duty / 1023.0);
            printf("  pwm_period: %lu\n", (unsigned long) msg->driver_state.pwm_period);
            printf("  pwm_cs_bits: 0x%02X\n", (unsigned int) msg->driver_state.pwm_cs_bits);
            printf("  pwm_ocr: %lu\n", (unsigned long) msg->driver_state.pwm_ocr);
            printf("  pwm_icr: %lu\n", (unsigned long) msg->driver_state.pwm_icr);

            const double clock_map[] =
            {
                [0b000] = 0.0,
                [0b001] = 1.0,
                [0b010] = 8.0,
                [0b011] = 64.0,
                [0b100] = 256.0,
                [0b101] = 1024.0,
            };

            const double clock = clock_map[msg->driver_state.pwm_cs_bits];
            const double freq = 1000000.0 / (double) msg->driver_state.pwm_period;

            printf("    - clock: %f\n", clock);
            printf("    - freq: %f\n", freq);

            (void) strncpy(
                    log_msg,
                    (char*) msg->log_msg,
                    (size_t) MIN(PROTO_MSG_LOG_MSG_SIZE, msg->log_msg_size));

            printf("log_msg_size: %lu\n", (unsigned long) msg->log_msg_size);
            printf("log_msg: '%s'\n", log_msg);
            printf("checksum: 0x%04lX\n", (unsigned long) msg->checksum);

            printf("\n\n");
            (void) fflush(stdout);
        }
        else
        {
            exit(1);
        }
    }

    return EXIT_SUCCESS;
}
