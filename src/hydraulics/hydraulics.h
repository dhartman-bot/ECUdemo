#ifndef HYDRAULICS_H
#define HYDRAULICS_H

#include "../common/types.h"

// Hydraulics control module - manages implements, loaders, PTO
typedef struct {
    float system_pressure;    // PSI
    float flow_rate;          // GPM
    float reservoir_level;    // Percentage
    float oil_temp;           // Celsius
    uint8_t pto_speed;        // RPM percentage
    bool pto_engaged;
    bool implement_raised;
    SystemStatus status;
} HydraulicsState;

// Dependencies: Engine (needs RPM for pump speed), CANBus (send hydraulic data)
void hydraulics_init(void);
void hydraulics_update(void);
void hydraulics_raise_implement(void);
void hydraulics_lower_implement(void);
void hydraulics_engage_pto(uint8_t speed_percent);
void hydraulics_disengage_pto(void);
HydraulicsState* hydraulics_get_state(void);
SystemStatus hydraulics_check_health(void);

#endif // HYDRAULICS_H
