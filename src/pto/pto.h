#ifndef PTO_H
#define PTO_H

#include <stdbool.h>
#include <stdint.h>

// PTO standard speeds for agricultural equipment
typedef enum {
    PTO_SPEED_540 = 540,   // Standard PTO speed (rpm)
    PTO_SPEED_1000 = 1000  // High-speed PTO (rpm)
} PTOSpeed;

typedef enum {
    PTO_DISENGAGED,
    PTO_ENGAGING,
    PTO_ENGAGED,
    PTO_ERROR
} PTOStatus;

typedef struct {
    PTOStatus status;
    PTOSpeed target_speed;
    int current_rpm;
    float load_percent;      // Load on PTO (0-100%)
    float torque_nm;         // Torque in Newton-meters
    bool overload_detected;
    float slip_percent;      // Clutch slip
} PTOState;

// PTO control functions
void pto_init(void);
void pto_engage(PTOSpeed speed);
void pto_disengage(void);
void pto_update(void);
PTOState* pto_get_state(void);

#endif // PTO_H
