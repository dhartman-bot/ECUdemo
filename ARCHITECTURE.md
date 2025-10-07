# Tractor ECU Monolith Architecture

## Overview
This is a monolithic embedded ECU (Electronic Control Unit) controller for agricultural equipment. All subsystems are tightly coupled in a single application.

## Subsystems

### 1. **Engine Control** (`src/engine/`)
Manages engine operations including RPM, fuel injection, and thermal monitoring.

**Responsibilities:**
- RPM control and throttle management
- Fuel rate calculation
- Engine temperature monitoring
- Oil pressure monitoring
- Engine start/stop sequences

**Dependencies:**
- **CANBus** → Sends RPM data (Message ID: 0x100)
- **Diagnostics** → Reports fault codes for overheating or low pressure

**State:**
- Current/Target RPM
- Fuel rate (L/hr)
- Coolant temperature
- Oil pressure
- Engine running status

---

### 2. **Hydraulics Control** (`src/hydraulics/`)
Controls hydraulic systems including implements, PTO (Power Take-Off), and fluid management.

**Responsibilities:**
- Hydraulic pressure regulation
- PTO speed control
- Implement raise/lower operations
- Reservoir level monitoring
- Hydraulic oil temperature

**Dependencies:**
- **Engine** → Reads engine RPM to calculate pump speed
- **CANBus** → Sends hydraulic data (Message ID: 0x200)
- **Diagnostics** → Reports hydraulic faults

**State:**
- System pressure (PSI)
- Flow rate (GPM)
- Reservoir level (%)
- PTO engagement and speed
- Implement position

---

### 3. **Transmission Control** (`src/transmission/`)
Manages gear selection, clutch engagement, and power transfer from engine to wheels.

**Responsibilities:**
- Gear shifting (Park, Neutral, Drive 1-4, Reverse)
- Clutch control
- Output speed calculation based on gear ratios
- Transmission temperature monitoring
- Transmission oil pressure

**Dependencies:**
- **Engine** → Reads engine RPM for gear ratio calculations
- **CANBus** → Sends speed data (Message ID: 0x300)
- **Diagnostics** → Reports transmission faults

**State:**
- Current gear position
- Clutch engagement status
- Output shaft speed
- Transmission temperature
- Oil pressure

**Gear Ratios:**
- Park/Neutral: 0.0
- Drive 1: 3.5:1
- Drive 2: 2.2:1
- Drive 3: 1.5:1
- Drive 4: 1.0:1
- Reverse: -4.0:1

---

### 4. **Diagnostics** (`src/diagnostics/`)
Central fault tracking and system health monitoring.

**Responsibilities:**
- Fault code management (storage of up to 50 faults)
- Active fault tracking
- System health status aggregation
- Fault reporting and clearing
- Status display

**Dependencies:**
- **CANBus** → Sends diagnostic summary (Message ID: 0x400)
- Called by: Engine, Hydraulics, Transmission (to report faults)

**State:**
- Fault records (code, module, description, timestamp)
- Active fault count
- Overall system status

**Fault Codes:**
- 0x01: Engine faults
- 0x02: Hydraulics faults
- 0x03: Transmission faults

---

### 5. **CAN Bus** (`src/canbus/`)
Core communication layer enabling inter-module data exchange.

**Responsibilities:**
- Message sending/receiving
- Message buffering (up to 100 messages)
- Bus load monitoring
- Communication statistics

**Dependencies:**
- None (core infrastructure layer)
- Used by: All other modules

**State:**
- Message buffer
- Messages sent/received counters
- Bus load percentage
- Communication status

**Message IDs:**
- 0x100: Engine RPM data
- 0x200: Hydraulic pressure data
- 0x300: Transmission speed data
- 0x400: Diagnostic fault count

---

## Dependency Graph

