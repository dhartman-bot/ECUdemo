#ifndef CANBUS_H
#define CANBUS_H

#include "../common/types.h"

#define MAX_CAN_MESSAGES 100

// CAN bus communication module - handles inter-module communication
typedef struct {
    uint32_t message_id;
    uint8_t data[8];
    uint8_t length;
    uint32_t timestamp;
} CANMessage;

typedef struct {
    CANMessage message_buffer[MAX_CAN_MESSAGES];
    uint16_t message_count;
    uint32_t messages_sent;
    uint32_t messages_received;
    float bus_load_percent;
    SystemStatus status;
} CANBusState;

// Core module - all other modules depend on this for communication
void canbus_init(void);
void canbus_update(void);
void canbus_send_message(uint32_t id, uint8_t* data, uint8_t length);
bool canbus_receive_message(CANMessage* message);
void canbus_print_stats(void);
CANBusState* canbus_get_state(void);

#endif // CANBUS_H
