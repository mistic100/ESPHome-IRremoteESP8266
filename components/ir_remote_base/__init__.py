import esphome.codegen as cg

def load_ir_remote():
    cg.add_library(name="IRremoteESP8266", version=None)
    cg.add_define("_IR_ENABLE_DEFAULT_", False)
