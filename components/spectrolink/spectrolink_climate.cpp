#include "spectrolink_climate.h"
#include "esphome/core/log.h"

namespace esphome {
namespace spectrolink {

static const char *const TAG = "spectrolink.climate";

static const uint8_t ZONE_ALL  = 167;  // zone123
static const uint8_t ZONE_23  = 166;
static const uint8_t ZONE_13  = 165;
static const uint8_t ZONE_3   = 164;
static const uint8_t ZONE_12  = 163;
static const uint8_t ZONE_2   = 162;
static const uint8_t ZONE_1   = 161;
static const uint8_t ZONE_OFF = 160;

static const uint8_t STATE_OFF  = 74;
static const uint8_t STATE_COOL = 202;
static const uint8_t STATE_HEAT = 138;

void SpectrolinkClimate::setup() {
  send_frame();  // Initial reset frame
  last_send_ = millis();
}

void SpectrolinkClimate::loop() {
  if (millis() - last_send_ >= FRAME_PERIOD_MS) {
    send_frame();
    last_send_ = millis();
  }
}

void SpectrolinkClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    switch (call.get_mode().value()) {
      case climate::CLIMATE_MODE_HEAT:
        system_state_ = STATE_HEAT;
        mode = climate::CLIMATE_MODE_HEAT;
        break;
      case climate::CLIMATE_MODE_COOL:
        system_state_ = STATE_COOL;
        mode = climate::CLIMATE_MODE_COOL;
        break;
      default:
        system_state_ = STATE_OFF;
        mode = climate::CLIMATE_MODE_OFF;
        break;
    }
  }
  send_frame();
}

uint8_t SpectrolinkClimate::zone_byte() const {
  bool z1 = zones_[0], z2 = zones_[1], z3 = zones_[2];
  if (z1 && z2 && z3) return ZONE_ALL;
  if (z1 && z2) return ZONE_12;
  if (z1 && z3) return ZONE_13;
  if (z2 && z3) return ZONE_23;
  if (z1) return ZONE_1;
  if (z2) return ZONE_2;
  if (z3) return ZONE_3;
  return ZONE_OFF;
}

uint8_t SpectrolinkClimate::checksum(const uint8_t* data) const {
  // Ported directly from Spectrolink.ino switch statements [file:18]
  uint8_t zones = data[2];
  if (system_state_ == STATE_OFF) {
    switch (zones) {
      case ZONE_ALL: return 104;
      case ZONE_23: return 29;
      case ZONE_13: return 71;
      case ZONE_3: return 50;
      case ZONE_12: return 54;
      case ZONE_2: return 67;
      case ZONE_1: return 25;
      default: return 108;
    }
  } else if (system_state_ == STATE_HEAT) {
    switch (zones) {
      case ZONE_ALL: return 144;
      case ZONE_23: return 7;
      case ZONE_13: return 93;
      case ZONE_3: return 40;
      case ZONE_12: return 44;
      case ZONE_2: return 89;
      case ZONE_1: return 3;
      default: return 24;
    }
  } else {  // COOL
    switch (zones) {
      case ZONE_ALL: return 58;
      case ZONE_23: return 79;
      case ZONE_13: return 21;
      case ZONE_3: return 96;
      case ZONE_12: return 100;
      case ZONE_2: return 17;
      case ZONE_1: return 75;
      default: return 62;
    }
  }
}

void SpectrolinkClimate::send_frame() {
  uint8_t zones = zone_byte();
  uint8_t data[8] = {
    FRAME_HEADER,  // 202
    0,
    zones,
    (uint8_t)(system_state_ == STATE_OFF ? 0 : 8),
    (uint8_t)(system_state_ == STATE_HEAT ? 64 : 128),
    system_state_,
    1,
    0  // Checksum
  };
  data[7] = checksum(data);
  
  write_array(data, 8);
  
  ESP_LOGD(TAG, "TX zones=0x%02x mode=%d checksum=%d", zones, system_state_, data[7]);
}

void SpectrolinkClimate::dump_config() {
  ESP_LOGCONFIG(TAG, "Spectrolink Braemar Heater");
  ESP_LOGCONFIG(TAG, "  TX-only 300 baud, 1s frames");
}

climate::ClimateTraits SpectrolinkClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
  traits.add_supported_mode(climate::CLIMATE_MODE_COOL);
  traits.add_supported_mode(climate::CLIMATE_MODE_OFF);
  return traits;
}

}  // namespace spectrolink
}  // namespace esphome
