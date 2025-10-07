#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include "../common/types.h"

// Transmission control module - manages gear selection, clutch, speed
typedef enum {
    GEAR_PARK = 0,
    GEAR_NEUTRAL = 1,
    GEAR_DRIVE_1 = 2,
    GEAR_DRIVE_2 = 3,
    GEAR_DRIVE_3 = 4,
    GEAR_DRIVE_4 = 5,
    GEAR_REVERSE = 6
} GearPosition;

typedef struct {
    GearPosition current_gear;
    float clutch_position;     // 0-100%
    float output_speed;        // RPM
    float transmission_temp;   // Celsius
    float oil_pressure;        // PSI
    bool clutch_engaged;
    SystemStatus status;
} TransmissionState;

// Dependencies: Engine (needs RPM), CANBus (send speed data), Diagnostics
void transmission_init(void);
void transmission_update(void);
void transmission_shift_gear(GearPosition gear);
void transmission_engage_clutch(void);
void transmission_disengage_clutch(void);
TransmissionState* transmission_get_state(void);
SystemStatus transmission_check_health(void);

#endif // TRANSMISSION_H
