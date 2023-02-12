/**
 * ADS1115_sample.c - 12/9/2013.
 * Written by David Purdie as part of the openlabtools initiative
 * Initiates and reads a single sample from the ADS1115 (without
 * error handling)
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>          // open
#include <inttypes.h>       // uint8_t, etc
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>  // I2C bus definitions

#define DELAY           150000
#define CONVERSION_REG  0x00
#define CONFIG_REG      0x01
#define CFG_OS_MASK     0x8000
#define CFG_MUX_MASK    0x7000
#define CFG_PGA_MASK    0x0E00
#define CFG_MODE_MASK   0x0100
#define CFG_DR_MASK     0x00E0
#define CFG_COMP_MODE_MASK  0x0010
#define CFG_COMP_POL_MASK   0x0008
#define CFG_COMP_LAT_MASK   0x0004
#define CFG_COMP_QUE_MASK   0x0003

const float FSR[8] = {
    6.144,
    4.096,
    2.048,  // Default
    1.024,
    0.512,
    0.256,
    0.256,
    0.256
};

int16_t readADC(int handler)
{
    if (handler < 0) return 0;

    uint8_t writeBuf[3];      // Buffer to store the 3 bytes that we write to the I2C device
    uint8_t readBuf[2];       // 2 byte buffer to store the data read from the I2C device
    int16_t ret;              // Stores the 16 bit value of our ADC conversion

    // These three bytes are written to the ADS1115 to set the config register and start a
    // conversion 
    writeBuf[0] = CONFIG_REG; // This sets the pointer register so that the following two
                              // bytes write to the config register
    writeBuf[1] = 0xC3;       // This sets the 8 MSBs of the config register (bits 15-8) to
                              // 11000011
    writeBuf[2] = 0x03;       // This sets the 8 LSBs of the config register (bits 7-0) to
                              // 00000011

    // Initialize the buffer used to read data from the ADS1115 to 0
    readBuf[0]= 0;
    readBuf[1]= 0;

    // Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
    // this begins a single conversion
    write(handler, writeBuf, 3);

    // Wait for the conversion to complete, this requires bit 15 to change from 0->1
    while ((readBuf[0] & 0x80) == 0)// readBuf[0] contains 8 MSBs of config register, AND
                                    // with 10000000 to select bit 15
    {
        read(handler, readBuf, 2);  // Read the config register into readBuf
    }

    writeBuf[0] = CONVERSION_REG;   // Set pointer register to 0 to read from the conversion
                                    // register
    write(handler, writeBuf, 1);

    read(handler, readBuf, 2);      // Read the contents of the conversion register into
                                    // readBuf

    ret = readBuf[0] << 8 | readBuf[1];   // Combine the two bytes of readBuf into a single
                                          // 16 bit result 
    return ret;
}

int main() {

    int ADS_address = 0x48;   // Address of our device on the I2C bus
    int I2CFile;
    int16_t readVal = 0;


    I2CFile = open("/dev/i2c-1", O_RDWR);     // Open the I2C device

    ioctl(I2CFile, I2C_SLAVE, ADS_address);   // Specify the address of the I2C Slave to
                                            // communicate with

    for (;;) {
        readVal = readADC(I2CFile);
        // Print the result to terminal, first convert from binary value to mV
        printf("readVal = %u, Voltage Reading %f (V) \n", readVal, (float)readVal*FSR[1]/32767.0);
        usleep(DELAY);
    }

    close(I2CFile);

    return 0;

}
