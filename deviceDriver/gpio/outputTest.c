/*
* Name : output_test.c
* Author : Vu Nguyen <quangngmetro@gmail.com>
* Version : 0.1
* Copyright : GPL
* Description : This is a test application which is used for testing
* GPIO output functionality of the raspi-gpio Linux device driver
* implemented for Raspberry Pi revision B platform. The test
* application first sets all the GPIO pins on the Raspberry Pi to
* output, then it sets all the GPIO pins to "high"/"low" logic
* level based on the options passed to the program from the command
* line
* Usage example:
* ./output_test 1 // Set all GPIO pins to output, high state
* ./output_test 0 // Set all GPIO pins to output, low state
* 
J8:
   3V3  (1) (2)  5V    
 GPIO2  (3) (4)  5V    
 GPIO3  (5) (6)  GND   
 GPIO4  (7) (8)  GPIO14
   GND  (9) (10) GPIO15
GPIO17 (11) (12) GPIO18
GPIO27 (13) (14) GND   
GPIO22 (15) (16) GPIO23
   3V3 (17) (18) GPIO24
GPIO10 (19) (20) GND   
 GPIO9 (21) (22) GPIO25
GPIO11 (23) (24) GPIO8 
   GND (25) (26) GPIO7 
 GPIO0 (27) (28) GPIO1 
 GPIO5 (29) (30) GND   
 GPIO6 (31) (32) GPIO12
GPIO13 (33) (34) GND   
GPIO19 (35) (36) GPIO16
GPIO26 (37) (38) GPIO20
   GND (39) (40) GPIO21

*
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
int main(int argc, char **argv)
{
    int i = 0, value;
    int fd;
    char path[PATH_SIZE];
    char buf[BUF_SIZE];
    if (argc != 3) {
        printf("Usage: a.out <gpio_pin(3,4,etc.)> <0|1>\n");
        exit(EXIT_FAILURE);
    }
    // Open all GPIO pins
    i = atoi(argv[1]);
    if (i != 0 && i != 1 && i != 5 && i != 6 &&
    i != 12 && i != 13 && i != 16 && i != 19 &&
    i != 20 && i != 21 && i != 26) {
        snprintf(path, sizeof(path), "/dev/raspiGpio%d", i);
        fd = open(path, O_WRONLY);
        if (fd < 0) {
            perror("Error opening GPIO pin");
            exit(EXIT_FAILURE);
        } else {
            printf("successfully open %s\n", path);
        }
    } else {
        printf("Wrong GPIO PIN Number - %d\n", i);
        exit(EXIT_FAILURE);
    }
    // Set directiion of GPIO pins to output
    printf("Set GPIO pins to output, logic level :%s\n", argv[2]);
    strncpy(buf, "out", 3);
    buf[3] = '\0';
    if (write(fd, buf, sizeof(buf)) < 0) {
        perror("write, set pin output");
        exit(EXIT_FAILURE);
    } else {
        printf("Set %s as %s\n", path, buf);
    }
    // Set logic state of GPIO pins low/high
    value = atoi(argv[2]);
    if (value == 1) {
        strncpy(buf, "1", 1);
        buf[1] = '\0';
    } else if (value == 0) {
        strncpy(buf, "0", 1);
        buf[1] = '\0';
    } else {
        printf("Invalid logic value\n");
        exit(EXIT_FAILURE);
    }
    if (write(fd, buf, sizeof(buf)) < 0) {
        perror("write, set GPIO state of GPIO pins");
        exit(EXIT_FAILURE);
    } else {
        printf("Write %s to %s\n", buf, path);
    }
    close(fd);
    return EXIT_SUCCESS;
}
