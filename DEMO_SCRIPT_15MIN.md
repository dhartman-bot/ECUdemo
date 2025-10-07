# 🚜 Tractor ECU Demo Script (15-20 Minutes)

**Presenter:** [Your Name]
**Audience:** John Deere Engineering Team
**Goal:** Show how Claude Code accelerates monolith-to-microservices migration

---

## 🎬 SCRIPT

### Introduction (1 minute)

**SAY:**
> "Today I'm going to show you how Claude Code can help modernize legacy embedded systems. We'll use a realistic tractor ECU as our example - it's a monolithic C codebase controlling 5 subsystems. In the next 15 minutes, we'll analyze it, understand its dependencies, and design a microservices architecture - work that would normally take days or weeks."

**DO:**
- Show the tractor-ecu-demo directory
- Briefly explain: "This simulates a real tractor Electronic Control Unit with Engine, Hydraulics, Transmission, Diagnostics, and CAN Bus"

---

## PHASE 1: Explore the Monolith (5 minutes)

### 1.1 Show It Working (1 min)

**SAY:**
> "First, let's see what we're working with. I'll run the demo."

**TYPE:**
```bash
make demo
```

**SAY:** (while it runs)
> "You can see the different subsystems updating - engine RPM, hydraulic pressure, transmission gear, diagnostics monitoring everything. This is our monolithic system where everything is tightly coupled in one process."

**WAIT:** Let it run for 5-10 seconds, then Ctrl+C

---

### 1.2 Understand the Codebase (2 min)

**SAY:**
> "Now let's ask Claude Code to analyze this codebase and tell us what we're working with."

**TYPE IN CLAUDE CODE:**
```
What does this codebase do? Give me a high-level overview.
```

**EXPECTED OUTPUT:**
- Tractor ECU control system
- 5 main subsystems
- Simulation of agricultural equipment control

**SAY:**
> "Good. Now let's understand the structure."

**TYPE IN CLAUDE CODE:**
```
Show me the 5 main modules/subsystems and what each one does.
```

**EXPECTED OUTPUT:**
- Engine Control (engine.c) - RPM management, throttle
- Hydraulics (hydraulics.c) - Pressure control for implements
- Transmission (transmission.c) - Gear selection
- Diagnostics (diagnostics.c) - Fault monitoring
- CAN Bus (can_bus.c) - Inter-module communication

**HIGHLIGHT:**
> "This is typical of agricultural equipment - decades-old monolithic code that needs modernization."

---

### 1.3 Identify the Problem (2 min)

**SAY:**
> "The problem with this architecture is tight coupling. Let me show you."

**TYPE IN CLAUDE CODE:**
```
Show me all the dependencies between these modules. Which modules directly call functions from other modules?
```

**EXPECTED OUTPUT:**
- Hydraulics reads Engine RPM
- Transmission reads Engine RPM
- All modules write to CAN Bus
- All modules report to Diagnostics

**SAY:**
> "See this? Everything is reaching into everything else. If we want to update the Engine module, we risk breaking Hydraulics and Transmission. This is why modernization is hard."

---

## PHASE 2: Analyze Dependencies (5 minutes)

### 2.1 Deep Dive on Coupling (2 min)

**SAY:**
> "Let's get specific about these dependencies."

**TYPE IN CLAUDE CODE:**
```
Show me the exact lines of code where hydraulics.c and transmission.c depend on engine.c
```

**EXPECTED OUTPUT:**
- hydraulics.c line 20: calls engine_get_state()
- transmission.c line 21: calls engine_get_state()

**SAY:**
> "Both modules are directly calling engine functions. This tight coupling means we can't deploy or update them independently."

---

### 2.2 Find Migration Candidate (3 min)

**SAY:**
> "Here's the key question for any modernization project: what do we extract first?"

**TYPE IN CLAUDE CODE:**
```
Which module would be easiest to extract into a microservice first, and why?
```

**EXPECTED OUTPUT:**
- Engine module is the best candidate
- It has NO incoming dependencies
- Other modules depend on it (can be replaced with API calls)
- Clear boundary and state

**SAY:**
> "This is gold. Claude identified that Engine is the perfect starting point because nothing depends on it internally - other modules just read its state. This makes it a clean extraction."

**POINT OUT:**
> "In a real modernization project, this analysis alone could save you weeks of architecture meetings and code reviews."

---

## PHASE 3: Design Microservices Architecture (5-7 minutes)

### 3.1 Generate Architecture (3 min)

**SAY:**
> "Now let's design the target state - what should this look like as microservices?"

**TYPE IN CLAUDE CODE:**
```
Design a microservices architecture for this tractor ECU system. Show me what services we'd have and how they communicate.
```

**EXPECTED OUTPUT:**
- 4-5 independent services
- API Gateway for routing
- Message broker (MQTT) for real-time telemetry
- Each service independently deployable

**SAY:**
> "Claude is suggesting a clean separation of concerns. Each subsystem becomes its own service with well-defined APIs."

---

### 3.2 Define Service APIs (2 min)

**SAY:**
> "Let's get concrete - what would the Engine service API look like?"

**TYPE IN CLAUDE CODE:**
```
What REST API endpoints should the Engine service expose? Include request/response formats.
```

