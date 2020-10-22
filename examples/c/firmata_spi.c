/*
 * Author: Michael Ring <mail@michael-ring.org>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Display set of patterns on MAX7219 repeately.
 *                Press Ctrl+C to exit
 */

/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/spi.h"

/* SPI declaration */
#define SPI_BUS 512+0

/* SPI frequency in Hz */
#define SPI_FREQ 10000000

int
main(int argc, char** argv)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_spi_context spi;

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();
    mraa_add_subplatform(MRAA_GENERIC_FIRMATA, "/dev/ttyACM0");

    //! [Interesting]
    /* initialize SPI bus */
    spi = mraa_spi_init(SPI_BUS);
    if (spi == NULL) {
        fprintf(stderr, "Failed to initialize SPI\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set SPI frequency */
    status = mraa_spi_frequency(spi, SPI_FREQ);
    if (status != MRAA_SUCCESS)
        goto err_exit;

    /* set big endian mode */
    status = mraa_spi_lsbmode(spi, 0);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }
    uint8_t tx_data[4] = {0xc0};
    uint8_t rx_data[4] = {0x00};
    mraa_spi_transfer_buf(spi,tx_data,rx_data,1);
    sleep(1);
    tx_data[0] = 0x03;
    tx_data[1] = 0x0e;
    tx_data[2] = 0x00;
    mraa_spi_transfer_buf(spi,tx_data,rx_data,3);  
    fprintf(stdout,"rx_data[2] is %x\r\n",rx_data[2]);   
    tx_data[0] = 0x05;
    tx_data[1] = 0xf;
    tx_data[2] = 0xe0;
    tx_data[3] = 0x80;
    mraa_spi_transfer_buf(spi,tx_data,rx_data,4);
    tx_data[0] = 0x03;
    tx_data[1] = 0x0f;
    tx_data[2] = 0x00;
    mraa_spi_transfer_buf(spi,tx_data,rx_data,3);

    fprintf(stdout,"rx_data[2] is %x\r\n",rx_data[2]);   

    /* stop spi */
    mraa_spi_stop(spi);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* stop spi */
    mraa_spi_stop(spi);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