```
┌─────────────────────────────────────────────────────────────┐
│                        MAIN LOOP                            │
│                     (src/main.c)                            │
└──────────────┬──────────────────────────────────────────────┘
               │
               │ Coordinates all subsystems
               │
    ┌──────────┴──────────┬──────────┬──────────┬─────────────┐
    │                     │          │          │             │
    ▼                     ▼          ▼          ▼             ▼
┌─────────┐         ┌─────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Engine  │◄────────│Transmis.│  │Hydraul.│  │Diagnos.│  │CANBus  │
│ Control │         │ Control │  │Control │  │        │  │(Core)  │
└────┬────┘         └────┬────┘  └───┬────┘  └───┬────┘  └───▲────┘
     │                   │           │           │           │
     │ RPM data          │ RPM data  │ RPM data  │ Faults    │
     │                   │           │           │           │
     └───────────────────┴───────────┴───────────┴───────────┘
                    All modules send data to CAN bus
```

### Cross-Module Dependencies:

**Engine Control:**
- Sends to: CANBus, Diagnostics
- Receives from: None (independent)

**Hydraulics Control:**
- Sends to: CANBus, Diagnostics
- Receives from: Engine (reads engine state for pump calculations)

**Transmission Control:**
- Sends to: CANBus, Diagnostics
- Receives from: Engine (reads engine RPM for output calculations)

**Diagnostics:**
- Sends to: CANBus
- Receives from: Engine, Hydraulics, Transmission (fault reports)

**CAN Bus:**
- Sends to: None
- Receives from: All modules (core infrastructure)

---

## Problems with Monolithic Design

### 1. **Tight Coupling**
- Modules directly include and call each other's headers
- Changes in one module require recompilation of dependent modules
- Hard to test modules in isolation

### 2. **No Clear API Boundaries**
- Functions directly access other modules' state via getter functions
- No standardized message format or protocol

### 3. **Scalability Issues**
- Adding new features requires modifying existing code
- CAN bus message IDs are hardcoded throughout
- Single point of failure - one crash brings down entire system

### 4. **Testing Challenges**
- Can't mock dependencies easily
- Must initialize all modules even to test one
- No clear unit test boundaries

### 5. **Deployment Limitations**
- Must deploy entire application for any change
- Can't update individual modules independently
- All code runs in single process/memory space

---

## Microservices Migration Strategy

### Target Architecture

Each subsystem becomes an independent service:

1. **Engine Service** - Standalone engine controller
2. **Hydraulics Service** - Independent hydraulics management
3. **Transmission Service** - Isolated transmission control
4. **Diagnostics Service** - Centralized monitoring and logging
5. **Message Bus** - Replaced with proper message broker (MQTT/DDS)

### Benefits of Microservices:

- **Independence**: Deploy and update services separately
- **Resilience**: One service failure doesn't crash entire system
- **Scalability**: Scale individual services based on load
- **Technology flexibility**: Different languages/frameworks per service
- **Clear contracts**: Well-defined APIs and message schemas
- **Testability**: Easy to mock and test in isolation

### Migration Steps:

1. **Analyze dependencies** - Map all cross-module calls
2. **Define service boundaries** - Identify natural separation points
3. **Design message protocols** - Create standardized message formats
4. **Extract one service** - Start with least dependent module (e.g., Engine)
5. **Implement message bus** - Replace direct calls with async messaging
6. **Add service APIs** - REST/gRPC endpoints for service communication
7. **Repeat for other services** - Gradually extract remaining modules

---

## File Structure

```
tractor-ecu-demo/
├── src/
│   ├── main.c                    # Main control loop
│   ├── common/
│   │   └── types.h               # Shared type definitions
│   ├── engine/
│   │   ├── engine_control.h      # Engine interface
│   │   └── engine_control.c      # Engine implementation
│   ├── hydraulics/
│   │   ├── hydraulics.h          # Hydraulics interface
│   │   └── hydraulics.c          # Hydraulics implementation
│   ├── transmission/
│   │   ├── transmission.h        # Transmission interface
│   │   └── transmission.c        # Transmission implementation
│   ├── diagnostics/
│   │   ├── diagnostics.h         # Diagnostics interface
│   │   └── diagnostics.c         # Diagnostics implementation
│   └── canbus/
│       ├── canbus.h              # CAN bus interface
│       └── canbus.c              # CAN bus implementation
├── ARCHITECTURE.md               # This file
├── README.md                     # Getting started guide
└── package.json                  # Build configuration
```
