#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace spectrolink {

static const uint8_t FRAME_HEADER = 202;
static const uint32_t FRAME_PERIOD_MS = 1000;

/**
 * @brief SpectrolinkClimate - Custom climate component for Braemar Spectrolink heaters.
 * 
 * Communicates with Braemar heating systems via 300 baud UART TX-only protocol.
 * Sends 8-byte control frames every second encoding mode, zones, and system state.
 * 
 * Frame format: [202][0][ZONES][CMD][MODE][STATE][1][CHECKSUM]
 * 
 * @extends climate::Climate Provides Home Assistant climate entity interface
 * @extends uart::UARTDevice Provides UART communication capabilities
 * @extends Component Provides ESPHome lifecycle management (setup, loop)
 */
class SpectrolinkClimate : public climate::Climate, public uart::UARTDevice, public Component {
public:
    /// Initialize component during ESPHome boot
    void setup() override;
    
    /// Called every loop cycle - sends UART frames every 1000ms
    void loop() override;
    
    /// Log component configuration to console
    void dump_config() override;
    
    /// Setup priority for component initialization order
    float get_setup_priority() const override { return setup_priority::DATA; }
    
    /// Handle climate control requests from Home Assistant
    void control(const climate::ClimateCall &call) override;
    
    /// Send 8-byte control frame to heater via UART
    void send_frame();
    
    /// Zone on/off states (3 zones: index 0-2)
    uint8_t zones_[3] = {false, false, false};
    
    /// Get climate traits (supported modes, temperature ranges)
    climate::ClimateTraits traits() override;
    
    /// Set UART bus instance
    void set_uart(uart::UARTComponent *uart_inst) { this->uart_bus_ = uart_inst; }

protected:
    /// Calculate zone byte from zones_[] array (bit-packed format)
    uint8_t zone_byte() const;
    
    /// Calculate checksum for 8-byte frame
    uint8_t checksum(const uint8_t* data) const;
    
    /// Current system state byte (74 = OFF, 138 = ON)
    uint8_t system_state_ = 74;  // OFF
    
    /// Timestamp of last frame transmission (milliseconds)
    uint32_t last_send_ = 0;
    
    /// UART bus instance for communication
    uart::UARTComponent *uart_bus_ = nullptr;
};

}  // namespace spectrolink
}  // namespace esphome
