#include "implement.h"
#include "../hydraulics/hydraulics.h"
#include "../pto/pto.h"
#include "../canbus/canbus.h"
#include "../diagnostics/diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static ImplementState impl_state = {
    .type = IMPLEMENT_NONE,
    .status = IMPLEMENT_IDLE,
    .working_depth_cm = 0.0,
    .target_depth_cm = 10.0,
    .working_width_m = 0.0,
    .pressure_bar = 0.0,
    .flow_lpm = 0.0,
    .auto_depth_control = true,
    .rows_or_sections = 0,
    .coverage_rate_ha_hr = 0.0
};

static const char* implement_type_names[] = {
    "None",
    "Planter",
    "Sprayer",
    "Baler",
    "Cultivator",
    "Mower"
};

void implement_init(void) {
    printf("[IMPLEMENT] Initializing implement control module\n");
    impl_state.type = IMPLEMENT_NONE;
    impl_state.status = IMPLEMENT_IDLE;
    impl_state.working_depth_cm = 0.0;
}

void implement_attach(ImplementType type) {
    impl_state.type = type;
    impl_state.status = IMPLEMENT_RAISED;

    printf("[IMPLEMENT] Attaching %s\n", implement_type_names[type]);

    // Configure implement-specific parameters
    switch (type) {
        case IMPLEMENT_PLANTER:
            impl_state.working_width_m = 12.0;  // 12-meter planter
            impl_state.rows_or_sections = 24;    // 24 rows
            impl_state.target_depth_cm = 5.0;    // 5 cm seed depth
            printf("[IMPLEMENT] 24-row planter configured (12m width)\n");
            break;

        case IMPLEMENT_SPRAYER:
            impl_state.working_width_m = 18.0;   // 18-meter boom
            impl_state.rows_or_sections = 36;    // 36 nozzle sections
            impl_state.target_depth_cm = 0.0;    // No depth for sprayer
            printf("[IMPLEMENT] Boom sprayer configured (18m width, 36 sections)\n");
            break;

        case IMPLEMENT_BALER:
            impl_state.working_width_m = 2.3;    // 2.3-meter pickup width
            impl_state.rows_or_sections = 1;
            impl_state.target_depth_cm = 0.0;
            printf("[IMPLEMENT] Round baler configured (2.3m pickup)\n");
            break;

        case IMPLEMENT_CULTIVATOR:
            impl_state.working_width_m = 9.0;    // 9-meter cultivator
            impl_state.rows_or_sections = 45;    // 45 shanks
            impl_state.target_depth_cm = 15.0;   // 15 cm working depth
            printf("[IMPLEMENT] Field cultivator configured (9m width, 45 shanks)\n");
            break;

        case IMPLEMENT_MOWER:
            impl_state.working_width_m = 7.5;    // 7.5-meter mower
            impl_state.rows_or_sections = 3;     // 3 sections
            impl_state.target_depth_cm = 8.0;    // 8 cm cutting height
            printf("[IMPLEMENT] Mower conditioner configured (7.5m width)\n");
            break;

        default:
            break;
    }

    // Send CAN message
    uint8_t data[8] = {0x01, (uint8_t)type, (uint8_t)impl_state.rows_or_sections, 0, 0, 0, 0, 0};
    canbus_send_message(0x240, data, 8);
}

void implement_detach(void) {
    printf("[IMPLEMENT] Detaching %s\n", implement_type_names[impl_state.type]);
    impl_state.type = IMPLEMENT_NONE;
    impl_state.status = IMPLEMENT_IDLE;
    impl_state.working_depth_cm = 0.0;
    impl_state.working_width_m = 0.0;
}

void implement_lower(void) {
    if (impl_state.type == IMPLEMENT_NONE) {
        printf("[IMPLEMENT] No implement attached\n");
        return;
    }

    HydraulicsState* hyd = hydraulics_get_state();
    if (hyd->system_pressure < 100.0) {
        printf("[IMPLEMENT] Cannot lower - insufficient hydraulic pressure\n");
        diagnostics_report_fault(3001, "Implement", "Implement lowering failed - low pressure");
        return;
    }

    printf("[IMPLEMENT] Lowering %s to working position\n", implement_type_names[impl_state.type]);
    impl_state.status = IMPLEMENT_WORKING;
    impl_state.working_depth_cm = impl_state.target_depth_cm;

    // Send CAN message
    uint8_t data[8] = {0x01, (uint8_t)impl_state.working_depth_cm, 0, 0, 0, 0, 0, 0};
    canbus_send_message(0x241, data, 8);
}

void implement_raise(void) {
    if (impl_state.type == IMPLEMENT_NONE) {
        return;
    }

    printf("[IMPLEMENT] Raising %s\n", implement_type_names[impl_state.type]);
    impl_state.status = IMPLEMENT_RAISED;
    impl_state.working_depth_cm = 0.0;

    // Send CAN message
    uint8_t data[8] = {0x00, 0, 0, 0, 0, 0, 0, 0};
    canbus_send_message(0x241, data, 8);
}

void implement_set_depth(float depth_cm) {
    impl_state.target_depth_cm = depth_cm;
    printf("[IMPLEMENT] Target depth set to %.1f cm\n", depth_cm);
}

void implement_update(void) {
    if (impl_state.type == IMPLEMENT_NONE) {
        return;
    }

    HydraulicsState* hyd = hydraulics_get_state();
    PTOState* pto = pto_get_state();

    if (impl_state.status == IMPLEMENT_WORKING) {
        // Monitor hydraulic pressure and flow
        impl_state.pressure_bar = hyd->system_pressure;
        impl_state.flow_lpm = 80.0 + (rand() % 40); // 80-120 lpm

        // Auto depth control simulation
        if (impl_state.auto_depth_control && impl_state.target_depth_cm > 0) {
            float depth_error = impl_state.target_depth_cm - impl_state.working_depth_cm;
            if (fabs(depth_error) > 0.5) {
                impl_state.working_depth_cm += depth_error * 0.1; // Gradual adjustment
            }
        }

        // Calculate coverage rate (simplified)
        // Coverage = width (m) × speed (km/h) × 0.1 (to get ha/hr)
        // Assuming average speed of 10 km/h
        impl_state.coverage_rate_ha_hr = impl_state.working_width_m * 10.0 * 0.1;

        // Check for implement errors
        if (impl_state.pressure_bar < 80.0) {
            diagnostics_report_fault(3002, "Implement", "Implement hydraulic pressure too low");
            impl_state.status = IMPLEMENT_ERROR;
        }

        // Check PTO engagement for implements that need it
        if (impl_state.type == IMPLEMENT_BALER || impl_state.type == IMPLEMENT_MOWER) {
            if (pto->status != PTO_ENGAGED) {
                diagnostics_report_fault(3003, "Implement", "PTO not engaged for implement operation");
            }
        }

        // Send telemetry via CAN
        uint8_t data[8] = {
            (uint8_t)impl_state.status,
            (uint8_t)impl_state.working_depth_cm,
            (uint8_t)impl_state.pressure_bar,
            (uint8_t)impl_state.flow_lpm,
            (uint8_t)(impl_state.coverage_rate_ha_hr * 10),
            0, 0, 0
        };
        canbus_send_message(0x242, data, 8);
    }
}

ImplementState* implement_get_state(void) {
    return &impl_state;
}
