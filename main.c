#include <stdio.h>
#include "pico/stdlib.h" //uart definitions
#include <stdlib.h> //atoi,atol
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "tusb.h"

uint8_t capture_buf[2048];
uint8_t capture_buf2[2048];
volatile uint32_t *tstsa0,*tstsa1;
volatile uint32_t *taddra0,*taddra1;
volatile int lowerhalf; //are we processing the upper or lower half of the data buffers

void my_stdio_usb_out_chars(const char *buf, int length) {
    static uint64_t last_avail_time;
    uint32_t owner;
    if (tud_cdc_connected()) {
        for (int i = 0; i < length;) {
            int n = length - i;
            int avail = (int) tud_cdc_write_available();
            if (n > avail) n = avail;
            if (n) {
                int n2 = (int) tud_cdc_write(buf + i, (uint32_t)n);
                tud_task();
		tud_cdc_write_flush();
                i += n2;
                last_avail_time = time_us_64();
            } else {
                tud_task();
		tud_cdc_write_flush();
                if (!tud_cdc_connected() ||
                    (!tud_cdc_write_available() && time_us_64() > last_avail_time + PICO_STDIO_USB_STDOUT_TIMEOUT_US)) {
                    break;
                }
            }
        }
    } else {
        // reset our timeout
        last_avail_time = 0;
    }
}

void main() {
#define SYS_CLK_BASE 120000
    set_sys_clock_khz(SYS_CLK_BASE,true);
    stdio_usb_init();
    sleep_ms(4000);

    adc_gpio_init(26);
    adc_init();
    adc_select_input(0);
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable.
        true     // Shift each sample to 8 bits when pushing to FIFO
    );
    adc_set_clkdiv(9600);
    sleep_ms(10);
    uint dma_chan1 = dma_claim_unused_channel(true);
    dma_channel_config cfg1 = dma_channel_get_default_config(dma_chan1);

    uint dma_chan2 = dma_claim_unused_channel(true);
    dma_channel_config cfg2 = dma_channel_get_default_config(dma_chan2);

    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&cfg1, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg1, false);
    channel_config_set_write_increment(&cfg1, true);

    channel_config_set_transfer_data_size(&cfg2, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg2, false);
    channel_config_set_write_increment(&cfg2, true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg1, DREQ_ADC);
    channel_config_set_dreq(&cfg2, DREQ_ADC);

    dma_channel_configure(dma_chan1, &cfg1,
        capture_buf,    // dst
        &adc_hw->fifo,  // src
        2048,  // transfer count
        true            // start immediately
    );

    dma_channel_configure(dma_chan2, &cfg2,
        capture_buf2,    // dst
        &adc_hw->fifo,  // src
        2048,  // transfer count
        true            // start immediately
    );
    channel_config_set_chain_to(&cfg2, dma_chan1);
    adc_run(true);
    while (1) {
        dma_channel_wait_for_finish_blocking(dma_chan1);
        my_stdio_usb_out_chars(capture_buf, 2048);
        dma_channel_configure(dma_chan1, &cfg1,
            capture_buf,    // dst
            &adc_hw->fifo,  // src
            2048,  // transfer count
            true            // start immediately
        );
        channel_config_set_chain_to(&cfg1, dma_chan2);
        dma_channel_wait_for_finish_blocking(dma_chan2);
        my_stdio_usb_out_chars(capture_buf2, 2048);
        dma_channel_configure(dma_chan2, &cfg2,
            capture_buf2,    // dst
            &adc_hw->fifo,  // src
            2048,  // transfer count
            true            // start immediately
        );
        channel_config_set_chain_to(&cfg2, dma_chan1);
    }
}