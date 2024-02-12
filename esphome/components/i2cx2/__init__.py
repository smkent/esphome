import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_CHANNEL, CONF_CHANNELS, CONF_ID, CONF_SCAN

CODEOWNERS = ["@andreashergert1984"]

DEPENDENCIES = ["i2c"]

i2cx2_ns = cg.esphome_ns.namespace("i2cx2")
I2Cx2Component = i2cx2_ns.class_("I2Cx2Component", cg.Component, i2c.I2CDevice)
I2Cx2Channel = i2cx2_ns.class_("I2Cx2Channel", i2c.I2CBus)

MULTI_CONF = True

CONF_BUS_ID = "bus_id"
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(I2Cx2Component),
            cv.Optional(CONF_SCAN): cv.invalid("This option has been removed"),
            cv.Optional(CONF_CHANNELS, default=[]): cv.ensure_list(
                {
                    cv.Required(CONF_BUS_ID): cv.declare_id(I2Cx2Channel),
                    cv.Required(CONF_CHANNEL): cv.int_range(min=0, max=7),
                }
            ),
        }
    )
    .extend(i2c.i2c_device_schema(0x70))
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    for conf in config[CONF_CHANNELS]:
        chan = cg.new_Pvariable(conf[CONF_BUS_ID])
        cg.add(chan.set_parent(var))
        cg.add(chan.set_channel(conf[CONF_CHANNEL]))
