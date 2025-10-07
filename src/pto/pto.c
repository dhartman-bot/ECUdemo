#include "pto.h"
#include "../engine/engine_control.h"
#include "../canbus/canbus.h"
#include "../diagnostics/diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static PTOState pto_state = {
    .status = PTO_DISENGAGED,
    .target_speed = PTO_SPEED_540,
    .current_rpm = 0,
    .load_percent = 0.0,
    .torque_nm = 0.0,
    .overload_detected = false,
    .slip_percent = 0.0
};

void pto_init(void) {
    printf("[PTO] Initializing PTO module\n");
    pto_state.status = PTO_DISENGAGED;
    pto_state.current_rpm = 0;
    pto_state.load_percent = 0.0;
}

void pto_engage(PTOSpeed speed) {
    EngineState* engine = engine_get_state();

    if (engine->current_rpm < 800) {
        printf("[PTO] Cannot engage - engine RPM too low\n");
        diagnostics_report_fault(SPN_PTO_ENGAGEMENT, FMI_MECHANICAL_FAULT, "PTO", "PTO engagement failed - engine RPM below minimum threshold");
        return;
    }

    pto_state.status = PTO_ENGAGING;
    pto_state.target_speed = speed;
    printf("[PTO] Engaging PTO at %d RPM target\n", speed);

    // Send CAN message
    uint8_t data[8] = {0x01, (uint8_t)(speed >> 8), (uint8_t)(speed & 0xFF), 0, 0, 0, 0, 0};
    canbus_send_message(0x220, data, 8);
}

void pto_disengage(void) {
    printf("[PTO] Disengaging PTO\n");
    pto_state.status = PTO_DISENGAGED;
    pto_state.current_rpm = 0;

    // Send CAN message
    uint8_t data[8] = {0x00, 0, 0, 0, 0, 0, 0, 0};
    canbus_send_message(0x220, data, 8);
}

void pto_update(void) {
    EngineState* engine = engine_get_state();

    if (pto_state.status == PTO_ENGAGING) {
        // Simulate PTO spin-up
        if (pto_state.current_rpm < pto_state.target_speed) {
            pto_state.current_rpm += 50; // Gradual engagement
            pto_state.slip_percent = ((float)(pto_state.target_speed - pto_state.current_rpm) /
                                     pto_state.target_speed) * 100.0;
        } else {
            pto_state.status = PTO_ENGAGED;
            pto_state.slip_percent = 0.0;
            printf("[PTO] PTO fully engaged at %d RPM\n", pto_state.current_rpm);
        }
    }

    if (pto_state.status == PTO_ENGAGED) {
        // PTO speed should track engine RPM ratio
        float engine_ratio = (float)engine->current_rpm / 2100.0; // 2100 is nominal engine RPM
        pto_state.current_rpm = (int)(pto_state.target_speed * engine_ratio);

        // Simulate load based on implement work
        pto_state.load_percent = 45.0 + (rand() % 30); // 45-75% load
        pto_state.torque_nm = (pto_state.load_percent / 100.0) * 850.0; // Max 850 Nm

        // Check for overload
        if (pto_state.load_percent > 90.0) {
            pto_state.overload_detected = true;
            diagnostics_report_fault(SPN_PTO_SHAFT_SPEED, FMI_DATA_ABOVE_NORMAL, "PTO", "PTO overload detected - shaft load exceeds maximum rating");
            pto_state.status = PTO_ERROR;
        }

        // Send telemetry via CAN
        uint8_t data[8] = {
            (uint8_t)(pto_state.current_rpm >> 8),
            (uint8_t)(pto_state.current_rpm & 0xFF),
            (uint8_t)pto_state.load_percent,
            (uint8_t)(pto_state.torque_nm / 10),
            0, 0, 0, 0
        };
        canbus_send_message(0x221, data, 8);
    }
}

PTOState* pto_get_state(void) {
    return &pto_state;
}
