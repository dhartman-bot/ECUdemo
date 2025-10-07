#ifndef IMPLEMENT_H
#define IMPLEMENT_H

#include <stdbool.h>
#include <stdint.h>

// Types of implements that can be attached
typedef enum {
    IMPLEMENT_NONE,
    IMPLEMENT_PLANTER,
    IMPLEMENT_SPRAYER,
    IMPLEMENT_BALER,
    IMPLEMENT_CULTIVATOR,
    IMPLEMENT_MOWER
} ImplementType;

typedef enum {
    IMPLEMENT_IDLE,
    IMPLEMENT_WORKING,
    IMPLEMENT_RAISED,
    IMPLEMENT_ERROR
} ImplementStatus;

typedef struct {
    ImplementType type;
    ImplementStatus status;
    float working_depth_cm;      // Current working depth
    float target_depth_cm;       // Target working depth
    float working_width_m;       // Width of implement
    float pressure_bar;          // Hydraulic pressure
    float flow_lpm;              // Hydraulic flow (liters per minute)
    bool auto_depth_control;     // Automatic depth adjustment
    int rows_or_sections;        // Number of rows (planter) or sections (sprayer)
    float coverage_rate_ha_hr;   // Coverage rate in hectares per hour
} ImplementState;

// Implement control functions
void implement_init(void);
void implement_attach(ImplementType type);
void implement_detach(void);
void implement_lower(void);
void implement_raise(void);
void implement_set_depth(float depth_cm);
void implement_update(void);
ImplementState* implement_get_state(void);

#endif // IMPLEMENT_H
