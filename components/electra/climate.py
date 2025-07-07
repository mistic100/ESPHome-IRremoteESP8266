import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir", "ir_remote_base"]

electra_ns = cg.esphome_ns.namespace("electra")
ElectraClimate = electra_ns.class_("ElectraClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(ElectraClimate)

async def to_code(config):
    cg.add_library("IRremoteESP8266", None)
    var = await climate_ir.new_climate_ir(config)
