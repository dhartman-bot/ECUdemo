#include "transmission.h"
#include "../engine/engine_control.h"
#include "../canbus/canbus.h"
#include "../diagnostics/diagnostics.h"
#include <stdio.h>

static TransmissionState transmission_state = {0};
static const float gear_ratios[] = {0.0, 0.0, 3.5, 2.2, 1.5, 1.0, -4.0};

void transmission_init(void) {
    printf("[TRANSMISSION] Initializing transmission control module\n");
    transmission_state.current_gear = GEAR_PARK;
    transmission_state.clutch_position = 0.0;
    transmission_state.output_speed = 0.0;
    transmission_state.transmission_temp = 20.0;
    transmission_state.oil_pressure = 50.0;
    transmission_state.clutch_engaged = false;
    transmission_state.status = STATUS_OK;
}

void transmission_update(void) {
    // Get engine state
    EngineState* engine = engine_get_state();

    if (engine->engine_running && transmission_state.clutch_engaged) {
        // Calculate output speed based on gear ratio
        float ratio = gear_ratios[transmission_state.current_gear];
        if (ratio != 0.0) {
            transmission_state.output_speed = engine->current_rpm / ratio;
        } else {
            transmission_state.output_speed = 0.0;
        }

        // Temperature increases with use
        transmission_state.transmission_temp += 0.2;
    } else {
        transmission_state.output_speed = 0.0;
    }

    // Send speed data to CAN bus
    canbus_send_message(0x300, (uint8_t*)&transmission_state.output_speed, 4);

    // Check for fault conditions
    SystemStatus health = transmission_check_health();
    if (health != STATUS_OK) {
        diagnostics_report_fault(0x03, "Transmission", "High temperature or low pressure");
    }
}

void transmission_shift_gear(GearPosition gear) {
    printf("[TRANSMISSION] Shifting to gear: %d\n", gear);
    transmission_state.current_gear = gear;
}

void transmission_engage_clutch(void) {
    printf("[TRANSMISSION] Engaging clutch\n");
    transmission_state.clutch_engaged = true;
    transmission_state.clutch_position = 100.0;
}

void transmission_disengage_clutch(void) {
    printf("[TRANSMISSION] Disengaging clutch\n");
    transmission_state.clutch_engaged = false;
    transmission_state.clutch_position = 0.0;
}

TransmissionState* transmission_get_state(void) {
    return &transmission_state;
}

SystemStatus transmission_check_health(void) {
    if (transmission_state.transmission_temp > 120.0) {
        transmission_state.status = STATUS_CRITICAL;
        return STATUS_CRITICAL;
    }
    if (transmission_state.oil_pressure < 25.0) {
        transmission_state.status = STATUS_WARNING;
        return STATUS_WARNING;
    }
    transmission_state.status = STATUS_OK;
    return STATUS_OK;
}
