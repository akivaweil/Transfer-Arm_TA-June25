# Transfer Arm State Machine

## State Structure Overview

The state machine is organized based on the **physical position** and **movement** of the transfer arm, not the operations being performed.

## Configuration Organization

**All configuration settings are now completely local to each state file** - no need to go to the config file!

- **Homing Configuration**: Movement distances (in inches), speeds, and timing
- **Pickup Configuration**: Vacuum activation positions (in inches) and timing
- **Transport Configuration**: Servo angles, rotation delays, and X positions (in inches)
- **Dropoff Configuration**: Speed settings, wait times, and Z positions (in inches)
- **Return Configuration**: Servo angles and movement sequences (in inches)

## Main States

### `00_IDLE.cpp`
- **Position**: Stationary at pickup position
- **Purpose**: Wait for trigger signals (start button or stage1 signal)
- **Next State**: `02_AT_PICKUP_POSITION` when triggered

### `01_HOMING.cpp`
- **Position**: Moving to home position
- **Purpose**: Home both Z and X axes sequentially
- **Configuration**: Movement distances (inches), speeds, positions (inches), and timing (all local)
- **Next State**: `00_IDLE` when complete
- **Usage**: Initial startup AND after each pick cycle completion

## Pick Cycle States (PICK_CYCLE subfolder)

### `02_AT_PICKUP_POSITION.cpp`
- **Position**: Stationary at pickup position
- **Purpose**: Lower Z, activate vacuum, wait, raise Z
- **Configuration**: Vacuum activation position (inches), pickup wait time, Z positions (inches) (all local)
- **Next State**: `03_MOVING_TO_DROPOFF` when complete

### `03_MOVING_TO_DROPOFF.cpp`
- **Position**: Moving from pickup to dropoff
- **Purpose**: Rotate servo, move to overshoot, rotate servo again, move to dropoff
- **Configuration**: Servo angles, rotation delays, X positions (inches) (all local)
- **Next State**: `04_AT_DROPOFF_POSITION` when complete

### `04_AT_DROPOFF_POSITION.cpp`
- **Position**: Stationary at dropoff position
- **Purpose**: Check safety signal, lower Z, release vacuum, wait, raise Z
- **Configuration**: Dropoff speeds, wait times, Z positions (inches) (all local)
- **Next State**: `05_MOVING_TO_PICKUP` when complete

### `05_MOVING_TO_PICKUP.cpp`
- **Position**: Moving from dropoff back to pickup
- **Purpose**: Turn off Stage 2 signal, reset servo, move X home, then to pickup
- **Configuration**: Servo home angle, movement sequences (inches) (all local)
- **Next State**: `01_HOMING` when complete (automatic homing)

## State Flow

```
IDLE → AT_PICKUP_POSITION → MOVING_TO_DROPOFF → AT_DROPOFF_POSITION → MOVING_TO_PICKUP → HOMING → IDLE
```

## Automatic Homing Feature

**Your Transfer Arm now automatically homes after each pick cycle!** This ensures:

- **Position Accuracy**: Returns to known good positions after each operation
- **Drift Prevention**: Eliminates cumulative positioning errors
- **Reliability**: Consistent performance over long running periods
- **Safety**: Always starts from a known reference position

### **Homing Occurs:**
1. **At startup** - Initial system calibration
2. **After each pick cycle** - Automatic accuracy maintenance
3. **When manually triggered** - For maintenance or troubleshooting

## Configuration Benefits

1. **Truly Local**: All settings are in each state file - no config file needed
2. **Easy Access**: Change timing, speeds, positions directly in the state file
3. **Quick Modification**: Adjust values without searching through code or config files
4. **Clear Documentation**: Each constant is clearly labeled and commented
5. **Maintainability**: Configuration is separated from logic and easily accessible
6. **Human Readable**: Position values are in inches, not abstract step counts
7. **Completely Self-Contained**: No external config file dependencies
8. **Automatic Maintenance**: Self-homing ensures long-term accuracy

## File Naming Convention

- Main states: `00_`, `01_` prefixes
- Pick cycle states: `02_` through `05_` prefixes
- All states use descriptive names based on position/movement
- Configuration sections are clearly marked with banners

## Configuration Sections

Each state file contains these organized sections:
1. **Includes and Headers**
2. **Configuration Constants** (at the top - all local values)
3. **External References**
4. **State Logic**
5. **Helper Functions** (if any)

## Example Configuration Values

### Homing State (`01_HOMING.cpp`)
```cpp
const float Z_HOMING_DISTANCE_INCHES = -5.0;              // Distance to move Z when homing (inches)
const uint32_t Z_HOMING_SPEED = 1000;                     // Z homing speed (steps/sec)
const float Z_UP_POSITION_INCHES = 5.0;                   // Z position when fully up (inches)
const float X_PICKUP_POSITION_INCHES = 5.0;               // X position for pickup (inches)
```

### Pickup State (`02_AT_PICKUP_POSITION.cpp`)
```cpp
const float VACUUM_ACTIVATION_INCHES = 2.5;               // Z position to start vacuum (inches from top)
const float Z_PICKUP_POSITION_INCHES = 3.0;               // Z position for pickup (inches from home)
const float Z_UP_POSITION_INCHES = 5.0;                   // Z position when fully up (inches from home)
```

### Transport State (`03_MOVING_TO_DROPOFF.cpp`)
```cpp
const float X_OVERSHOOT_POSITION_INCHES = 7.5;            // X overshoot position (inches from home)
const float X_DROPOFF_POSITION_INCHES = 10.0;             // X position for dropoff (inches from home)
```

### Dropoff State (`04_AT_DROPOFF_POSITION.cpp`)
```cpp
const float Z_DROPOFF_POSITION_INCHES = 3.0;              // Z position for dropoff (inches from home)
const uint32_t DROPOFF_Z_SPEED = 500;                     // Z speed during dropoff (steps/sec)
```

### Return State (`05_MOVING_TO_PICKUP.cpp`)
```cpp
const float RETURN_X_HOME_INCHES = 0.0;                   // X position to return to home (inches)
const float RETURN_X_PICKUP_INCHES = 5.0;                 // X position to move to after home (inches)
```

## Units Used

- **Positions**: Inches (converted to steps automatically)
- **Speeds**: Steps per second (Hz) - kept as-is for stepper library compatibility
- **Timing**: Milliseconds
- **Servo**: Degrees
- **Distances**: Inches (negative = left/down, positive = right/up)

## Complete Self-Containment

**Your Transfer Arm is now completely self-contained with automatic homing!** All configuration values can be adjusted directly in each state file:

- **No config file needed** for any adjustments
- **All positions in inches** for easy understanding
- **All speeds in steps/sec** for stepper library compatibility
- **All timing in milliseconds** for human readability
- **Easy to modify** without searching through multiple files
- **Automatic accuracy maintenance** after each cycle

**All values can be adjusted directly in these files - no need to touch any config files!**
