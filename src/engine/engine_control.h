#ifndef ENGINE_CONTROL_H
#define ENGINE_CONTROL_H

#include "../common/types.h"

// Engine control module - manages RPM, fuel injection, timing
typedef struct {
    uint16_t current_rpm;
    uint16_t target_rpm;
    float fuel_rate;          // L/hr
    float oil_pressure;       // PSI
    float coolant_temp;       // Celsius
    bool engine_running;
    SystemStatus status;
} EngineState;

// Dependencies: CANBus (send RPM data), Diagnostics (report faults)
void engine_init(void);
void engine_update(void);
void engine_set_throttle(uint8_t throttle_percent);
void engine_start(void);
void engine_stop(void);
EngineState* engine_get_state(void);
SystemStatus engine_check_health(void);

#endif // ENGINE_CONTROL_H
