"""ESPHome Spectrolink Braemar heater climate component.

This component implements a custom climate integration for Braemar Spectrolink
heating systems via 300 baud UART communication.
"""

import esphome.codegen as cg

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@petruknw"]
AUTO_LOAD = ["climate"]

spectrolink_ns = cg.esphome_ns.namespace("spectrolink")
SpectrolinkClimate = spectrolink_ns.class_(
    "SpectrolinkClimate",
    cg.Component
)
