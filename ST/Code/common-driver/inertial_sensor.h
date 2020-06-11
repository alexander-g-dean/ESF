#ifndef INERTIAL_SENSOR_H
#define INERTIAL_SENSOR_H

extern float g_roll, g_pitch;
extern int32_t g_x, g_y, g_z;

void convert_xyz_to_roll_pitch(void);
void read_full_xyz(void);
void Init_Inertial_Sensor(void);

#endif

