/*
 * Author: Nandkishor Sonar
 * Contributors: Alex Tereschenko <alext.mkrs@gmail.com>
 * Contributors: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Reads ADC A0 value continuously. Press Ctrl+C to exit.
 */

/* standard headers */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/aio.h"

/* AIO port */
#define AIO_PORT 512 + 0

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int
main()
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_aio_context aio;
    int16_t value = 0;

    signal(SIGINT, sig_handler);

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();
    mraa_add_subplatform(MRAA_GENERIC_FIRMATA, "/dev/ttyACM0");
    //! [Interesting]
    /* initialize AIO */
    aio = mraa_aio_init(AIO_PORT);
    if (aio == NULL) {
        fprintf(stderr, "Failed to initialize AIO\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }
    sleep(1);
    while (flag) {
        value = mraa_aio_read(aio);
        fprintf(stdout, "ADC A0 read %X - %d\n", value, value);
        sleep(1);
    }

    // /* close AIO */
    status = mraa_aio_close(aio);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);
    
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
