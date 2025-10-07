#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "engine/engine_control.h"
#include "hydraulics/hydraulics.h"
#include "transmission/transmission.h"
#include "diagnostics/diagnostics.h"
#include "canbus/canbus.h"
#include "pto/pto.h"
#include "telematics/telematics.h"
#include "implement/implement.h"

// Main ECU control loop - coordinates all subsystems
void print_system_status(void) {
    EngineState* engine = engine_get_state();
    HydraulicsState* hydraulics = hydraulics_get_state();
    TransmissionState* transmission = transmission_get_state();
    PTOState* pto = pto_get_state();
    TelematicsState* telematics = telematics_get_state();
    ImplementState* implement = implement_get_state();

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
    printf("║                                                           ║\n");
    printf("║ PTO SYSTEM:                                               ║\n");
    printf("║   Status: %-12s  RPM: %4d / %4d               ║\n",
           pto->status == PTO_ENGAGED ? "Engaged" :
           pto->status == PTO_ENGAGING ? "Engaging" : "Disengaged",
           pto->current_rpm, pto->target_speed);
    printf("║   Load: %.1f%%    Torque: %.0f Nm                      ║\n",
           pto->load_percent, pto->torque_nm);
    printf("║                                                           ║\n");
    printf("║ GPS/TELEMATICS:                                           ║\n");
    printf("║   Position: %.4f, %.4f                     ║\n",
           telematics->gps.latitude, telematics->gps.longitude);
    printf("║   Speed: %.1f km/h    Heading: %.0f°                    ║\n",
           telematics->gps.speed_kmh, telematics->gps.heading_deg);
    printf("║   Cloud: %s    Signal: %.0f%%                      ║\n",
           telematics->connectivity.cloud_connected ? "Connected " : "Disconnected",
           telematics->connectivity.signal_strength);
    printf("║   Field Coverage: %.1f%%                                 ║\n",
           telematics->field_coverage_percent);
    printf("║                                                           ║\n");
    printf("║ IMPLEMENT CONTROL:                                        ║\n");
    if (implement->type != IMPLEMENT_NONE) {
        printf("║   Type: %-14s  Status: %-12s       ║\n",
               implement->type == IMPLEMENT_PLANTER ? "Planter" :
               implement->type == IMPLEMENT_SPRAYER ? "Sprayer" :
               implement->type == IMPLEMENT_BALER ? "Baler" :
               implement->type == IMPLEMENT_CULTIVATOR ? "Cultivator" :
               implement->type == IMPLEMENT_MOWER ? "Mower" : "Unknown",
               implement->status == IMPLEMENT_WORKING ? "Working" :
               implement->status == IMPLEMENT_RAISED ? "Raised" : "Idle");
        printf("║   Working Depth: %.1f cm    Width: %.1f m              ║\n",
               implement->working_depth_cm, implement->working_width_m);
        printf("║   Coverage Rate: %.1f ha/hr                            ║\n",
               implement->coverage_rate_ha_hr);
    } else {
        printf("║   No implement attached                                   ║\n");
    }
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
        pto_update();
        telematics_update();
        implement_update();
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
        pto_update();
        telematics_update();
        implement_update();
        diagnostics_update();
        canbus_update();
        sleep(1);
    }

    print_system_status();

    // Attach and configure implement
    printf("\n>>> Attaching 24-row planter...\n");
    implement_attach(IMPLEMENT_PLANTER);
    sleep(1);

    // Engage PTO
    printf("\n>>> Engaging PTO at 540 RPM...\n");
    pto_engage(PTO_SPEED_540);
    hydraulics_engage_pto(75);
    sleep(1);

    for (int i = 0; i < 4; i++) {
        engine_update();
        transmission_update();
        hydraulics_update();
        pto_update();
        telematics_update();
        implement_update();
        diagnostics_update();
        canbus_update();
        sleep(1);
    }

    // Lower implement and start working
    printf("\n>>> Lowering implement to begin planting...\n");
    implement_lower();
    hydraulics_raise_implement();

    for (int i = 0; i < 5; i++) {
        engine_update();
        transmission_update();
        hydraulics_update();
        pto_update();
        telematics_update();
        implement_update();
        diagnostics_update();
        canbus_update();
        sleep(1);
    }

    print_system_status();

    // Send telematics update
    printf("\n>>> Sending telematics data to cloud...\n");
    telematics_send_status_update();
    sleep(1);

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
    pto_init();             // PTO control
    telematics_init();      // GPS and cloud connectivity
    implement_init();       // Implement control

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
            pto_update();
            telematics_update();
            implement_update();
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
