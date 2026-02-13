#include "spectrolink_zone.h"

namespace esphome {
    namespace spectrolink {
        void SpectrolinkZone::setup() {
            // No special setup required for zone switch
        }
        void SpectrolinkZone::write_state(bool state) {
            if (parent_) {
                parent_->zones_[zone_index_] = state;
                parent_->send_frame();
            }
            publish_state(state);
        }
        void SpectrolinkZone::dump_config() {
            ESP_LOGCONFIG("spectrolink.zone", "Zone %d", zone_index_);
        }
    }  // namespace spectrolink
}  // namespace esphome
