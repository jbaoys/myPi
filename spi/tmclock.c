/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 * Raspberry Pi
 * Using /dev/spidev0.0 or /dev/spidev0.1
 *
 * Raspberry Pi                                 TM1638 LED&KEY
 * pin 4    5v Power <------------------------> VCC
 * pin 19   GPIO 10 (SPI0 MOSI) <-------------> DIO
 * pin 21   GPIO 9  (SPI0 MISO) <-------------> DIO
 * pin 23   GPIO 11 (SPIO SCLK) <-------------> CLK
 * pin 24   GPIO 8  (SPI0 CE0)  <-------------> STB  /dev/spidev0.0
 * pin 25   Ground <--------------------------> GND
 * pin 26   GPIO 7  (SPI0 CE1)   /dev/spidev0.1
 *
 * Function: a digital clock on TM1638
 * TM1638 is a LED driver control circuit with a keyboard scan interface,
 * the internal set Into a MCU digital interface, data latch, LED
 * high-voltage driver, keyboard scanning circuit.
 *
 * After running with argument "8", the linux system clock is displayed
 * on the middle 6 7-Segment LEDs
 *
 * Press and hold key "S1" will display year and date, releae it, go back
 * to clock mode.
 *
 * Press other keys, the top led will be "ON", release, it will be "OFF"
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <time.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static uint8_t reverseBits[256] = {
0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
};

struct font {
    char ch;
    uint8_t ft;
} tmFont[] = {
    { ' ', 0b00000000 },
    { '!', 0b10000110 },
    { '"', 0b00100010 },
    { '(', 0b00110000 },
    { ')', 0b00000110 },
    { ',', 0b00000100 },
    { '-', 0b01000000 },
    { '.', 0b10000000 },
    { '/', 0b01010010 },
    { '0', 0b00111111 },
    { '1', 0b00000110 },
    { '2', 0b01011011 },
    { '3', 0b01001111 },
    { '4', 0b01100110 },
    { '5', 0b01101101 },
    { '6', 0b01111101 },
    { '7', 0b00100111 },
    { '8', 0b01111111 },
    { '9', 0b01101111 },
    { '=', 0b01001000 },
    { '?', 0b01010011 },
    { '@', 0b01011111 },
    { 'A', 0b01110111 },
    { 'B', 0b01111111 },
    { 'C', 0b00111001 },
    { 'D', 0b00111111 },
    { 'E', 0b01111001 },
    { 'F', 0b01110001 },
    { 'G', 0b00111101 },
    { 'H', 0b01110110 },
    { 'I', 0b00000110 },
    { 'J', 0b00011111 },
    { 'K', 0b01101001 },
    { 'L', 0b00111000 },
    { 'M', 0b00010101 },
    { 'N', 0b00110111 },
    { 'O', 0b00111111 },
    { 'P', 0b01110011 },
    { 'Q', 0b01100111 },
    { 'R', 0b00110001 },
    { 'S', 0b01101101 },
    { 'T', 0b01111000 },
    { 'U', 0b00111110 },
    { 'V', 0b00101010 },
    { 'W', 0b00011101 },
    { 'X', 0b01110110 },
    { 'Y', 0b01101110 },
    { 'Z', 0b01011011 },
    { '[', 0b00111001 },
    { ']', 0b00001111 },
    { '_', 0b00001000 },
    { '`', 0b00100000 },
    { 'a', 0b01011111 },
    { 'b', 0b01111100 },
    { 'c', 0b01011000 },
    { 'd', 0b01011110 },
    { 'e', 0b01111011 },
    { 'f', 0b00110001 },
    { 'g', 0b01101111 },
    { 'h', 0b01110100 },
    { 'i', 0b00000100 },
    { 'j', 0b00001110 },
    { 'k', 0b01110101 },
    { 'l', 0b00110000 },
    { 'm', 0b01010101 },
    { 'n', 0b01010100 },
    { 'o', 0b01011100 },
    { 'p', 0b01110011 },
    { 'q', 0b01100111 },
    { 'r', 0b01010000 },
    { 's', 0b01101101 },
    { 't', 0b01111000 },
    { 'u', 0b00011100 },
    { 'v', 0b00101010 },
    { 'w', 0b00011101 },
    { 'x', 0b01110110 },
    { 'y', 0b01101110 },
    { 'z', 0b01000111 },
    { '{', 0b01000110 },
    { '|', 0b00000110 },
    { '}', 0b01110000 },
    { '~', 0b00000001 },
};

size_t convertStr(char* str, uint8_t *buf, uint8_t *keys)
{
    size_t k = 0;
    size_t fontSz = ARRAY_SIZE(tmFont);
    int i, j = 0;
    uint8_t s[8];
    for (i=0; i<4; ++i) {
        s[i] = keys[i] & 0x01;
        s[i+4] = (keys[i] >> 4) & 0x01;
    }
    while(str[j]) {
        buf[k] = tmFont[0].ft;
        for (i=0; i<fontSz; ++i) {
            if (tmFont[i].ch == str[j]) {
                buf[k] = tmFont[i].ft;
                buf[k+1] = s[k/2];
                break;
            }
        }
        if ((buf[k]==0b10000000) && (k>1)) {
            buf[k-2] |= buf[k];
            buf[k] = 0x00;
        } else {
            k+=2;
        }
        j++;
    }
    return k;
}

