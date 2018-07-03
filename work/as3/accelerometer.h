// This module provides the interface to receive
// measurements from the accelerometer

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

typedef struct {
	short x;
	short y;
	short z;
} AccelerometerOutput_t;

void Accelerometer_init();
void Accelerometer_shutdown();

AccelerometerOutput_t Accelerometer_getReadings();

#endif
