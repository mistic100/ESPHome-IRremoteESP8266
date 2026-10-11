import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.components import ir_remote_base
from esphome.const import CONF_MODEL, CONF_DEBOUNCE

AUTO_LOAD = ["climate_ir", "ir_remote_base"]

haier_ns = cg.esphome_ns.namespace("haier")
HaierClimate = haier_ns.class_("HaierClimate", climate_ir.ClimateIR)

Model = haier_ns.enum("Model")
MODELS = {
    "HAIER_AC": Model.HAIER_AC,
    "HAIER_AC160": Model.HAIER_AC160,
    "HAIER_AC176": Model.HAIER_AC176,
    "HAIER_AC_YRW02": Model.HAIER_AC_YRW02,
}

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(HaierClimate).extend(
    {
        cv.Required(CONF_MODEL): cv.enum(MODELS),
        cv.Optional(CONF_DEBOUNCE): cv.positive_time_period_milliseconds
    }
)


async def to_code(config):
    ir_remote_base.load_ir_remote()

    var = await climate_ir.new_climate_ir(config)
    cg.add(var.set_model(config[CONF_MODEL]))
    if CONF_DEBOUNCE in config:
        cg.add(var.set_debounce(config[CONF_DEBOUNCE]))
