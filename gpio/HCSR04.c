/**
 * Using wiringPi core functions to measure a distance via a HC-SR04
 */
#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#define TRIG 0
#define ECHO 1
#define LED  2
#define DELAY 150000
#define DELAY2 1

double distance()
{
    // send a 10us pulse to TRIG
    digitalWrite (TRIG, HIGH);
    usleep(10);
    digitalWrite (TRIG, LOW);

    clock_t start, end;

    while ( digitalRead(ECHO) == 0 )
        start = clock();

    while ( digitalRead(ECHO) == 1 )
        end = clock();

    double timeElapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    return (timeElapsed * 34300) / (double)2;
}

int main (void)
{
    if (wiringPiSetup () == -1)
        return 1;

    pinMode (TRIG, OUTPUT);
    pinMode (ECHO, INPUT);
    pinMode (LED, OUTPUT);

    int count = 0;
    for (;;)
    {
        double dist = distance();
        if (dist < 40)
        {
            count = DELAY2;
            digitalWrite (LED, HIGH);
        }
        else
        {
            if (count)
            {
                --count;
                if (!count)
                    digitalWrite (LED, LOW);
            }
        }
        printf("Measured Distance = %.2lf cm\n", dist);
        usleep(DELAY);
    }
    return 0;
}
