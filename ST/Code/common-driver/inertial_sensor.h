#ifndef INERTIAL_SENSOR_H
#define INERTIAL_SENSOR_H

extern double roll;
extern double pitch;

void convert_xyz_to_roll_pitch(void);
void read_full_xyz(void);
void Init_Inertial_Sensor(void);

#endif

