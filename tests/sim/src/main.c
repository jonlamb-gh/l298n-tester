/**
 * @file main.c
 * @brief Main.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"
#include "avr_ioport.h"

int main(int argc, char **argv)
{
    elf_firmware_t frmw;
    avr_vcd_t vcd_file;

    if(argc != 2)
    {
        exit(1);
    }

    // TODO

    const char * const frmw_name = argv[1];

    (void) memset(&frmw, 0, sizeof(frmw));

    printf("firmware: %s\n", frmw_name);

    elf_read_firmware(frmw_name, &frmw);

    printf("frequency: %lu\nmmcu: %s\n", (unsigned long) frmw.frequency, frmw.mmcu);

    avr_t * const avr = avr_make_mcu_by_name(frmw.mmcu);
    if(avr == NULL)
    {
        fprintf(stderr, "error\n");
        exit(1);
    }

    avr->log = LOG_DEBUG;
    avr_init(avr);

    avr->log = LOG_DEBUG;
    avr->gdb_port = 1234;
    //avr->trace = 1;

    avr_load_firmware(avr, &frmw);

    // usec
    avr_vcd_init(avr, "gtkwave_output.vcd", &vcd_file, 100000);

    avr_vcd_add_signal(
            &vcd_file,
            avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), IOPORT_IRQ_PIN_ALL), 8, "PORTB");

    avr_vcd_start(&vcd_file);

    printf("starting simulation\n");

    uint32_t cycles = 0;
    const uint32_t MAX_CYCLES = 0xFFFFFF;

    int state = cpu_Running;
    while((state != cpu_Done) && (state != cpu_Crashed))
    {
        state = avr_run(avr);

        cycles += 1;
        if(cycles >= MAX_CYCLES)
        {
            state = cpu_Done;
        }
    }

    if(avr != NULL)
    {
        avr_terminate(avr);
    }

    printf("simulation over - state: %d\n", state);

    return EXIT_SUCCESS;
}
