# Spectrolink Protocol Documentation

## Overview

The Braemar Spectrolink system uses a **TX-only 300 baud UART protocol**. The controller sends 8-byte frames every 1000ms to command the heater. The heater applies commands immediately from each valid frame with no acknowledgment.

## Message Types

The protocol uses two primary message types:

### Type 202: Standard Control Messages

These are the primary control frames sent every second when in the main sequence.

**Frame Format:**
```
[ZONES][CMD][MODE][STATE][FLAG][CHECKSUM]
```

### Type 218: Status/Configuration Messages

Extended status messages sent periodically (messages 2-4, 10-12 in rotation sequence).

**Frame Format:**
```
[MSG_TYPE][DATA2][DATA3][DATA4][STATE][DATA6][CHECKSUM]
```

## Standard Control Frame (Type 202)

### Byte 0: Header
- **Value**: `202` (0xCA)
- **Purpose**: Frame identification

### Byte 1: Reserved
- **Value**: `0`
- **Purpose**: Unused in standard messages

### Byte 2: ZONES
Zone selection byte determines which zones are active.

| Value | Binary   | Zones Active | Description |
|-------|----------|--------------|-------------|
| 167   | 10100111 | 1, 2, 3      | All zones   |
| 166   | 10100110 | 2, 3         | Zones 2&3   |
| 165   | 10100101 | 1, 3         | Zones 1&3   |
| 164   | 10100100 | 3            | Zone 3 only |
| 163   | 10100011 | 1, 2         | Zones 1&2   |
| 162   | 10100010 | 2            | Zone 2 only |
| 161   | 10100001 | 1            | Zone 1 only |
| 160   | 10100000 | None         | All zones off (cooling) |
| 128   | 10000000 | None         | All zones off (heating) |

**Note**: Zone encoding uses bit positions but not sequential. Pattern appears to be bits 0, 1, 2, 5, 7 active.

### Byte 3: CMD
Command byte controls system power and special functions.

| Value | Function |
|-------|----------|
| 8     | System ON |
| 0     | System OFF |
| 32    | Reset/Initialize |

### Byte 4: MODE
Operating mode selection.

| Value | Mode    | Description |
|-------|---------|-------------|
| 128   | Cool    | Air conditioning mode |
| 64    | Heat    | Heating mode |
| 0     | Off     | No mode selected |

### Byte 5: STATE
Current system state (may be informational or part of command).

| Value | State | Description |
|-------|-------|-------------|
| 74    | OFF   | System off/idle |
| 202   | COOL  | Actively cooling |
| 138   | HEAT  | Actively heating |
| 90    | TRANSITION | Heating → Cooling |
| 26    | TRANSITION | Cooling → Heating |
| 10    | TRANSITION | Heating → Off |

### Byte 6: FLAG
- **Value**: `1` when system ON
- **Value**: `0` when system OFF or during reset

### Byte 7: CHECKSUM
Calculated checksum varies based on all previous bytes. Exact algorithm unknown but appears to be a simple XOR or sum-based calculation.

## Example Frames

### Heat All Zones
```

```
- Header: 202
- Zones: 167 (all zones)
- Command: 8 (ON)
- Mode: 64 (heat)
- State: 138 (heating)
- Flag: 1 (active)
- Checksum: 144

### Cool Zone 1 Only
```

```
- Zones: 161 (zone 1)
- Command: 8 (ON)
- Mode: 128 (cool)
- State: 202 (cooling)
- Checksum: 75

### System Off
```

```
- Command: 0 (OFF)
- Mode: 0 (none)
- State: 74 (off)
- Flag: 0 (inactive)
- Checksum: 104

### Reset Command
```

```
- Command: 32 (reset)
- All other bytes: 0
- Checksum: 126

## Status Messages (Type 218)

The controller sends Type 218 messages in a rotating sequence along with Type 202 messages:

**Message Sequence**: `[1, 1, 1, 2, 1, 3, 1, 4, 1, 1, 1, 10, 1, 11, 1, 12]`

Where:
- `1` = Type 202 standard control message
- `2-4` = Type 218 low-range messages
- `10-12` = Type 218 high-range messages

### Type 218 Frame Structure

```
[MSG_TYPE][DATA2][DATA3][DATA4][STATE][DATA6][CHECKSUM]
```

**Byte 1 (MSG_TYPE)**: Message identifier (2, 3, 4, 10, 11, or 12)

**Bytes 2-6**: Data varies by mode and message type:

#### Cooling Mode Data
- Byte 2: `48`
- Byte 3: `56`
- Byte 4: `44`
- Byte 5: Current STATE
- Byte 6: `48`

#### Heating Mode Data
- Byte 2: `40`
- Byte 3: `30`
- Byte 4: `42`
- Byte 5: Current STATE
- Byte 6: `30`

**Byte 7**: Checksum (varies by state and message type)

### Purpose of 218 Messages

Unknown - possibly:
- Temperature setpoint/readings
- Time synchronization
- Status reporting to slave units
- Extended configuration data

## Timing

- **Frame Period**: 1000ms (1 second)
- **Baud Rate**: 300 baud
- **Data Format**: 8N1 (8 bits, no parity, 1 stop bit)
- **Direction**: TX only (no response expected)

## Reset Behavior

When a reset is triggered:
1. Send reset frame with CMD=32 for 10 consecutive frames
2. Set all zones ON (167)
3. Clear mode and state bytes
4. After 10 frames, return to normal operation

## Implementation Notes

1. **No Feedback**: The heater does not send acknowledgments. Controller must assume commands are received.

2. **Continuous Transmission**: Controller must send frames every 1000ms continuously to maintain state.

3. **Zone Limitation**: Original Arduino implementation disables Zone 1 when all 3 zones active in heating mode to prevent system failure.

4. **Cooldown Timers**: 
   - Air conditioning: 30 second cooldown after turning off
   - Heating: 180 second (3 minute) cooldown after turning off
   - Maximum heating runtime: 1800 seconds (30 minutes)

5. **Auto-Reset**: System performs automatic reset every 300 seconds (5 minutes) when off.

## Checksum Algorithm

**Status**: Not fully reverse-engineered

The checksum appears to be calculated from bytes 0-6. Common algorithms to test:
- XOR of all bytes
- Sum of all bytes (with or without carry)
- CRC-8 variant

Each zone/mode/state combination has a unique checksum value hardcoded in the original implementation.

## Unknown Elements

1. **Fan Speed**: Not observed in protocol. May be controlled locally by heater or in Type 218 messages.

2. **Temperature Control**: No explicit temperature setpoint in frames. Possibly encoded in Type 218 data bytes.

3. **Type 218 Data Bytes**: Purpose of specific values (48, 56, 44, etc.) in cooling/heating modes unclear.

4. **Zone Encoding Pattern**: Why specific bit pattern (bits 0,1,2,5,7) instead of sequential?

## References

Original reverse engineering by [diablosv21/spectrolink2nest](https://github.com/diablosv21/spectrolink2nest)

## Testing & Validation

To capture additional protocol details:
1. Connect logic analyzer to Spectrolink TX line
2. Set to 300 baud, 8N1
3. Capture frames while changing:
   - Temperature setpoints
   - Fan speeds (if available)
   - Different operating modes
4. Compare byte patterns to identify encoding
