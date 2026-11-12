# Robot code

Same layout as the original project: `main.cpp` handles the controller and
competition modes, `robot.cpp` defines the hardware and intake, `autons.cpp`
contains the routines, and `interface.cpp` draws the selector. Headers are back
in `include/hteam/`. Calls still look like `robot->chassis.moveToPoint(...)`.

## Set up the new robot

- Fill in the **0 placeholders** in `Robot::Robot()` and `Intake::Intake()` in
  `src/robot.cpp`: ports, drivetrain dimensions/gearing, tracking wheel, and IMU.
  Pneumatic port 0 means unused; replace it with an ADI letter to use it.
- Tune the lateral/angular gains there too. The old gains have been zeroed.
- Fill in the intake voltages in `Intake::intakeTask()`. All modes start at zero.
- Set `ROBOT_CONFIGURED` to true near the top of `src/main.cpp` when the hardware
  settings are ready. Until then only the selector runs.
- Write the new routes in `Autons::red1()` through `blue4()` and `skills()`.
  Every original routine is still there, with its old route emptied.

`initialize()`, `disabled()`, `competition_initialize()`, `autonomous()`, and
`opcontrol()` are still in main.cpp. The R1/R2/L2 intake examples and B/L1/Down
pneumatic bindings are there to change. The old cube/park automation is removed.

## Toggles

Pneumatic names stay the same. The helper in `hteam/utilities.h` holds the state
and press tracking so there aren't separate booleans to update:

```cpp
robot->matchLoaderPneumatic.set_value(true); // autonomous: known state
robot->matchLoaderPneumatic.update(controller.get_digital(pros::E_CONTROLLER_DIGITAL_B));
// Call update every driver loop. The next press toggles from the last set_value.
```

`Toggle` is still in `functions.h` for other uses: `update(heldButton)`, `get()`,
and `set(value)`. The driver-entry reset calls just ignore a button already held
when the mode begins.

## Tasks

The task functions live in the classes in robot.cpp, not in a new framework.
`startIntakeTask()`, `startRobotTask()`, and `startInterfaceTask()` are called once
in initialize. Their handles are kept as members; no extra taskRunning flags.
Don't delete/recreate them between competition modes.

The intake functions select an atomic mode; the intake task writes the motors.
Pneumatic setters update an atomic state; the robot/intake tasks apply it. No
mechanism mutex is held by a competition callback that PROS might delete.
`disabled()` and mode entry stop the intake, and its task also gates motor output
while disabled. Drivetrain motors, sensors, and `chassis` are still public.

Screen drawing stays in the interface task. Avoid screen prints inside
opcontrol/autonomous or while holding a mechanism mutex. LemLib moves are async
by default; use `waitUntilDone()` when the next action needs the move to finish.

Build with `pros make` or `make`. `make test` checks toggles and queued pneumatic
commands on the computer using a fake ADI device. Motor directions, PID tuning,
and competition transitions still need checking on the robot.
