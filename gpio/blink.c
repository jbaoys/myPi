/**
 * Connect pin 11 (wPi# 0) to an LED that connects to a pull down resister
 * at the other end.
 *
 * using gcc -Wall -o blink blink.c -lwiringPi
 */
#include <wiringPi.h>
#define LED 0   // pin 11
int main (void)
{
    if (wiringPiSetup () == -1)
        return 1;
    pinMode (LED, OUTPUT);
    for (;;)
    {
        digitalWrite (LED, HIGH); delay (500);
        digitalWrite (LED, LOW);  delay (500);
    }
    return 0;
}
