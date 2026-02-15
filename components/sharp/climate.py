import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.components import ir_remote_base
from esphome.const import CONF_MODEL

AUTO_LOAD = ["climate_ir", "ir_remote_base"]

sharp_ns = cg.esphome_ns.namespace("sharp")
SharpClimate = sharp_ns.class_("SharpClimate", climate_ir.ClimateIR)

Model = sharp_ns.enum("Model")
MODELS = {
    "A907": Model.A907,
    "A705": Model.A705,
    "A903": Model.A903,
}

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(SharpClimate).extend(
    {
        cv.Required(CONF_MODEL): cv.enum(MODELS),
    }
)

async def to_code(config):
    ir_remote_base.load_ir_remote()

    var = await climate_ir.new_climate_ir(config)
    cg.add(var.set_model(config[CONF_MODEL]))
