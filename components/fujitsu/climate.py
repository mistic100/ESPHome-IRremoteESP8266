import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_MODEL

AUTO_LOAD = ["climate_ir"]

fujitsu_ns = cg.esphome_ns.namespace("fujitsu")
FujitsuClimate = fujitsu_ns.class_("FujitsuClimate", climate_ir.ClimateIR)

Model = fujitsu_ns.enum("Model")
MODELS = {
    "ARRAH2E": Model.ARRAH2E,
    "ARDB1": Model.ARDB1,
    "ARREB1E": Model.ARREB1E,
    "ARJW2": Model.ARJW2,
    "ARRY4": Model.ARRY4,
    "ARREW4E": Model.ARREW4E,
}

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(FujitsuClimate).extend(
    {
        cv.Required(CONF_MODEL): cv.enum(MODELS),
    }
)


async def to_code(config):
    cg.add_library("IRremoteESP8266", None)

    var = await climate_ir.new_climate_ir(config)
    cg.add(var.set_model(config[CONF_MODEL]))
