/* standard headers */
#include <endian.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/i2c.h"
#define I2C_BUS 512+0

#define ADS1115_GND_ADDRESS    (0x48)  // ADDR pin connect to GND

#define ADS1115_POINTER_CONVERT     (0x00)
#define ADS1115_POINTER_CONFIG      (0x01)
#define ADS1115_DEFAULT_CONFIG    0x8583

#define ADS1115_MODE_MASK    (0x0100)    // Device operating mode
#define ADS1115_MODE_CONTIN  (0x0000)    // Continuous conversion mode
#define ADS1115_MODE_SINGLE  (0x0100)    // Power-down single-shot mode (default)

#define ADS1115_OS_MASK      (0x8000)    // Operational status/single-shot conversion start
#define ADS1115_OS_NOEFFECT  (0x0000)    // Write: Bit = 0
#define ADS1115_OS_SINGLE    (0x8000)    // Write: Bit = 1
#define ADS1115_OS_BUSY      (0x0000)    // Read: Bit = 0
#define ADS1115_OS_NOTBUSY   (0x8000)    // Read: Bit = 1

#define ADS1115_MUX_MASK     (0x7000)    // Input multiplexer configuration
#define ADS1115_MUX_0_1      (0x0000)    // P = AIN0, N = AIN1(default)
#define ADS1115_MUX_0_3      (0x1000)    // P = AIN0, N = AIN3
#define ADS1115_MUX_1_3      (0x2000)    // P = AIN1, N = AIN3
#define ADS1115_MUX_2_3      (0x3000)    // P = AIN2, N = AIN3
#define ADS1115_MUX_0_G      (0x4000)    // P = AIN0, N = GND
#define ADS1115_MUX_1_G      (0x5000)    // P = AIN1, N = GND
#define ADS1115_MUX_2_G      (0x6000)    // P = AIN2, N = GND
#define ADS1115_MUX_3_G      (0x7000)    // P = AIN3, N = GND
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
main(void)
{   
    int16_t value = 0;
    uint8_t data[3] = {0};
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;
    /* install signal handler */
    signal(SIGINT, sig_handler);
    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();
    mraa_add_subplatform(MRAA_GENERIC_FIRMATA, "/dev/ttyACM0");
    /* initialize I2C bus */
    i2c = mraa_i2c_init(I2C_BUS);        
    if (i2c == NULL) {
        fprintf(stderr, "Failed to initialize I2C\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }
    /* set slave address */
    status = mraa_i2c_address(i2c, ADS1115_GND_ADDRESS);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }
    value = ADS1115_DEFAULT_CONFIG;
    //set P=A0 and N=GND
    value &= ~ADS1115_MUX_MASK;
    value |= ADS1115_MUX_0_G;    
    //set Continuous conversion mode
    value &= ~ADS1115_MODE_MASK;
    value |= ADS1115_MODE_CONTIN;
    //wirte config to reg 
    data[0] = ADS1115_POINTER_CONFIG;
    data[1] = (uint8_t)(value >> 8)&0xff;
    data[2] = (uint8_t)(value & 0xff);
    status = mraa_i2c_write(i2c,data,3);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }    

    sleep(1);
    
    //check the reg
    mraa_i2c_read_bytes_data(i2c,ADS1115_POINTER_CONFIG,data,2);
    value =  data[0] << 8 | data[1];
    fprintf(stdout, "the ADS1115_DEFAULT_CONFIG value is  %X - %d\n", value, value);
    
    while (flag) {
        //read A0 adc Value
        mraa_i2c_read_bytes_data(i2c,ADS1115_POINTER_CONVERT,data,2);
        value =  data[0] << 8 | data[1];
        if (value < 0) value = 0;
        fprintf(stdout, "the A0 value is  %X - %d\n", value, value);
        sleep(1);
    }
err_exit:
    mraa_result_print(status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}