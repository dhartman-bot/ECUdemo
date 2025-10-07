# Tractor ECU Monolith Demo

A realistic embedded ECU (Electronic Control Unit) controller demonstration for agricultural equipment. This project simulates a monolithic control system that can be analyzed and decomposed into microservices.

## 🎯 Purpose

This demo showcases:
1. **Monolithic architecture** with tightly coupled subsystems
2. **Complex interdependencies** between modules
3. **Real-world embedded patterns** common in agricultural equipment
4. **Migration path** from monolith to microservices

Perfect for demonstrating code analysis, dependency mapping, and refactoring capabilities with Claude Code.

## 🏗️ Architecture

The ECU controller manages 5 key subsystems:

### 1. **Engine Control**
- RPM management and throttle control
- Fuel injection and consumption tracking
- Temperature and pressure monitoring
- **Dependencies**: CANBus, Diagnostics

### 2. **Hydraulics Control**
- Hydraulic pressure regulation
- PTO (Power Take-Off) control
- Implement operations (raise/lower)
- **Dependencies**: Engine (for pump speed), CANBus, Diagnostics

### 3. **Transmission Control**
- Gear selection (Park, Neutral, Drive 1-4, Reverse)
- Clutch engagement
- Speed calculations
- **Dependencies**: Engine (for gear ratios), CANBus, Diagnostics

### 4. **Diagnostics**
- Fault code tracking (up to 50 faults)
- System health monitoring
- Event logging
- **Dependencies**: CANBus

### 5. **CAN Bus**
- Inter-module communication
- Message buffering
- Bus load monitoring
- **Dependencies**: None (core layer)

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed dependency graphs and design analysis.

## 🚀 Quick Start

### Prerequisites
- GCC compiler
- Make (optional)

### Build and Run

```bash
# Navigate to project directory
cd tractor-ecu-demo

# Create build directory
mkdir -p build

# Compile the monolith
gcc -o build/ecu_controller src/**/*.c src/*.c -I./src -lm

# Run in demo mode
./build/ecu_controller --demo

# Or run in continuous mode
./build/ecu_controller
```

### Expected Output

The demo will:
1. Initialize all subsystems
2. Start the engine
3. Shift through gears
4. Engage PTO and raise implements
5. Display system status and diagnostics

Example output:
```
╔══════════════════════════════════════════════════════════════╗
║         TRACTOR ECU MONOLITHIC CONTROLLER v1.0              ║
║              Agricultural Equipment Control System           ║
╚══════════════════════════════════════════════════════════════╝

[CANBUS] Initializing CAN bus module
[DIAGNOSTICS] Initializing diagnostics module
[ENGINE] Initializing engine control module
[TRANSMISSION] Initializing transmission control module
[HYDRAULICS] Initializing hydraulics control module

🚜 Starting Tractor ECU Demo Sequence...

>>> Starting engine...
[ENGINE] Starting engine

>>> Shifting to Drive (Gear 2)...
[TRANSMISSION] Shifting to gear: 2
[TRANSMISSION] Engaging clutch

>>> Increasing throttle to 50%...
[ENGINE] Throttle set to 50%, target RPM: 1700
```

## 📊 Demo Scenarios

### Scenario 1: Inspect the Monolith

**Goal**: Understand what the codebase does and how modules interact.

**Claude Code prompts:**
```
1. "Analyze this codebase and explain what it does"
2. "Map out all the dependencies between modules"
3. "What are the main features of this system?"
4. "Show me all the places where Engine module is used by other modules"
5. "What would happen if the Engine module fails?"
```

### Scenario 2: Analyze for Microservices

**Goal**: Identify boundaries for service extraction.

**Claude Code prompts:**
```
1. "Which module has the fewest dependencies?"
2. "What would it take to extract the Engine module into its own service?"
3. "Map all the CAN bus message IDs and their purposes"
4. "What data contracts exist between modules?"
5. "Create a migration plan to break this into microservices"
```

### Scenario 3: Extract Engine Service

**Goal**: Isolate the Engine module into a standalone service.

**Claude Code prompts:**
```
1. "Extract the Engine module into a standalone service"
2. "Create a REST API for the Engine service"
3. "Replace direct Engine calls with HTTP requests"
4. "Add MQTT support for Engine telemetry"
5. "Create docker-compose for Engine + message broker"
```

