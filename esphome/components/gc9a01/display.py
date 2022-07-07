import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import spi
from esphome.components import display
from esphome.const import (
    CONF_DC_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_RESET_PIN,
    CONF_PAGES,
)

CODEOWNERS = ["@4cello"]

DEPENDENCIES = ["spi"]

CONF_DEVICE_WIDTH = "device_width"
CONF_DEVICE_HEIGHT = "device_height"
CONF_OFFSET_X = "offset_x"
CONF_OFFSET_Y = "offset_y"
CONF_EIGHT_BIT_COLOR = "eight_bit_color"
CONF_INVERT_COLORS = "invert_colors"

gc9a01_ns = cg.esphome_ns.namespace("gc9a01")
SPIGC9A01 = gc9a01_ns.class_(
    "GC9A01", cg.PollingComponent, display.DisplayBuffer, spi.SPIDevice
)

GC9A01_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend(
    {
        cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    }
).extend(cv.polling_component_schema("5s"))

CONFIG_SCHEMA = cv.All(
    GC9A01_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SPIGC9A01),
            cv.Required(CONF_DC_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_DEVICE_WIDTH): cv.int_,
            cv.Required(CONF_DEVICE_HEIGHT): cv.int_,
            cv.Optional(CONF_OFFSET_Y, default=0): cv.int_,
            cv.Optional(CONF_OFFSET_X, default=0): cv.int_,
            cv.Optional(CONF_EIGHT_BIT_COLOR, default=True): cv.boolean,
            cv.Optional(CONF_INVERT_COLORS, default=False): cv.boolean,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(spi.spi_device_schema()),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)


async def setup_gc9a01(var, config):
    await cg.register_component(var, config)
    await display.register_display(var, config)

    if CONF_RESET_PIN in config:
        reset = await cg.gpio_pin_expression(config[CONF_RESET_PIN])
        cg.add(var.set_reset_pin(reset))
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_DEVICE_WIDTH],
        config[CONF_DEVICE_HEIGHT],
        config[CONF_OFFSET_Y],
        config[CONF_OFFSET_X],
        config[CONF_EIGHT_BIT_COLOR],
        config[CONF_INVERT_COLORS],
    )
    await setup_gc9a01(var, config)
    await spi.register_spi_device(var, config)

    dc = await cg.gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc))
