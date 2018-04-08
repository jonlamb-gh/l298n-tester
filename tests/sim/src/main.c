/**
 * @file main.c
 * @brief Main.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>

#include "sim_avr.h"
#include "sim_elf.h"
#include "sim_gdb.h"
#include "sim_vcd_file.h"
#include "avr_ioport.h"
#include "avr_timer.h"

#define TIMER0_COMPA_vect (21)

static sig_atomic_t global_exit_signal;

static void sig_handler(
        int sig)
{
    if(sig == SIGINT)
    {
        global_exit_signal = 1;
    }
}

static void timer_irq_changed_hook(
        struct avr_irq_t * irq,
        uint32_t value,
        void * param)
{
    //printf("*** IRQ cb name: %s\n", irq->name);
}

int main(int argc, char **argv)
{
    elf_firmware_t frmw;
    avr_vcd_t vcd_file;
    struct sigaction act;
    int verbose = 0;

    if((argc < 2) || (argc > 3))
    {
        fprintf(stderr, "args error\n");
        exit(1);
    }

    // TODO clean this mess up
    const char * const frmw_name = argv[1];

    if(argc == 3)
    {
        if(strcmp("-v", argv[2]) == 0)
        {
            verbose = 1;
        }
    }

    (void) memset(&frmw, 0, sizeof(frmw));

    global_exit_signal = 0;
    memset(&act, 0, sizeof(act));
    act.sa_flags = SA_RESTART;
    act.sa_handler = sig_handler;

    if(sigaction(SIGINT, &act, 0) < 0)
    {
        fprintf(stderr, "sigaction(SIGINT) failed\n");
        exit(1);
    }

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

    if(verbose != 0)
    {
        avr->trace = 1;
    }

    avr_load_firmware(avr, &frmw);

    avr_irq_t * const int_irq = avr_get_interrupt_irq(avr, TIMER0_COMPA_vect);

    printf(
            "TIMER0_COMPA_vect (%u | 0x%02X) IRQ name: %s\n",
            (unsigned int) TIMER0_COMPA_vect,
            (unsigned int) TIMER0_COMPA_vect,
            int_irq->name);

    avr_irq_register_notify(
            int_irq,
            timer_irq_changed_hook,
            NULL);

    const char *btn_name= "btn2";
    avr_irq_t * const btn_irq = avr_alloc_irq(&avr->irq_pool, 0, 1, &btn_name);

    // "connect" the output irq of the button to the port pin of the AVR
    avr_connect_irq(
        btn_irq,
        avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), IOPORT_IRQ_PIN7));

    // 'raise' button, it's a "pullup"
    avr_raise_irq(btn_irq, 1);

    // flush period in usec
    avr_vcd_init(avr, "gtkwave_output.vcd", &vcd_file, 10000UL);

    avr_vcd_add_signal(
            &vcd_file,
            btn_irq,
            1,
            "BTN2");

    avr_vcd_add_signal(
            &vcd_file,
            avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('C'), IOPORT_IRQ_PIN_ALL),
            8,
            "PORTC");

    // LED on D6, btn2 on D7
    avr_vcd_add_signal(
            &vcd_file,
            avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), IOPORT_IRQ_PIN_ALL),
            8,
            "PORTD");

    avr_vcd_add_signal(
            &vcd_file,
            int_irq,
            1,
            "TIMER0_COMPA");

    printf("starting simulation\n");
    avr_vcd_start(&vcd_file);

    uint32_t cycles = 0;
    uint8_t do_btn_press = 1;

    // ~5 seconds
    const uint32_t MAX_CYCLES = 0x3BFFFFF;
    const uint32_t BTN_CYCLES = MAX_CYCLES/4;

    int state = cpu_Running;
    while((state != cpu_Done) && (state != cpu_Crashed))
    {
        state = avr_run(avr);

        if((cycles > BTN_CYCLES) && (do_btn_press == 1))
        {
            avr_raise_irq(btn_irq, 0);

            do_btn_press = 0;
        }

        cycles += 1;

        if((cycles >= MAX_CYCLES) || (global_exit_signal != 0))
        {
            state = cpu_Done;
        }
    }

    avr_vcd_stop(&vcd_file);

    if(avr != NULL)
    {
        avr_terminate(avr);
    }

    printf("simulation over - state: %d\n", state);

    return EXIT_SUCCESS;
}
