/*
* Name : input_test.c
* Author : Vu Nguyen <quangngmetro@gmail.com>
* Version : 0.1
* Copyright : GPL
* Description : This is a test application which is used for testing
* GPIO input functionality of the raspi-gpio Linux device driver
* implemented for Raspberry Pi revision B platform. The test
* application first sets all the GPIO pins on the Raspberry Pi to
* input, then it reads all the GPIO pins logic level and print these
* value to the terminal.
*/
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define NUM_GPIO_PINS 21
#define MAX_GPIO_NUMBER 32
#define BUF_SIZE 4
#define PATH_SIZE 20

/**
 * Open raspiGpio device
 * @param   n       GPIO number, obtained by "pinout"
 * @param   mode    device operation mode: 0 : output, 1: input
 * @return file handler number.
 */
int openGpioDev(int n, int mode) {
    int fdn;
    char buff[BUF_SIZE];
    char path[PATH_SIZE];
    if (n != 0 && n != 1 && n != 5 && n != 6 &&
        n != 12 && n != 13 && n != 16 && n != 19 &&
        n != 20 && n != 21 && n != 26) {
        snprintf(path, sizeof(path), "/dev/raspiGpio%d", n);
        fdn = open(path, (mode==0) ? O_WRONLY : O_RDWR);
        if (fdn < 0) {
            printf("Error opening GPIO pin %d dev = %s\n", n, path);
            exit(EXIT_FAILURE);
        }
        if (mode==0) {
            strncpy(buff, "out", 3);
            buff[3] = '\0';
        } else {
            strncpy(buff, "in", 2);
            buff[2] = '\0';
        }
        printf("Set pin %s to %s\n", path, buff);
        if (write(fdn, buff, sizeof(buff)) < 0) {
            printf("write %s failed with dev %s\n", buff, path);
            exit(EXIT_FAILURE);
        } else {
            printf("Set pin %d as %s\n", n, buff);
        }
    } else {
        printf("Wrong INPUT GPIO Pin number - %d\n", n);
        exit(EXIT_FAILURE);
    }
    return fdn;
}

int main(int argc, char **argv)
{
    int i = 0, o1, o2, count=100, maxPress = 16;
    int fdi, fdo1, fdo2;
    char buf[BUF_SIZE];
    char readBuf[2];
    char led = 0;
    if (argc != 4) {
        perror("Usage: inputtest <input_gpio_pin> <output_gpio_1> <output_gpio_2>\n");
        exit(EXIT_FAILURE);
    }

    i = atoi(argv[1]);
    o1 = atoi(argv[2]);
    o2 = atoi(argv[3]);
    // Open all GPIO pins
    fdi = openGpioDev(i, 1);
    fdo1 = openGpioDev(o1, 0);
    fdo2 = openGpioDev(o2, 0);
    buf[1] = '\0';

    // Read logic level of GPIO pins and display them to the terminal
    while(maxPress>0) {
        if (read(fdi, readBuf, 1) < 1) {
            printf("read fail on pin %d\n", i);
            exit(EXIT_FAILURE);
        }
        //readBuf[1] = '\0';
        //printf("GPIO pin: %d Logic level: %s\n", i, readBuf);
        if (readBuf[0] == '0') {
            // key pressed
            if (count-- <= 0) {
                // debounded
                led++;
                buf[0] = led & 0x01 ? '1' : '0';
                if (write(fdo1, buf, sizeof(buf)) < 0) {
                    perror("write, set GPIO state of GPIO pins");
                    exit(EXIT_FAILURE);
                }
                buf[0] = led & 0x02 ? '1' : '0';
                if (write(fdo2, buf, sizeof(buf)) < 0) {
                    perror("write, set GPIO state of GPIO pins");
                    exit(EXIT_FAILURE);
                }
                count = 100;
                maxPress--;
            }
        } else {
            count = 100;
        }
        usleep(1000);
    }
    close(fdi);
    close(fdo1);
    close(fdo2);
    return EXIT_SUCCESS;
}
