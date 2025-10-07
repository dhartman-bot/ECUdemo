#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "engine/engine_control.h"
#include "hydraulics/hydraulics.h"
#include "transmission/transmission.h"
#include "diagnostics/diagnostics.h"
#include "canbus/canbus.h"

// Main ECU control loop - coordinates all subsystems
void print_system_status(void) {
    EngineState* engine = engine_get_state();
    HydraulicsState* hydraulics = hydraulics_get_state();
    TransmissionState* transmission = transmission_get_state();

    printf("\n╔════════════════════════════════════════════════════════════╗\n");
    printf("║              TRACTOR ECU MONOLITH STATUS                  ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ ENGINE:                                                   ║\n");
    printf("║   RPM: %4d / %4d    Status: %-10s              ║\n",
           engine->current_rpm, engine->target_rpm,
           engine->status == STATUS_OK ? "OK" : "WARNING");
    printf("║   Fuel Rate: %.1f L/hr    Coolant: %.1f°C             ║\n",
           engine->fuel_rate, engine->coolant_temp);
    printf("║                                                           ║\n");
    printf("║ TRANSMISSION:                                             ║\n");
    printf("║   Gear: %d    Output Speed: %.0f RPM                     ║\n",
           transmission->current_gear, transmission->output_speed);
    printf("║   Temp: %.1f°C    Clutch: %s                         ║\n",
           transmission->transmission_temp,
           transmission->clutch_engaged ? "Engaged " : "Released");
    printf("║                                                           ║\n");
    printf("║ HYDRAULICS:                                               ║\n");
    printf("║   Pressure: %.0f PSI    Flow: %.1f GPM                  ║\n",
           hydraulics->system_pressure, hydraulics->flow_rate);
    printf("║   PTO: %s at %d%%    Implement: %s              ║\n",
           hydraulics->pto_engaged ? "Engaged " : "Disabled",
           hydraulics->pto_speed,
           hydraulics->implement_raised ? "Raised " : "Lowered");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
}

void run_demo_sequence(void) {
    printf("\n🚜 Starting Tractor ECU Demo Sequence...\n\n");

    // Start engine
    printf(">>> Starting engine...\n");
    engine_start();
    sleep(1);

    // Let engine warm up
    for (int i = 0; i < 3; i++) {
        engine_update();
        transmission_update();
        hydraulics_update();
        diagnostics_update();
        canbus_update();
        sleep(1);
    }

    // Shift to drive
    printf("\n>>> Shifting to Drive (Gear 2)...\n");
    transmission_shift_gear(GEAR_DRIVE_1);
    transmission_engage_clutch();
    sleep(1);

    // Increase throttle
    printf("\n>>> Increasing throttle to 50%%...\n");
    engine_set_throttle(50);

    for (int i = 0; i < 5; i++) {
        engine_update();
        transmission_update();
        hydraulics_update();
        diagnostics_update();
        canbus_update();
        sleep(1);
    }

    print_system_status();

    // Engage PTO
    printf("\n>>> Engaging PTO at 75%%...\n");
    hydraulics_engage_pto(75);
    sleep(1);

    // Raise implement
    printf("\n>>> Raising implement...\n");
    hydraulics_raise_implement();

    for (int i = 0; i < 3; i++) {
        engine_update();
        transmission_update();
        hydraulics_update();
        diagnostics_update();
        canbus_update();
        sleep(1);
    }

    print_system_status();

    // Print diagnostics
    diagnostics_print_status();
    canbus_print_stats();

    printf("\n✅ Demo sequence complete!\n");
}

int main(int argc, char* argv[]) {
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║         TRACTOR ECU MONOLITHIC CONTROLLER v1.0              ║\n");
    printf("║              Agricultural Equipment Control System           ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    // Initialize all subsystems
    printf("Initializing subsystems...\n");
    canbus_init();          // Core communication layer
    diagnostics_init();     // Fault tracking
    engine_init();          // Engine control
    transmission_init();    // Transmission control
    hydraulics_init();      // Hydraulics control

    printf("\n✓ All subsystems initialized\n");

    // Run demo or interactive mode
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        run_demo_sequence();
    } else {
        printf("\nStarting main control loop (press Ctrl+C to stop)...\n");
        printf("Tip: Run with --demo flag to see automated demo\n\n");

        engine_start();
        transmission_shift_gear(GEAR_NEUTRAL);

        // Main control loop
        for (int cycle = 0; cycle < 10; cycle++) {
            engine_update();
            transmission_update();
            hydraulics_update();
            diagnostics_update();
            canbus_update();

            if (cycle % 3 == 0) {
                print_system_status();
            }

            sleep(2);
        }
    }

    printf("\n🛑 Shutting down ECU controller...\n");
    engine_stop();

    return 0;
}
