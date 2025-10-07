#include "hydraulics.h"
#include "../engine/engine_control.h"
#include "../canbus/canbus.h"
#include "../diagnostics/diagnostics.h"
#include <stdio.h>

static HydraulicsState hydraulics_state = {0};

void hydraulics_init(void) {
    printf("[HYDRAULICS] Initializing hydraulics control module\n");
    hydraulics_state.system_pressure = 0.0;
    hydraulics_state.flow_rate = 0.0;
    hydraulics_state.reservoir_level = 85.0;
    hydraulics_state.oil_temp = 20.0;
    hydraulics_state.pto_speed = 0;
    hydraulics_state.pto_engaged = false;
    hydraulics_state.implement_raised = false;
    hydraulics_state.status = STATUS_OK;
}

void hydraulics_update(void) {
    // Get engine state to determine pump speed
    EngineState* engine = engine_get_state();

    if (engine->engine_running) {
        // Hydraulic pump driven by engine
        float pump_speed_factor = engine->current_rpm / 2600.0;
        hydraulics_state.system_pressure = pump_speed_factor * 3000.0; // Max 3000 PSI
        hydraulics_state.flow_rate = pump_speed_factor * 25.0; // Max 25 GPM

        // Oil heats up with use
        if (hydraulics_state.pto_engaged || hydraulics_state.implement_raised) {
            hydraulics_state.oil_temp += 0.3;
        }
    } else {
        hydraulics_state.system_pressure = 0.0;
        hydraulics_state.flow_rate = 0.0;
    }

    // Send hydraulics data to CAN bus
    canbus_send_message(0x200, (uint8_t*)&hydraulics_state.system_pressure, 4);

    // Check for fault conditions
    SystemStatus health = hydraulics_check_health();
    if (health != STATUS_OK) {
        diagnostics_report_fault(SPN_HYDRAULIC_PRESSURE, FMI_DATA_BELOW_NORMAL, "Hydraulics", "Hydraulic system pressure below normal operating range");
    }
}

void hydraulics_raise_implement(void) {
    printf("[HYDRAULICS] Raising implement\n");
    hydraulics_state.implement_raised = true;
}

void hydraulics_lower_implement(void) {
    printf("[HYDRAULICS] Lowering implement\n");
    hydraulics_state.implement_raised = false;
}

void hydraulics_engage_pto(uint8_t speed_percent) {
    if (speed_percent > 100) speed_percent = 100;
    printf("[HYDRAULICS] Engaging PTO at %d%%\n", speed_percent);
    hydraulics_state.pto_engaged = true;
    hydraulics_state.pto_speed = speed_percent;
}

void hydraulics_disengage_pto(void) {
    printf("[HYDRAULICS] Disengaging PTO\n");
    hydraulics_state.pto_engaged = false;
    hydraulics_state.pto_speed = 0;
}

HydraulicsState* hydraulics_get_state(void) {
    return &hydraulics_state;
}

SystemStatus hydraulics_check_health(void) {
    if (hydraulics_state.reservoir_level < 20.0) {
        hydraulics_state.status = STATUS_CRITICAL;
        return STATUS_CRITICAL;
    }
    if (hydraulics_state.oil_temp > 90.0) {
        hydraulics_state.status = STATUS_WARNING;
        return STATUS_WARNING;
    }
    hydraulics_state.status = STATUS_OK;
    return STATUS_OK;
}
