#ifndef TELEMATICS_H
#define TELEMATICS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    double latitude;
    double longitude;
    float altitude_m;
    float speed_kmh;
    float heading_deg;        // 0-360 degrees
    int satellites;
    bool gps_fix;
} GPSData;

typedef struct {
    bool cloud_connected;
    float signal_strength;    // 0-100%
    int data_sent_kb;
    int data_received_kb;
    char connection_type[16]; // "4G LTE", "5G", "Satellite"
} ConnectivityStatus;

typedef struct {
    GPSData gps;
    ConnectivityStatus connectivity;
    float field_coverage_percent;  // How much of field has been covered
    float work_hours;              // Operating hours today
    bool remote_command_pending;
} TelematicsState;

// Telematics functions
void telematics_init(void);
void telematics_update(void);
void telematics_send_status_update(void);
TelematicsState* telematics_get_state(void);

#endif // TELEMATICS_H
