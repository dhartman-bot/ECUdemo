# 🚜 Tractor ECU Demo Guide

Quick reference for demonstrating the monolith-to-microservices journey with Claude Code.

## 🎬 Demo Flow (30-45 minutes)

### Phase 1: Explore the Monolith (10 min)

**Goal**: Understand what we're working with

```bash
# Build and run the demo
make demo
```

**Ask Claude Code:**
1. `"What does this codebase do?"`
2. `"Show me the 5 main features/modules"`
3. `"Map out all dependencies between modules"`
4. `"Which modules are tightly coupled?"`

**Expected insights:**
- 5 subsystems: Engine, Hydraulics, Transmission, Diagnostics, CAN Bus
- Tight coupling through direct includes and function calls
- Shared state via getter functions
- CAN bus acts as core communication layer

---

### Phase 2: Analyze Dependencies (10 min)

**Goal**: Identify problems and migration candidates

**Ask Claude Code:**
1. `"Show me all places where hydraulics.c depends on engine.c"`
2. `"What data does the Engine module send to other modules?"`
3. `"Which module would be easiest to extract first?"`
4. `"Create a dependency graph showing all relationships"`

**Key findings to highlight:**
- **Hydraulics** reads Engine RPM (line 20 in hydraulics.c)
- **Transmission** reads Engine RPM (line 21 in transmission.c)
- **All modules** send data to CAN Bus
- **All modules** report faults to Diagnostics
- **Engine has no incoming dependencies** → Best extraction candidate!

---

### Phase 3: Plan Microservices Architecture (10 min)

**Goal**: Design the target state

**Ask Claude Code:**
1. `"Design a microservices architecture for this system"`
2. `"What API endpoints should the Engine service expose?"`
3. `"How should services communicate? REST, gRPC, or MQTT?"`
4. `"Create a docker-compose.yml for running all services"`

**Target architecture:**
```
┌─────────────────┐
│   API Gateway   │
└────────┬────────┘
         │
    ┌────┴─────────────────────────────┐
    │                                   │
    ▼                                   ▼
┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐
│ Engine │  │Hydraul.│  │Transmi.│  │Diagnos.│
│Service │  │Service │  │Service │  │Service │
└───┬────┘  └───┬────┘  └───┬────┘  └───┬────┘
    │           │            │            │
    └───────────┴────────────┴────────────┘
                     │
                     ▼
              ┌────────────┐
              │MQTT Broker │
              │(Mosquitto) │
              └────────────┘
```

---

### Phase 4: Extract Engine Service (15+ min)

**Goal**: Live code the migration

**Ask Claude Code:**
1. `"Extract the Engine module into a standalone Python service with Flask"`
2. `"Create REST endpoints for: start, stop, set_throttle, get_state"`
3. `"Add MQTT publishing for real-time RPM telemetry"`
4. `"Create a Dockerfile for the Engine service"`
5. `"Update the main controller to use HTTP calls instead of direct includes"`

**Expected artifacts:**
```
services/
├── engine-service/
│   ├── app.py              # Flask REST API
│   ├── engine.py           # Business logic (ported from C)
│   ├── mqtt_client.py      # Telemetry publisher
│   ├── requirements.txt    # Python dependencies
│   └── Dockerfile          # Container image
├── docker-compose.yml      # Multi-service orchestration
└── README.md               # Service documentation
```

**API endpoints created:**
- `POST /engine/start` - Start the engine
- `POST /engine/stop` - Stop the engine
- `POST /engine/throttle` - Set throttle (0-100%)
- `GET /engine/state` - Get current engine state
- `GET /engine/health` - Health check

**MQTT topics:**
- `tractor/engine/rpm` - Current RPM (published every 100ms)
- `tractor/engine/faults` - Fault codes
- `tractor/engine/telemetry` - Full state snapshot

---

## 🎯 Key Talking Points

### Why This Matters for John Deere

1. **Legacy Codebase Management**
   - Agricultural equipment has decades-old code
   - Need to modernize without breaking existing systems
   - Incremental migration reduces risk

2. **IoT and Connectivity**
   - Modern tractors need cloud connectivity
   - Telematics for fleet management
   - Over-the-air updates

