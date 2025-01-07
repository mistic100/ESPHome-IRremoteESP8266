import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

AUTO_LOAD = ["climate_ir"]

goodweather_ns = cg.esphome_ns.namespace("goodweather")
GoodweatherClimate = goodweather_ns.class_(
    "GoodweatherClimate", climate_ir.ClimateIR
)

CONFIG_SCHEMA = climate_ir.CLIMATE_IR_WITH_RECEIVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(GoodweatherClimate),
    }
)


async def to_code(config):
    cg.add_library("IRremoteESP8266", None)

    var = cg.new_Pvariable(config[CONF_ID])
    await climate_ir.register_climate_ir(var, config)