**EXPECTED OUTPUT:**
- POST /engine/start
- POST /engine/stop
- POST /engine/throttle (with body: {percent: 0-100})
- GET /engine/state (returns: RPM, status, temperature)
- GET /engine/health

**SAY:**
> "Perfect. We now have a clear API contract. Any module that needs engine data would call these endpoints instead of direct function calls."

---

### 3.3 Show the Architecture Diagram (2 min)

**SAY:**
> "Let me show you what this architecture looks like."

**DO:**
- Open DEMO_GUIDE.md
- Scroll to the architecture diagram (lines 60-80)
- Show the target architecture:

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

**SAY:**
> "This is our target state:
> - Each subsystem is an independent service
> - They communicate via REST APIs for commands
> - Real-time telemetry flows through MQTT
> - We can deploy, scale, and update each service independently
> - If Hydraulics crashes, Engine keeps running"

---

## CLOSING (2 minutes)

### Summary

**SAY:**
> "So what did we just accomplish in 15 minutes?
>
> ✅ We analyzed a complex monolithic codebase
> ✅ We identified all the dependencies and coupling
> ✅ We found the optimal module to extract first
> ✅ We designed a complete microservices architecture
> ✅ We defined the API contracts
>
> Normally, this analysis would take a team of engineers several days to weeks - architecture reviews, whiteboard sessions, code archaeology. Claude Code did it conversationally in minutes.
>
> And this is just the analysis phase. In Phase 4, which we don't have time for today, Claude can actually *write the code* to extract the Engine service - create the Flask API, add MQTT, generate a Dockerfile, update the main controller. The whole migration."

---

### Why This Matters for John Deere

**SAY:**
> "Why does this matter for agricultural equipment?
>
> 1. **Legacy Modernization**: You have decades of embedded C code that needs to move to the cloud for IoT and telematics
>
> 2. **Risk Reduction**: Incremental migration lets you modernize without a risky big-bang rewrite
>
> 3. **Feature Velocity**: Once services are separated, different teams can work independently
>
> 4. **Over-the-Air Updates**: Update the Engine service without touching Hydraulics or Transmission
>
> 5. **Shared Services**: One Engine service can support tractors, combines, and sprayers"

---

### Call to Action

**SAY:**
> "Questions? I can also show you Phase 4 where we live-code the extraction if we have more time, or we can dive deeper into any of the areas we covered."

---

## 📋 PREPARATION CHECKLIST

Before the demo:

```bash
□ cd /Users/dhartman/tractor-ecu-demo
□ Run `make clean && make` to ensure it builds
□ Test `make demo` works
□ Have Claude Code open and authenticated
□ Have DEMO_GUIDE.md open in another window
□ Prepare to share your screen
□ Close unnecessary applications
□ Turn off notifications
□ Test your microphone
```

---

## 🎯 TIMING BREAKDOWN

- Introduction: 1 min
- Phase 1 (Explore): 5 min
  - Show demo running: 1 min
  - Understand codebase: 2 min
  - Identify problems: 2 min
- Phase 2 (Dependencies): 5 min
  - Deep dive coupling: 2 min
  - Find candidate: 3 min
- Phase 3 (Architecture): 5 min
  - Generate architecture: 3 min
  - Define APIs: 2 min
  - Show diagram: 2 min (overlap with above)
- Closing: 2 min

**Total: 18 minutes** (with 2 min buffer for questions)

---

## 🔧 BACKUP PLANS

**If Claude Code is slow:**
- Have pre-generated answers in comments below each prompt
- Can read them as "Claude says..."

**If demo won't compile:**
- Show the code walkthrough instead
- Focus on the analysis and architecture design

**If you run over time:**
- Skip 3.2 (API endpoints) - just show architecture diagram
- Cut introduction to 30 seconds

**If you have extra time:**
- Show the actual code in engine.c
- Demonstrate one extraction step from Phase 4
- Take deeper questions

---

## 💬 ANTICIPATED QUESTIONS & ANSWERS

**Q: "How long would Phase 4 actually take?"**
A: "With Claude Code, extracting the Engine service takes about 15-20 minutes of guided conversation. Without it, a skilled engineer might need 4-6 hours. The quality is comparable."

**Q: "Does this work with other languages besides C?"**
A: "Yes, Claude Code understands all major languages. This same process works for Java monoliths, .NET, Python, Go - any codebase."

**Q: "What about testing the extracted services?"**
A: "Great question. Claude can generate unit tests, integration tests, and even contract tests to ensure the services work correctly. We didn't cover that today but it's part of Phase 4."

**Q: "Can it handle larger codebases?"**
A: "Claude Sonnet 4.5 has a 200K token context window - that's about 50,000+ lines of code. For truly massive codebases, you'd work module-by-module, which is actually the recommended migration approach anyway."

**Q: "What's the cost?"**
A: "Claude API costs about $3 per million input tokens and $15 per million output tokens. For a project like this, you'd spend maybe $20-50 total. Compare that to weeks of engineering time at $150K/year salaries."

**Q: "Is the generated code production-ready?"**
A: "The code Claude generates is high quality and follows best practices, but you should still review it. Think of Claude as a senior engineer pair programmer - very capable, but you're still the architect and decision maker."

---

**You're ready to demo!** 🚀
