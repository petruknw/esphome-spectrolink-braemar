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

**Note**: Zone encoding uses bit positions but not sequential. Pattern appears to be bi
