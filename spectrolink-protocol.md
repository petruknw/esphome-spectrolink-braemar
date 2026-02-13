# Spectrolink Protocol Documentation

This document describes the communication protocol used by the Braemar Spectrolink heater. The protocol is based on a TX-only UART communication at 300 baud.

## Frame Structure

Each frame consists of 8 bytes:

```
[ZONES][CMD][MODE][STATE][CHECKSUM][1]
```

### Byte Definitions
- **ZONES**: Specifies the active zones.
- **CMD**: Command byte.
- **MODE**: Operating mode (e.g., heat, cool, off).
- **STATE**: Current state of the heater.
- **CHECKSUM**: Checksum for frame validation.
- **1**: Constant byte.

### Example Frames
- Heat all zones: `[202, 0, 167, 8, 64, 138, 1, 144]`
- Off: `[202, 0, 167, 0, 0, 74, 0, 104]`

## Protocol Details

- **Baud Rate**: 300
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1

## Implementation Notes

- The heater applies the state from each valid frame.
- There is no feedback or response from the heater.

## Troubleshooting

- Ensure the TX pin is connected properly.
- Verify the frame structure matches the protocol.
- Check the UART configuration (baud rate, data bits, etc.).