3. **Scalability**
   - Different equipment types (tractors, combines, sprayers)
   - Shared services across product lines
   - Independent scaling of features

4. **Maintenance and Updates**
   - Update individual services without downtime
   - A/B testing new features
   - Rollback capabilities

### Problems Solved by Microservices

| Monolith Problem | Microservices Solution |
|------------------|------------------------|
| Tight coupling | Independent services |
| Single point of failure | Fault isolation |
| Slow deployment | Deploy services individually |
| Technology lock-in | Use best tool per service |
| Hard to test | Mock service interfaces |
| Scaling limitations | Scale hot services only |

---

## 🔧 Commands Cheat Sheet

```bash
# Build and run
make demo                    # Run automated demo
make run                     # Run continuous mode
make clean                   # Clean build artifacts

# Development
make                         # Build project
./build/ecu_controller       # Run manually

# Docker (after service extraction)
docker-compose up            # Start all services
docker-compose logs engine   # View engine service logs
docker-compose down          # Stop all services

# Testing extracted service
curl -X POST http://localhost:5000/engine/start
curl http://localhost:5000/engine/state
curl -X POST http://localhost:5000/engine/throttle -d '{"percent": 75}'
```

---

## 📊 Demo Variations

### Short Version (15 min)
1. Show running demo
2. Quick code walkthrough
3. Explain dependencies
4. Show extraction plan (no live coding)

### Medium Version (30 min)
1. Explore monolith with Claude Code
2. Analyze dependencies
3. Live extract Engine service API
4. Demo the new service

### Long Version (45-60 min)
1. Full exploration and analysis
2. Extract Engine service completely
3. Add MQTT messaging
4. Create docker-compose
5. Show multiple services running
6. Discuss migration of remaining modules

---

## 🎤 Claude Code Prompts Library

### Discovery Phase
```
"Analyze this ECU codebase and create a summary"
"Show me all the module boundaries"
"Which modules have the highest coupling?"
"Find all the global state in this codebase"
"What are the API boundaries between modules?"
```

### Dependency Analysis
```
"Map all dependencies from hydraulics to other modules"
"Show everywhere that engine_get_state() is called"
"What happens if the Engine module crashes?"
"Create a call graph for the update() functions"
"Which CAN message IDs are used by each module?"
```

### Refactoring
```
"Extract Engine control into a separate directory"
"Create an interface header for Engine module"
"Remove direct coupling between Hydraulics and Engine"
"Add dependency injection for CAN bus communication"
"Create mock objects for testing Transmission in isolation"
```

### Microservices Migration
```
"Convert Engine module to a Python Flask service"
"Design REST API for remote engine control"
"Add MQTT for real-time telemetry streaming"
"Create a service discovery mechanism"
"Implement circuit breakers for service calls"
"Add health checks and readiness probes"
```

### Documentation
```
"Generate OpenAPI spec for Engine service"
"Create sequence diagrams for engine start procedure"
"Document the migration strategy from monolith to services"
"Write integration tests for service boundaries"
```

---

## 🚦 Success Criteria

By the end of the demo, attendees should understand:

✅ What a monolithic architecture looks like in embedded systems
✅ How to identify dependencies and coupling
✅ Service boundary identification strategies
✅ API design for extracted services
✅ Communication patterns (REST, MQTT, gRPC)
✅ Benefits and tradeoffs of microservices
✅ Practical migration steps
✅ How Claude Code accelerates this work

---

## 💡 Follow-up Ideas

### For Deeper Dives:
1. **Add more realism**: Implement actual CAN bus protocol (SocketCAN)
2. **Security**: Add authentication, TLS, certificate management
3. **Observability**: Add Prometheus metrics, Grafana dashboards, tracing
4. **Service mesh**: Deploy on Kubernetes with Istio
5. **Testing**: Add unit tests, integration tests, contract tests
6. **CI/CD**: GitHub Actions pipeline for automated deployment

### Additional Modules to Extract:
- Hydraulics → Python service with hardware simulator
- Transmission → Go service for performance
- Diagnostics → Centralized logging service (ELK stack)
- CAN Bus → Replace with MQTT/Kafka message broker

---

**Ready to demo!** 🎉

For questions or customization help, check the main README.md or ARCHITECTURE.md.
