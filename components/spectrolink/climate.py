"""Spectrolink climate platform configuration."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, climate
from esphome.const import CONF_ID
from . import spectrolink_ns, SpectrolinkClimate


CONFIG_SCHEMA = climate.climate_schema(SpectrolinkClimate).extend(
    {
        cv.GenerateID(): cv.declare_id(SpectrolinkClimate),
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    """Generate C++ code for Spectrolink climate component."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    await uart.register_uart_device(var, config)
