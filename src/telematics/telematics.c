#include "telematics.h"
#include "../canbus/canbus.h"
#include "../diagnostics/diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static TelematicsState telem_state = {
    .gps = {
        .latitude = 41.6032,    // John Deere HQ in Moline, IL
        .longitude = -90.5776,
        .altitude_m = 180.0,
        .speed_kmh = 0.0,
        .heading_deg = 0.0,
        .satellites = 0,
        .gps_fix = false
    },
    .connectivity = {
        .cloud_connected = false,
        .signal_strength = 0.0,
        .data_sent_kb = 0,
        .data_received_kb = 0,
        .connection_type = "4G LTE"
    },
    .field_coverage_percent = 0.0,
    .work_hours = 0.0,
    .remote_command_pending = false
};

static int update_counter = 0;

void telematics_init(void) {
    printf("[TELEMATICS] Initializing GPS/Telematics module\n");
    printf("[TELEMATICS] Acquiring GPS signal...\n");

    // Simulate GPS acquisition
    telem_state.gps.satellites = 8;
    telem_state.gps.gps_fix = true;

    printf("[TELEMATICS] GPS fix acquired - %d satellites\n", telem_state.gps.satellites);
    printf("[TELEMATICS] Position: %.4f, %.4f\n",
           telem_state.gps.latitude, telem_state.gps.longitude);

    // Simulate cloud connection
    telem_state.connectivity.cloud_connected = true;
    telem_state.connectivity.signal_strength = 85.0;
    printf("[TELEMATICS] Connected to cloud via %s (signal: %.0f%%)\n",
           telem_state.connectivity.connection_type,
           telem_state.connectivity.signal_strength);
}

void telematics_update(void) {
    update_counter++;

    // Update GPS position (simulate movement)
    if (telem_state.gps.gps_fix) {
        // Simulate tractor moving in a field pattern
        telem_state.gps.latitude += 0.00001 * (rand() % 3 - 1);
        telem_state.gps.longitude += 0.00001 * (rand() % 3 - 1);
        telem_state.gps.speed_kmh = 8.0 + (rand() % 30) / 10.0; // 8-11 km/h
        telem_state.gps.heading_deg = 90.0 + (rand() % 20 - 10); // Generally east

        // Update field coverage (increases over time)
        if (telem_state.field_coverage_percent < 100.0) {
            telem_state.field_coverage_percent += 0.5;
        }
    }

    // Update work hours
    telem_state.work_hours += 0.1 / 3600.0; // Increment by update interval

    // Simulate connectivity fluctuations
    telem_state.connectivity.signal_strength = 75.0 + (rand() % 20);

    // Check for connectivity issues
    if (telem_state.connectivity.signal_strength < 30.0) {
        diagnostics_report_fault(SPN_CELLULAR_SIGNAL, FMI_DATA_BELOW_NORMAL, "Telematics", "Cellular signal strength below minimum threshold");
        telem_state.connectivity.cloud_connected = false;
    } else {
        telem_state.connectivity.cloud_connected = true;
    }

    // Send GPS data via CAN bus
    if (update_counter % 10 == 0) {
        uint8_t data[8] = {
            (uint8_t)(telem_state.gps.latitude * 10000),
            (uint8_t)(telem_state.gps.longitude * 10000),
            (uint8_t)telem_state.gps.speed_kmh,
            (uint8_t)telem_state.gps.heading_deg,
            telem_state.gps.satellites,
            (uint8_t)telem_state.connectivity.signal_strength,
            0, 0
        };
        canbus_send_message(0x230, data, 8);
    }
}

void telematics_send_status_update(void) {
    printf("[TELEMATICS] Sending status update to cloud...\n");
    telem_state.connectivity.data_sent_kb += 5; // 5 KB update

    // Simulate receiving acknowledgment
    telem_state.connectivity.data_received_kb += 1;

    printf("[TELEMATICS] Cloud sync complete (↑%d KB ↓%d KB)\n",
           telem_state.connectivity.data_sent_kb,
           telem_state.connectivity.data_received_kb);
}

TelematicsState* telematics_get_state(void) {
    return &telem_state;
}
