#include "diagnostics.h"
#include "../canbus/canbus.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static DiagnosticsState diagnostics_state = {0};

void diagnostics_init(void) {
    printf("[DIAGNOSTICS] Initializing diagnostics module\n");
    diagnostics_state.fault_count = 0;
    diagnostics_state.active_fault_count = 0;
    diagnostics_state.overall_status = STATUS_OK;
    memset(diagnostics_state.faults, 0, sizeof(diagnostics_state.faults));
}

void diagnostics_update(void) {
    // Count active faults and determine overall status
    diagnostics_state.active_fault_count = 0;
    diagnostics_state.overall_status = STATUS_OK;

    for (int i = 0; i < diagnostics_state.fault_count; i++) {
        if (diagnostics_state.faults[i].active) {
            diagnostics_state.active_fault_count++;
            // Escalate overall status
            if (diagnostics_state.overall_status < STATUS_WARNING) {
                diagnostics_state.overall_status = STATUS_WARNING;
            }
        }
    }

    // Send diagnostic summary to CAN bus
    if (diagnostics_state.active_fault_count > 0) {
        canbus_send_message(0x400, (uint8_t*)&diagnostics_state.active_fault_count, 2);
    }
}

void diagnostics_report_fault(uint16_t code, const char* module, const char* description) {
    // Check if fault already exists
    for (int i = 0; i < diagnostics_state.fault_count; i++) {
        if (diagnostics_state.faults[i].fault_code == code) {
            diagnostics_state.faults[i].active = true;
            diagnostics_state.faults[i].timestamp = (uint32_t)time(NULL);
            return;
        }
    }

    // Add new fault
    if (diagnostics_state.fault_count < MAX_FAULTS) {
        FaultRecord* fault = &diagnostics_state.faults[diagnostics_state.fault_count];
        fault->fault_code = code;
        strncpy(fault->module, module, sizeof(fault->module) - 1);
        strncpy(fault->description, description, sizeof(fault->description) - 1);
        fault->timestamp = (uint32_t)time(NULL);
        fault->active = true;
        diagnostics_state.fault_count++;

        printf("[DIAGNOSTICS] FAULT 0x%04X in %s: %s\n", code, module, description);
    }
}

void diagnostics_clear_fault(uint16_t code) {
    for (int i = 0; i < diagnostics_state.fault_count; i++) {
        if (diagnostics_state.faults[i].fault_code == code) {
            diagnostics_state.faults[i].active = false;
            printf("[DIAGNOSTICS] Cleared fault 0x%04X\n", code);
            return;
        }
    }
}

void diagnostics_print_status(void) {
    printf("\n=== DIAGNOSTICS STATUS ===\n");
    printf("Total faults recorded: %d\n", diagnostics_state.fault_count);
    printf("Active faults: %d\n", diagnostics_state.active_fault_count);
    printf("Overall status: ");

    switch (diagnostics_state.overall_status) {
        case STATUS_OK: printf("OK\n"); break;
        case STATUS_WARNING: printf("WARNING\n"); break;
        case STATUS_ERROR: printf("ERROR\n"); break;
        case STATUS_CRITICAL: printf("CRITICAL\n"); break;
    }

    if (diagnostics_state.active_fault_count > 0) {
        printf("\nActive Faults:\n");
        for (int i = 0; i < diagnostics_state.fault_count; i++) {
            if (diagnostics_state.faults[i].active) {
                printf("  [0x%04X] %s: %s\n",
                    diagnostics_state.faults[i].fault_code,
                    diagnostics_state.faults[i].module,
                    diagnostics_state.faults[i].description);
            }
        }
    }
    printf("==========================\n\n");
}

DiagnosticsState* diagnostics_get_state(void) {
    return &diagnostics_state;
}
