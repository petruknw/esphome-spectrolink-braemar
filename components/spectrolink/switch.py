import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID
from . import spectrolink_ns, SpectrolinkClimate

SpectrolinkZone = spectrolink_ns.class_("SpectrolinkZone", switch.Switch, cg.Component)

CONF_SPECTROLINK_ID = "spectrolink_id"
CONF_ZONE = "zone"

CONFIG_SCHEMA = switch.switch_schema(SpectrolinkZone).extend({
    cv.Required(CONF_SPECTROLINK_ID): cv.use_id(SpectrolinkClimate),
    cv.Required(CONF_ZONE): cv.int_range(min=0, max=2),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await switch.register_switch(var, config)
    
    parent = await cg.get_variable(config[CONF_SPECTROLINK_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_zone_index(config[CONF_ZONE]))
