/**
 * Using wiringPi core functions to measure a distance via a HC-SR04
 */
#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#define TRIG 0
#define ECHO 1
#define LED  2
#define DELAY 500000
#define DELAY2 1


static clock_t start, end;
static unsigned triggered = 0;
static unsigned measured = 0;
static double timeElapsed, dist;

void trigger()
{
    // send a 10us pulse to TRIG
    digitalWrite (TRIG, HIGH);
    usleep(10);
    digitalWrite (TRIG, LOW);

    start = end;
    triggered++;
}

void MEAS_INT(void)
{
    if ( digitalRead(ECHO) == 0 )
    {
        measured++;
        if (measured == triggered)
        {
            end = clock();
            if (end > start)
            {
                timeElapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
                dist = (timeElapsed * 34300) / (double)2;
            }
        }
    }
    else
    {
        start = clock();
    }
}

int main (void)
{
    if (wiringPiSetup () == -1)
        return 1;

    pinMode (TRIG, OUTPUT);
    pinMode (ECHO, INPUT);
    pinMode (LED, OUTPUT);
    wiringPiISR(ECHO, INT_EDGE_BOTH, &MEAS_INT);
    triggered = measured = 0;

    //int count = 0;
    for (;;)
    {
        trigger();
        while (measured < triggered)
        {
        }

        if (dist < 40)
        {
            //count = DELAY2;
            digitalWrite (LED, HIGH);
        }
        else
        {
            digitalWrite (LED, LOW);
            /*
            if (count)
            {
                --count;
                if (!count)
                    digitalWrite (LED, LOW);
            }
            */
        }
        printf("Measured Distance = %.2lf cm\n", dist);
        usleep(DELAY);
    }
    return 0;
}
