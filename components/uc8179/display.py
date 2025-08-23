import esphome.codegen as cg
from esphome.components import display
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_MODEL

DEPENDENCIES = ["spi"]

CONF_DRIVER_ID = "driver_id"

uc8179_ns = cg.esphome_ns.namespace("uc8179")
UC8179 = uc8179_ns.class_("UC8179", cg.Component)
UC8179DisplayBase = uc8179_ns.class_("UC8179DisplayBase", display.DisplayBuffer)

UC8179Display_KW = uc8179_ns.class_("UC8179Display_KW", UC8179DisplayBase)
UC8179Display_G4 = uc8179_ns.class_("UC8179Display_G4", UC8179Display_KW)

GDEY075T7 = uc8179_ns.class_("GDEY075T7", UC8179Display_G4)

MODELS = {
    "gdey075t7": GDEY075T7,
}

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend(
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

    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