**Expected artifacts:**
- `services/engine-service/` - Standalone engine controller
- `services/engine-service/api.py` - REST API endpoints
- `services/engine-service/Dockerfile` - Container definition
- `docker-compose.yml` - Multi-service orchestration
- Updated main controller using HTTP client

### Scenario 4: Add New Feature

**Goal**: Demonstrate difficulty of adding features to monolith vs microservices.

**Claude Code prompts:**
```
1. "Add a new GPS/Navigation module to the monolith"
2. "How many files need to be modified?"
3. "What are the risks of this change?"
4. "Now show me how this would be easier with microservices"
```

## 🔍 Key Files for Demo

### Start here:
- `src/main.c` - Main control loop, shows system initialization
- `ARCHITECTURE.md` - Complete system documentation

### Explore dependencies:
- `src/engine/engine_control.c` - Lines 24-40 show CANBus usage
- `src/hydraulics/hydraulics.c` - Line 20 shows Engine dependency
- `src/transmission/transmission.c` - Line 21 shows Engine dependency

### Understand the problems:
- All modules include each other's headers (tight coupling)
- CAN message IDs hardcoded (0x100, 0x200, 0x300, 0x400)
- No versioning or backward compatibility
- Single point of failure

## 🎓 Learning Outcomes

After this demo, you'll understand:

1. **Monolith characteristics**
   - Tight coupling between modules
   - Shared memory and state
   - Synchronous communication
   - Single deployment unit

2. **Dependency analysis**
   - Direct dependencies (includes, function calls)
   - Indirect dependencies (shared state)
   - Circular dependencies
   - Dependency direction and depth

3. **Microservices benefits**
   - Independent deployment
   - Fault isolation
   - Technology diversity
   - Scalability

4. **Migration strategy**
   - Identify service boundaries
   - Design API contracts
   - Implement message bus
   - Gradual extraction

## 🛠️ Customization Ideas

Enhance the demo with:

1. **Add more subsystems**
   - GPS/Navigation
   - Climate control (A/C, heating)
   - Telematics (cellular connectivity)
   - Implement control (specific to equipment type)

2. **Add complexity**
   - Configuration file parsing
   - State persistence (save/load)
   - Firmware update mechanism
   - Security (authentication, encryption)

3. **Add monitoring**
   - Prometheus metrics export
   - Grafana dashboards
   - Structured logging (JSON)
   - Distributed tracing

4. **Create microservices**
   - Extract each module to separate service
   - Implement with different languages (Python, Go, Rust)
   - Add API gateway
   - Deploy on Kubernetes

## 📝 Notes for Presenters

### Talking Points:

1. **Agricultural equipment context**
   - ECUs control complex machinery (tractors, combines, sprayers)
   - Real-time requirements (engine timing, hydraulic response)
   - Safety-critical systems (must be reliable)
   - Long service life (10-20 years)

2. **Why monoliths exist in embedded**
   - Resource constraints (limited CPU, memory)
   - Real-time determinism (predictable latency)
   - Toolchain maturity (C/C++ ecosystem)
   - Legacy codebases (decades old)

3. **Modern trends**
   - More powerful embedded processors
   - Connectivity requirements (IoT, telematics)
   - Over-the-air updates
   - Cloud integration

4. **Migration challenges**
   - Cannot stop a running tractor for updates
   - Network reliability (fields have poor connectivity)
   - Backward compatibility with older equipment
   - Safety certification requirements

### Demo Flow Suggestions:

1. **Introduction** (5 min)
   - Show tractor ECU context (photos/videos)
   - Explain the 5 subsystems
   - Run the demo to show it working

2. **Code Analysis** (10 min)
   - Use Claude Code to map dependencies
   - Identify tight coupling points
   - Explain problems with current design

3. **Microservices Planning** (10 min)
   - Design service boundaries
   - Define API contracts
   - Create architecture diagram

4. **Live Extraction** (15 min)
   - Extract Engine module live with Claude Code
   - Create REST API
   - Show before/after comparison

5. **Q&A** (10 min)

## 🤝 Contributing

This is a demo project. Feel free to:
- Add more realistic features
- Improve the simulation logic
- Create additional migration examples
- Add test suites

## 📄 License

MIT License - Use freely for demonstrations and learning.

---

**Built for demonstrating Claude Code capabilities with real-world embedded systems.**
