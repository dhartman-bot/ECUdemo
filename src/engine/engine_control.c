#include "engine_control.h"
#include "../canbus/canbus.h"
#include "../diagnostics/diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static EngineState engine_state = {0};

void engine_init(void) {
    printf("[ENGINE] Initializing engine control module\n");
    engine_state.current_rpm = 0;
    engine_state.target_rpm = 0;
    engine_state.fuel_rate = 0.0;
    engine_state.oil_pressure = 45.0;
    engine_state.coolant_temp = 20.0;
    engine_state.engine_running = false;
    engine_state.status = STATUS_OK;
}

void engine_update(void) {
    if (!engine_state.engine_running) {
        return;
    }

    // Simulate RPM control
    if (engine_state.current_rpm < engine_state.target_rpm) {
        engine_state.current_rpm += 50;
    } else if (engine_state.current_rpm > engine_state.target_rpm) {
        engine_state.current_rpm -= 50;
    }

    // Simulate fuel consumption based on RPM
    engine_state.fuel_rate = (engine_state.current_rpm / 2200.0) * 15.0;

    // Simulate temperature increase
    if (engine_state.current_rpm > 1000) {
        engine_state.coolant_temp += 0.5;
    }

    // Send data to CAN bus
    canbus_send_message(0x100, (uint8_t*)&engine_state.current_rpm, 2);

    // Check for fault conditions
    SystemStatus health = engine_check_health();
    if (health != STATUS_OK) {
        diagnostics_report_fault(SPN_ENGINE_COOLANT_TEMP, FMI_DATA_ABOVE_NORMAL, "Engine", "Engine coolant temperature extremely high");
    }
}

void engine_set_throttle(uint8_t throttle_percent) {
    if (throttle_percent > 100) throttle_percent = 100;
    engine_state.target_rpm = 800 + (throttle_percent * 18); // Idle at 800, max at 2600
    printf("[ENGINE] Throttle set to %d%%, target RPM: %d\n", throttle_percent, engine_state.target_rpm);
}

void engine_start(void) {
    printf("[ENGINE] Starting engine\n");
    engine_state.engine_running = true;
    engine_state.target_rpm = 800; // Idle RPM
}

void engine_stop(void) {
    printf("[ENGINE] Stopping engine\n");
    engine_state.engine_running = false;
    engine_state.current_rpm = 0;
    engine_state.target_rpm = 0;
}

EngineState* engine_get_state(void) {
    return &engine_state;
}

SystemStatus engine_check_health(void) {
    if (engine_state.coolant_temp > 105.0) {
        engine_state.status = STATUS_CRITICAL;
        return STATUS_CRITICAL;
    }
    if (engine_state.oil_pressure < 20.0) {
        engine_state.status = STATUS_WARNING;
        return STATUS_WARNING;
    }
    engine_state.status = STATUS_OK;
    return STATUS_OK;
}
