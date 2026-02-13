"""ESPHome Spectrolink Braemar heater climate component.

This component implements a custom climate integration for Braemar Spectrolink
heating systems via 300 baud UART communication.
"""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@petruknw"]
AUTO_LOAD = ["climate"]

spectrolink_ns = cg.esphome_ns.namespace("spectrolink")
SpectrolinkClimate = spectrolink_ns.class_(
    "SpectrolinkClimate",
    climate.Climate,      # ADD THIS - climate entity interface
    cg.Component,         # Already have this
    uart.UARTDevice       # ADD THIS - UART communication
)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(SpectrolinkClimate),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    """Generate C++ code for the component."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    await uart.register_uart_device(var, config)
