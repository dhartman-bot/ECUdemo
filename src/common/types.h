#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

// Common types used across all modules
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = 1,
    STATUS_WARNING = 2,
    STATUS_CRITICAL = 3
} SystemStatus;

typedef enum {
    LOG_INFO = 0,
    LOG_WARNING = 1,
    LOG_ERROR = 2,
    LOG_CRITICAL = 3
} LogLevel;

typedef struct {
    uint32_t timestamp;
    LogLevel level;
    char message[256];
    char module[64];
} LogEntry;

typedef struct {
    float voltage;
    float current;
    float temperature;
} SensorData;

#endif // TYPES_H
