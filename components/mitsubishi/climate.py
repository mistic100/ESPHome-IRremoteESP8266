import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.components import ir_remote_base
from esphome.const import CONF_MODEL

AUTO_LOAD = ["climate_ir", "ir_remote_base"]

mitsubishi_ns = cg.esphome_ns.namespace("mitsubishi")
MitsubishiClimate = mitsubishi_ns.class_("MitsubishiClimate", climate_ir.ClimateIR)

Model = mitsubishi_ns.enum("Model")
MODELS = {
    "MITSUBISHI_AC": Model.MITSUBISHI_AC,
    "MITSUBISHI136": Model.MITSUBISHI136,
    "MITSUBISHI112": Model.MITSUBISHI112,
}

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(MitsubishiClimate).extend(
    {
        cv.Required(CONF_MODEL): cv.enum(MODELS),
    }
)

async def to_code(config):
    ir_remote_base.load_ir_remote()

    var = await climate_ir.new_climate_ir(config)
    cg.add(var.set_model(config[CONF_MODEL]))