static void pabort(const char *s)
{
    perror(s);
    abort();
}

/*
 * GPIO10(SPI0 MOSI) <===> GPIO9(SPI0 MISO)
 */
static const char *device = "/dev/spidev0.0";
static uint8_t mode = SPI_MODE_3;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;
//static int lsb_setting = 0;

static void transfer2(int fd, uint8_t *send, size_t send_len,
                              uint8_t *recv, size_t recv_len)
{
    int ret;
    struct spi_ioc_transfer msgs[2] = { };
    unsigned nmsgs = 0;
    if (send && send_len != 0) {
        for (ret=0; ret < send_len; ret++) {
            send[ret] = reverseBits[send[ret]];
        }
        msgs[nmsgs].tx_buf = (unsigned long) send;
        msgs[nmsgs].rx_buf = 0;
        msgs[nmsgs].len = send_len;
        msgs[nmsgs].speed_hz = speed;
        msgs[nmsgs].delay_usecs = delay;
        msgs[nmsgs].bits_per_word = bits;
        //msgs[nmsgs].cs_change = 0;
        nmsgs++;
    }

    if (recv && recv_len != 0) {
        msgs[nmsgs].tx_buf = 0;
        msgs[nmsgs].rx_buf = (unsigned long) recv;
        msgs[nmsgs].len = recv_len;
        msgs[nmsgs].speed_hz = speed;
        msgs[nmsgs].delay_usecs = delay;
        msgs[nmsgs].bits_per_word = bits;
        //msgs[nmsgs].cs_change = 0;
        nmsgs++;
    }
    if (nmsgs) {
        ret = ioctl(fd, SPI_IOC_MESSAGE(nmsgs), &msgs);
        if (ret < 1)
            pabort("can't send/recv spi message");
        uint8_t *rx = (uint8_t*)recv;
        for (ret = 0; ret < recv_len; ret++) {
            if (!(ret % 6))
                puts("");
            printf("%.2X ", rx[ret]);
        }
        puts("");
    }
}

static void transfer(int fd, uint8_t *buf, size_t sz)
{
    int ret;
    uint8_t *rx = malloc(sizeof(uint8_t)*sz);
    uint8_t *txr = malloc(sizeof(uint8_t)*sz);
    for (ret=0; ret < sz; ret++) {
        txr[ret] = reverseBits[buf[ret]];
    }
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)txr,
        .rx_buf = (unsigned long)rx,
        .len = sz,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");

    free(rx);
    free(txr);
}
static void getbuttons(int fd, uint8_t *rx)
{
    int ret;
    const ssize_t sz = 5;
    uint8_t tx[] = {
        // for output to high since it connect to the input
        0x42, 0xff, 0xff, 0xff, 0xff
    };
    tx[0] = reverseBits[tx[0]];
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = sz,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
        pabort("can't send spi message");

    for (ret = 0; ret < sz; ret++) {
        /*
        if (!(ret % 6))
            puts("");
        printf("%.2X ", reverseBits[rx[ret]]);
        */
        rx[ret] = reverseBits[rx[ret]];
    }
    //puts("");
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int fd;
    int pulse = 0;
    if (argc>1) pulse = atoi(argv[1]);

    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

#if 0
    ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_setting);
    if (ret == -1)
        pabort("can't set spi lsb mode");
#endif
    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");
/*
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");
        */

    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    uint8_t tx_cmd[] = {
        0x40,0x00,0x00,0x00,0x00
    };
    uint8_t tx_data[32];
    uint8_t rx_data[32];
    char str[32];
    size_t ii, dsz = 17;
    time_t T;
    struct tm tm;
    tx_data[0] = 0xC0;
    while(1) {
        T = time(NULL);
        tm = *localtime(&T);
        if (rx_data[1] & 0x0f) {
            snprintf(str,32, "%04d.%02d.%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
        } else {
            snprintf(str,32, " %02d.%02d.%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
        }
        ii = convertStr(str, &tx_data[1], &rx_data[1]) + 1;
        for (; ii<dsz; ++ii) {
            tx_data[ii]=0x00;
        }
        tx_cmd[0]=0x40;
        transfer(fd, tx_cmd, 1);
        transfer(fd, tx_data, dsz);
        tx_cmd[0]=0x80 + (uint8_t)pulse;
        transfer(fd, tx_cmd, 1);
        //tx_cmd[0]=0x42;
        //transfer2(fd, tx_cmd, 1, rx_data, 4);
        getbuttons(fd, rx_data);
        sleep(0.1);
    }
    close(fd);
    return ret;
}
