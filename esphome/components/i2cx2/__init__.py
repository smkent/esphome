import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_PIN, CONF_SCAN

try:
    from esphome.const import (
        CONF_ALWAYS_RESTORE_FIRST_BUS,
        CONF_FIRST_BUS_PIN_STATE,
        CONF_VIRTUAL_BUSES,
    )
except ImportError:
    CONF_ALWAYS_RESTORE_FIRST_BUS = "always_restore_first_bus"
    CONF_FIRST_BUS_PIN_STATE = "first_bus_pin_state"
    CONF_VIRTUAL_BUSES = "virtual_buses"


DEPENDENCIES = ["i2c"]

i2cx2_ns = cg.esphome_ns.namespace("i2cx2")
I2Cx2Component = i2cx2_ns.class_("I2Cx2Component", cg.Component, i2c.I2CDevice)
I2Cx2VirtualBus = i2cx2_ns.class_("I2Cx2VirtualBus", cg.Component, i2c.I2CBus)

MULTI_CONF = True

CONF_BUS_ID = "bus_id"
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(I2Cx2Component),
            cv.Optional(CONF_SCAN): cv.invalid("This option has been removed"),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_FIRST_BUS_PIN_STATE, default=True): cv.boolean,
            cv.Optional(CONF_ALWAYS_RESTORE_FIRST_BUS, default=True): cv.boolean,
            cv.Optional(CONF_VIRTUAL_BUSES, default=[]): cv.ensure_list(
                {
                    cv.Required(CONF_BUS_ID): cv.declare_id(I2Cx2VirtualBus),
                    cv.Optional(CONF_SCAN, default=True): cv.boolean,
                }
            ),
        }
    )
    .extend(i2c.i2c_device_schema(0x00))
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    cg.add(var.set_pin(pin))
    cg.add(var.set_bus0_pin_state(config[CONF_FIRST_BUS_PIN_STATE]))
    cg.add(var.set_bus0_restore(config[CONF_ALWAYS_RESTORE_FIRST_BUS]))

    for i, virtual_bus in enumerate(config[CONF_VIRTUAL_BUSES]):
        if i > 1:
            raise cv.Invalid("I2Cx2 supports a maximum of two virtual buses")
        chan = cg.new_Pvariable(virtual_bus[CONF_BUS_ID])
        await cg.register_component(chan, virtual_bus)
        cg.add(chan.set_parent(var))
        cg.add(chan.set_virtual_bus_num(i))
        cg.add(chan.set_scan(virtual_bus[CONF_SCAN]))
