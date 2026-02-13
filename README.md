# ESPHome Spectrolink Braemar Heater

Replace your Braemar Spectrolink controller with an ESP8266 D1 Mini for full Home Assistant integration.

## Features

- Native HA climate card with heat/cool/off modes
- 3 independent zone switches
- Automatic 300 baud UART transmission every second
- OTA updates and WiFi management
- Works with modified fonske MHI-AC-Ctrl PCB

## Prerequisites

- [ESPHome](https://esphome.io/guides/installing_esphome.html) installed (2026.1.0+)
- Home Assistant instance (optional but recommended)
- USB cable for initial flash
- Modified fonske MHI-AC-Ctrl PCB (see Hardware section)

## Hardware

**Required components**:
- Wemos D1 Mini (ESP8266)
- [fonske MHI-AC-Ctrl PCB](https://github.com/ginkage/MHI-AC-Ctrl-ESPHome/blob/master/JLCPCB/Hardware.md) (order from JLCPCB). Don't purchase the TSR_1-2450E.
- HLK-PM01 5V AC-DC module

**PCB Build**:
1. Build the PCB as per fonske MHI-AC-Ctrl PCB](https://github.com/ginkage/MHI-AC-Ctrl-ESPHome/blob/master/JLCPCB/Hardware.md), except U1
2. Solder HLK-PM01 module:
   - AC input → Spectrolink RJ22 pins 1/2 (24VAC)
   - 5V output (Pin 4) → TSR Pad 1
   - GND (Pin 3) → TSR Pad 2
3. Wire Spectrolink connector:
   - RJ22 Pin 3 (TX) → CNS Pin 4 (via BSS138 Q1 level shifter)
   - RJ22 Pin 4 (GND) → CNS Pin 5
4. C1 (100nF capacitor) remains in place across Pads 1-2

## Configuration

```yaml
esphome:
  name: spectrolink-heater
  friendly_name: Spectrolink Heater

external_components:
  - source: github://petruknw/esphome-spectrolink-braemar
    components: [spectrolink]

esp8266:
  board: d1_mini

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  
  # Enable fallback hotspot (captive portal) in case wifi fails
  ap:
    ssid: "Spectrolink Fallback"
    password: "spectrolink123"

captive_portal:

api:
  encryption:
    key: !secret api_encryption_key

ota:
  - platform: esphome
    password: !secret ota_password

logger:
  level: DEBUG

uart:
  tx_pin: GPIO5  # D1 on D1 Mini
  baud_rate: 300
  id: uart_bus

climate:
  - platform: spectrolink
    id: heater
    name: "Braemar Heater"

switch:
  - platform: spectrolink
    spectrolink_id: heater
    zone: 0
    name: "Zone 1"
  - platform: spectrolink
    spectrolink_id: heater
    zone: 1
    name: "Zone 2"
  - platform: spectrolink
    spectrolink_id: heater
    zone: 2
    name: "Zone 3"
```

## Configuration Options

### GPIO Pin Mapping

- `tx_pin: GPIO5` - D1 on D1 Mini → BSS138 TX shifter
- Other GPIOs available: D2 (GPIO4), D5 (GPIO14), D6 (GPIO12), D7 (GPIO13)

### Zone Configuration

Skip zones if not needed:

```yaml
climate:
  - platform: spectrolink
    name: "Braemar Heater"
```

### Debug Logging

Monitor UART frames:

```yaml
logger:
  level: DEBUG
  logs:
    spectrolink.climate: DEBUG
```

## Protocol Details

**TX-only 300 baud UART**. ESP sends 8-byte frames every second:

```
[ZONES][CMD][MODE][STATE][CHECKSUM][1]
```

**Example frames**:
- Heat all zones: `[202, 0, 167, 8, 64, 138, 1, 144]`
- Off: `[202, 0, 167, 0, 0, 74, 0, 104]`

Heater applies state from each valid frame. No feedback/response from heater.

**Full protocol documentation**: [spectrolink-protocol.md](spectrolink-protocol.md)

## Troubleshooting

### Hardware Issues

**Logs show TX frames but heater doesn't respond**:
- Verify 24VAC present at HLK-PM01 input
- Check 5V output on TSR Pad 3 (should be ~5.0V)
- Confirm TX wire connected to BSS138 shifter (CNS Pin 4)
- Test with multimeter: TX pin should show ~3.3V idle

**D1 Mini won't power on**:
- Check HLK-PM01 soldering (AC input and 5V output)
- Verify C1 capacitor not shorted
- Measure 24VAC at heater transformer

**Zones don't work**:
- Add `id: heater` to climate config
- Verify `spectrolink_id: heater` in zone switches
- Check logs for zone change events

## Credits

- **Protocol reverse engineering**: [diablosv21/spectrolink2nest](https://github.com/diablosv21/spectrolink2nest)
- **Hardware base**: [fonske MHI-AC-Ctrl PCB](https://github.com/ginkage/MHI-AC-Ctrl-ESPHome)
- **ESPHome framework**: [esphome.io](https://esphome.io)

## License

MIT License - see [LICENSE](LICENSE)
