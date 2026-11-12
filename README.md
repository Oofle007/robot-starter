# Robot starter

PROS + LemLib, with split arcade drive and empty autonomous routines.
The old game's mechanisms, paths, and tuned settings have been removed.

## Start here

1. Set the ports, wheel sizes, and gearing in `include/robot/config.hpp`.
   The wiring there is an example, not the new team's wiring.
2. Check motor direction, then tune the lateral and angular gains in
   `src/robot.cpp`. They start at zero. Driver control works without PID gains;
   autonomous movement needs them tuned.
3. Add driver bindings in `src/main.cpp` and moves in `src/autons.cpp`.
4. Build with `pros make` (or `make` with the ARM toolchain on PATH).
   Upload with `pros upload` once the hardware settings match the robot.

The brain's left/right LCD buttons select **None**, **Routine 1**, or **Skills**
while disabled. Nothing drives in any routine until you add moves. Wait for IMU
calibration to finish before enabling. Without a competition switch, PROS enters
operator control after initialization.

## Where things live

| File | What to change |
| --- | --- |
| `include/robot/config.hpp` | Ports, dimensions, optional devices, loop periods |
| `include/robot/robot.hpp` | Hardware members; motors and chassis stay public |
| `src/robot.cpp` | Hardware construction, PID gains, task startup |
| `src/main.cpp` | Competition callbacks and controller bindings |
| `src/autons.cpp` | Autonomous routines |
| `src/interface.cpp` | Selector and slow status display |
| `include/robot/utilities.hpp` | Persistent task helper and pneumatic wrapper |
| `include/robot/toggle.hpp` | Button toggle, with its own press tracking |
| `src/motor_subsystem.cpp` | Example of a motor controlled by a background task |

Use `robot.leftMotors`, `robot.rightMotors`, `robot.imu`, and `robot.chassis`
directly. The hardware isn't behind a framework. Odometry uses drive encoders by
default; enable and measure the optional tracking wheel in config if you have one.

## Mechanisms and toggles

Set `auxiliaryMotorPort` to a smart port to enable the motor example.
R1 runs it forward, R2 reverses it, and both together stop it. Voltage is in mV,
clamped to -12000 through 12000. Set the port to 0 to leave it out.

Set `pneumaticPort` to an ADI letter to enable the pneumatic example. B toggles
it once per press. Autonomous uses the same state:

```cpp
robot.pneumatic->set(true);          // next B press will turn it off
robot.auxiliary->setVoltage(6000);   // task applies the command within one loop
robot.auxiliary->stop();
```

Those examples assume the devices are enabled. Guard optional devices with
`if (robot.auxiliary)` or `if (robot.pneumatic)` as main.cpp does. Pneumatics keep
their state through mode changes; motor commands are cleared at each mode entry.
Add new motor subsystems to `Robot::stopSubsystems()` too.

For another button toggle, use `Utilities::Toggle` and pass the raw held-button
value to `update()` every loop. Use `set()` for a known state and `get()` to read
it. No separate "button pressed" variable is needed. `resetInput(currentButton)`
at driver entry prevents a button already held from counting as a fresh press.

## Background tasks

The optional motor, pneumatic, and display tasks start in `initialize()` and
stay alive between modes. Commands use atomics; setters don't take a mutex or
write to a motor. The motor task gates output while disabled. The display is the
only task updating the LCD, and it runs at 4 Hz.

To add a task, keep a `Utilities::PeriodicTask` as a permanent member:

```cpp
static void step(void* context); // one update, then return; no while(true) here
Utilities::PeriodicTask task{"My mechanism", step, this, 20};
// In initialize(): check task.start() for failure.
```

Keep its object and context alive for the whole program. Don't remove/recreate
tasks on mode changes or start them inside an auton. `start()` is idempotent and
must be called from initialize, not concurrently. `healthy()` checks a heartbeat;
`state()`, `handle()`, and `iterations()` are available for debugging. A healthy
task has completed an update recently, not necessarily reached its motor target.

For a PID mechanism, add an atomic target and keep the sensor reads, PID update,
and motor write in `step()`. Keep screen calls and locks out of that control loop.
A mutex held by a deleted competition callback can stay locked; even a scope
lock doesn't solve abrupt task deletion. Don't recover by spawning duplicates.

LemLib still manages its own motion/odometry tasks. Its moves are asynchronous
by default: use `waitUntilDone()` before a mechanism action that depends on the
move finishing. Driver entry cancels queued autonomous moves. Don't command the
same motors from a mechanism task and driver control at the same time.

## Checks

`make test` checks toggles, task creation/heartbeats, and mechanism commands on
your computer. The task checks use a small fake PROS runtime, not a robot
simulator. `make` compiles and links the actual V5 program. Neither replaces checking motor direction,
tracking wheel offsets, PID tuning, and disable/enable transitions on the robot.

The bundled libraries are PROS 4.1.0 and LemLib 0.5.4. `firmware/` and the other
library headers belong to those dependencies; team code lives in `src/` and
`include/robot/`.
