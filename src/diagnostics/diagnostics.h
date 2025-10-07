#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "../common/types.h"

#define MAX_FAULTS 50

// Diagnostics module - tracks faults, logs events, health monitoring
typedef struct {
    uint16_t fault_code;
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
void diagnostics_report_fault(uint16_t code, const char* module, const char* description);
void diagnostics_clear_fault(uint16_t code);
void diagnostics_print_status(void);
DiagnosticsState* diagnostics_get_state(void);

#endif // DIAGNOSTICS_H
