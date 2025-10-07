# 🚀 Quick Start

Get the demo running in under 2 minutes.

## Build and Run

```bash
# Build the project
make

# Run the demo
make demo
```

That's it! You'll see the tractor ECU system start up, run through various operations, and display status.

## What You'll See

```
🚜 Starting Tractor ECU Demo Sequence...

>>> Starting engine...
>>> Shifting to Drive (Gear 2)...
>>> Increasing throttle to 50%...

╔════════════════════════════════════════════════════════════╗
║              TRACTOR ECU MONOLITH STATUS                  ║
╠════════════════════════════════════════════════════════════╣
║ ENGINE:                                                   ║
║   RPM: 1700 / 1700    Status: OK                         ║
║ TRANSMISSION:                                             ║
║   Gear: 2    Output Speed: 485 RPM                        ║
║ HYDRAULICS:                                               ║
║   Pressure: 1955 PSI    Flow: 16.3 GPM                    ║
╚════════════════════════════════════════════════════════════╝
```

## What Next?

### Explore the Code
- **Start here**: `src/main.c` - Main control loop
- **Features**: Each `src/*/` directory is a subsystem
- **Architecture**: See `ARCHITECTURE.md`

### Demo with Claude Code

Open Claude Code and try:

```
"Analyze this tractor ECU codebase"
"Map all dependencies between modules"
"Which module should we extract first?"
"Extract the Engine module into a microservice"
```

### Full Demo Guide
See `DEMO_GUIDE.md` for complete walkthrough with talking points.

## Project Structure

```
tractor-ecu-demo/
├── src/
│   ├── main.c                   # Main control loop
│   ├── engine/                  # Engine control
│   ├── hydraulics/              # Hydraulics control
│   ├── transmission/            # Transmission control
│   ├── diagnostics/             # Fault tracking
│   └── canbus/                  # Communication layer
├── ARCHITECTURE.md              # Detailed design docs
├── DEMO_GUIDE.md                # Full demo walkthrough
├── README.md                    # Complete documentation
└── Makefile                     # Build system
```

## Key Features

**5 Tightly-Coupled Subsystems:**
1. 🔧 **Engine** - RPM, fuel, temperature control
2. 💧 **Hydraulics** - Pressure, PTO, implements
3. ⚙️ **Transmission** - Gears, clutch, speed
4. 🔍 **Diagnostics** - Fault tracking, logging
5. 📡 **CAN Bus** - Inter-module communication

**Perfect for demonstrating:**
- Monolithic architecture analysis
- Dependency mapping
- Microservices extraction
- API design
- Service decomposition

## Troubleshooting

**Build fails?**
```bash
# Check GCC is installed
gcc --version

# Clean and rebuild
make clean && make
```

**Want to see more detail?**
```bash
# Run without demo mode for continuous operation
make run
```

---

**Ready to explore!** Check `DEMO_GUIDE.md` for complete demo scenarios.
