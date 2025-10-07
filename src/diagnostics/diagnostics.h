#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "../common/types.h"

#define MAX_FAULTS 50

// John Deere SPN-FMI Fault Code Structure
// Format: SPN.FMI (e.g., 000110.00)
// SPN = Suspect Parameter Number (identifies component)
// FMI = Failure Mode Identifier (identifies failure type)

// Common FMI codes:
#define FMI_DATA_ABOVE_NORMAL    0   // Data valid but above normal range
#define FMI_DATA_BELOW_NORMAL    1   // Data valid but below normal range
#define FMI_DATA_ERRATIC         2   // Data erratic/intermittent
#define FMI_VOLTAGE_HIGH         3   // Voltage above normal
#define FMI_VOLTAGE_LOW          4   // Voltage below normal
#define FMI_CURRENT_LOW          5   // Current below normal
#define FMI_CURRENT_HIGH         6   // Current above normal
#define FMI_MECHANICAL_FAULT     7   // Mechanical system not responding
#define FMI_ABNORMAL_FREQUENCY   8   // Abnormal frequency/pulse width
#define FMI_ABNORMAL_UPDATE      9   // Abnormal update rate
#define FMI_ABNORMAL_CHANGE     10   // Abnormal rate of change
#define FMI_ROOT_CAUSE_UNKNOWN  12   // Bad intelligent device or component
#define FMI_CALIBRATION_FAULT   13   // Out of calibration
#define FMI_NETWORK_ERROR       19   // Received network data in error

// SPN codes for ECU subsystems
// Engine (ECU) codes
#define SPN_ENGINE_COOLANT_TEMP     110
#define SPN_ENGINE_OIL_PRESSURE     100
#define SPN_ENGINE_RPM              190
#define SPN_FUEL_RATE               183
#define SPN_ENGINE_SPEED_SENSOR     636

// Hydraulic (HCU) codes
#define SPN_HYDRAULIC_PRESSURE      3480
#define SPN_HYDRAULIC_OIL_TEMP      3481
#define SPN_HYDRAULIC_FLOW_RATE     3482
#define SPN_HYDRAULIC_RESERVOIR     3483

// Transmission codes
#define SPN_TRANS_OUTPUT_SPEED      161
#define SPN_TRANS_GEAR_POSITION     523
#define SPN_TRANS_CLUTCH_PRESSURE   524
#define SPN_TRANS_OIL_TEMP          177

// PTO codes
#define SPN_PTO_ENGAGEMENT          558
#define SPN_PTO_SPEED               559
#define SPN_PTO_SHAFT_SPEED        1483

// Telematics codes
#define SPN_GPS_LATITUDE            2829
#define SPN_GPS_LONGITUDE           2830
#define SPN_CELLULAR_SIGNAL         2831

// Implement codes
#define SPN_IMPLEMENT_POSITION      1810
#define SPN_IMPLEMENT_DEPTH         1811
#define SPN_IMPLEMENT_PRESSURE      1812

// Diagnostics module - tracks faults, logs events, health monitoring
typedef struct {
    uint32_t spn;        // Suspect Parameter Number (6 digits)
    uint8_t fmi;         // Failure Mode Identifier (2 digits)
    char module[32];
    char description[128];
    uint32_t timestamp;
    bool active;
} FaultRecord;

typedef struct {
    FaultRecord faults[MAX_FAULTS];
    uint16_t fault_count;
    uint16_t active_fault_count;
    SystemStatus overall_status;
} DiagnosticsState;

// Dependencies: CANBus (send diagnostic data to external tools)
void diagnostics_init(void);
void diagnostics_update(void);
void diagnostics_report_fault(uint32_t spn, uint8_t fmi, const char* module, const char* description);
void diagnostics_clear_fault(uint32_t spn, uint8_t fmi);
void diagnostics_print_status(void);
DiagnosticsState* diagnostics_get_state(void);

#endif // DIAGNOSTICS_H
