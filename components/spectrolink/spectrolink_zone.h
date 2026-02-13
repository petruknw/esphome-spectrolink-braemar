#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "spectrolink_climate.h"

namespace esphome {
namespace spectrolink {

class SpectrolinkZone : public switch_::Switch, public Component {
 public:
  void setup() override;
  void write_state(bool state) override;
  void dump_config() override;
  
  void set_parent(SpectrolinkClimate *parent) { parent_ = parent; }
  void set_zone_index(uint8_t index) { zone_index_ = index; }
  
 protected:
  SpectrolinkClimate *parent_;
  uint8_t zone_index_;
};

}  // namespace spectrolink
}  // namespace esphome
