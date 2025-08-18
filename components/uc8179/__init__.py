import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

uc8179_ns = cg.esphome_ns.namespace("uc8179")
UC8179 = uc8179_ns.class_("UC8179", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(UC8719),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)
