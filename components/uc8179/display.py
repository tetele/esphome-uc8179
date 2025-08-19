import esphome.codegen as cg
from esphome.components import display
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MODEL

DEPENDENCIES = ["spi"]

CONF_DRIVER_ID = "driver_id"

uc8179_ns = cg.esphome_ns.namespace("uc8179")
UC8179 = uc8179_ns.class_("UC8179", cg.Component)
UC8179DisplayBase = uc8179_ns.class_("UC8179DisplayBase", display.DisplayBuffer)

MODELS = {}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(UC8179DisplayBase),
        cv.Required(CONF_MODEL): cv.one_of(*MODELS, lower=True),
        cv.GenerateID(key=CONF_DRIVER_ID): cv.use_id(UC8179),
    }
)


async def to_code(config):
    model = MODELS[config[CONF_MODEL]]
    var = cg.Pvariable(config[CONF_ID], model.new(), model)

    uc8179 = await cg.get_variable(config[CONF_DRIVER_ID])

    cg.add(var.set_driver(uc8179))
