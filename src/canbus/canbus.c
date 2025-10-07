#include "canbus.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static CANBusState canbus_state = {0};

void canbus_init(void) {
    printf("[CANBUS] Initializing CAN bus module\n");
    canbus_state.message_count = 0;
    canbus_state.messages_sent = 0;
    canbus_state.messages_received = 0;
    canbus_state.bus_load_percent = 0.0;
    canbus_state.status = STATUS_OK;
    memset(canbus_state.message_buffer, 0, sizeof(canbus_state.message_buffer));
}

void canbus_update(void) {
    // Calculate bus load based on message count
    canbus_state.bus_load_percent = (canbus_state.message_count / (float)MAX_CAN_MESSAGES) * 100.0;

    // Clear old messages if buffer is getting full
    if (canbus_state.message_count > MAX_CAN_MESSAGES * 0.8) {
        printf("[CANBUS] Buffer 80%% full, clearing old messages\n");
        canbus_state.message_count = 0;
    }

    // Update status based on bus load
    if (canbus_state.bus_load_percent > 90.0) {
        canbus_state.status = STATUS_WARNING;
    } else {
        canbus_state.status = STATUS_OK;
    }
}

void canbus_send_message(uint32_t id, uint8_t* data, uint8_t length) {
    if (length > 8) length = 8;

    if (canbus_state.message_count < MAX_CAN_MESSAGES) {
        CANMessage* msg = &canbus_state.message_buffer[canbus_state.message_count];
        msg->message_id = id;
        msg->length = length;
        memcpy(msg->data, data, length);
        msg->timestamp = (uint32_t)time(NULL);

        canbus_state.message_count++;
        canbus_state.messages_sent++;
    }
}

bool canbus_receive_message(CANMessage* message) {
    if (canbus_state.message_count > 0) {
        memcpy(message, &canbus_state.message_buffer[0], sizeof(CANMessage));
        canbus_state.messages_received++;
        return true;
    }
    return false;
}

void canbus_print_stats(void) {
    printf("\n=== CAN BUS STATISTICS ===\n");
    printf("Messages sent: %u\n", canbus_state.messages_sent);
    printf("Messages received: %u\n", canbus_state.messages_received);
    printf("Current buffer count: %u\n", canbus_state.message_count);
    printf("Bus load: %.1f%%\n", canbus_state.bus_load_percent);
    printf("Status: ");

    switch (canbus_state.status) {
        case STATUS_OK: printf("OK\n"); break;
        case STATUS_WARNING: printf("WARNING\n"); break;
        case STATUS_ERROR: printf("ERROR\n"); break;
        case STATUS_CRITICAL: printf("CRITICAL\n"); break;
    }
    printf("==========================\n\n");
}

CANBusState* canbus_get_state(void) {
    return &canbus_state;
}
