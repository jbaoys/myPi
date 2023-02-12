/**
 * thermistor.c - 2/4/2020.
 * Using Thermistor MF52 104 (100K)
 * Raspberry Pi         ADS1115
 *    3V3(1) <--------> VDD <---------> R0(100K) <--------.
 *    SDA1(3) <-------> SDA                               |
 *    SCL1(5) <-------> SCL                               |
 *    GND(6) <--------> GND <---------> Thermistor(104) <-+
 *    GND(6) <--------> ADDR                              |
 *                      A0 <------------------------------'
 * Compiling command: gcc -o thermistor thermistor2.c -lm
 * Running result:
 * pi@raspberrypi:~/myPi/ads1115 $ ./thermistor 
 * Vout = 29669.400391, Rt = 128855.250000, Temperature: 20.107014 (C), 68.192627 (F)
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>          // open
#include <inttypes.h>       // uint8_t, etc
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>  // I2C bus definitions
#include <math.h>

#define DELAY           500000
#define DELAY2          10000
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

#define CFG_OS_SHIFT    15
#define CFG_MUX_SHIFT   12
#define CFG_PGA_SHIFT   9
#define CFG_MODE_SHIFT  8
#define CFG_DR_SHIFT    5
#define CFG_COMP_MODE_SHIFT 4
#define CFG_COMP_POL_SHIFT  3
#define CFG_COMP_LAT_SHIFT  2
#define CFG_COMP_QUE_SHIFT  0

#define FSR_4096    1
#define FSR_2048    2
#define CFG_OS_SINGLE_CONVERSION    0x80
#define CFG_MUX_AIN0_GND            0x40
#define CFG_PGA_RANGE_4096          (FSR_4096 << 1)
#define CFG_PGA_RANGE_2048          (FSR_2048 << 1)
#define CFG_MODE_SINGLE_SHOT        0x01
#define CFG_DATA_RATE_8SPS          0x00
#define CFG_COMP_MODE_TRADITION     0x00
#define CFG_COMP_POL_LOW            0x00
#define CFG_COMP_LAT_NO_LATCH       0x00
#define CFG_COMP_QUE_DISABLE        0x03

#define THERMISTOR_CFG_HIGH (CFG_OS_SINGLE_CONVERSION | CFG_MUX_AIN0_GND | CFG_PGA_RANGE_4096 \
        | CFG_MODE_SINGLE_SHOT)
#define THERMISTOR_CFG_LOW  (CFG_DATA_RATE_8SPS | CFG_COMP_MODE_TRADITION | CFG_COMP_POL_LOW \
        | CFG_COMP_LAT_NO_LATCH | CFG_COMP_QUE_DISABLE)

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
    writeBuf[1] = THERMISTOR_CFG_HIGH;  // This sets the 8 MSBs of the config register (bits
                                        // 15-8) to 11000101
    writeBuf[2] = THERMISTOR_CFG_LOW;   // This sets the 8 LSBs of the config register (bits
                                        // 7-0) to 00000011

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

float getRt(float Vin, float Vout, float R0, float* pVo)
{
#if 1
    float Vo = Vout * FSR[FSR_4096] / Vin;
    *pVo = Vo;
    return (R0 / ( 3.285 / Vo - 1.0));
#else
    float Vo = Vout * FSR[FSR_4096] / Vin;
    *pVo = Vo;
    return (R0 * ( 3.3 / Vo - 1.0));
#endif
}
/**
 * Calculate the temperature in Celsius
 * @param   Rt  The resistance value of the thermistor
 * @param   R0  The balance resistor value
 * @param   T0  The Room typical temperature
 * @param   B   The BETA value from the datasheet
 */
float getCelsius(float Rt, float R0, float T0, float B)
{
    return T0 * B / (T0 * log(Rt/R0) + B) - 273.15;
}

float getCelsius2(float Rt)
{
    const float A = 2.114990448e-3;
    const float B = 0.3832381228e-4;
    const float C = 5.228061052e-7;
    const float Beta = 3799.42;
    const float R25 = 100000;
    float logRt = log(Rt);
    float T;
    T = (1.0/ (A + Beta*logRt + C * logRt * logRt * logRt)); // Steinhart and Hart Equation.
                                                 // T  = 1 / {A + B[ln(R)] + C[ln(R)]^3}
    return T - 273.15;
}

float getFahrenheit(float tCelsius)
{
    return (tCelsius * 9.0)/5.0 + 32.0;
}

int main() {

    int ADS_address = 0x48;   // Address of our device on the I2C bus
    int I2CFile;
    float readVal = 0.0;
    float sumOfRead = 0.0;
    float Rt, tC, tF, Vo;
    const unsigned NumOfSample = 5;
    const float Vin = (float)0x7fff;
    const float R0 = 100000.0;  // 100K Ohms
    const float B = 3950.0;     // Beta Value
    const float T0 = 298.15;    // 25 Celsium in Kelvin


    I2CFile = open("/dev/i2c-1", O_RDWR);     // Open the I2C device

    ioctl(I2CFile, I2C_SLAVE, ADS_address);   // Specify the address of the I2C Slave to
                                            // communicate with

    for (;;) {
        sumOfRead = 0.0;
        for (int i=0; i<NumOfSample; i++)
        {
            sumOfRead += (float)readADC(I2CFile);
            usleep(DELAY2);
        }
        readVal = sumOfRead / (float)NumOfSample;
        Rt = getRt(Vin, readVal, R0, &Vo);
        //printf("Rt= %f\n", Rt);
        tC = getCelsius(Rt, R0, T0, B);
        //tC = getCelsius2(Rt);
        tF = getFahrenheit(tC);

        printf("\rVout = %4.3f(V), Rt = %f, Temperature: %f (C), %f (F)", Vo, Rt, tC, tF);
        fflush(stdout);
        usleep(DELAY);
    }

    close(I2CFile);

    return 0;

}
