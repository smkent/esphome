# Custom components for ESPHome

Usage in ESPHome:

```yaml
external_components:
- source: github://smkent/esphome@main
  components: [ component_1_name, component_2_name, ... ]
```

## GC9A01

Support for GC9A01 chipset round LCD modules

Collected from PRs referenced within [ESPHome issue #1699][esphome-issue-1699],
plus some of my own tweaks

Example usage:

```yaml
external_components:
- source: github://smkent/esphome@main
  components: [ gc9a01 ]

esphome:
  on_boot:
    priority: 600
    then:
    - light.turn_on: lcd_round_backlight

output:
- id: !extend lcd_round_backlight_output
  platform: ledc
  pin: 20

light:
- platform: monochromatic
  output: "lcd_round_backlight_output"
  id: "lcd_round_backlight"
  name: "Display backlight"
  default_transition_length: 400ms

spi:
  mosi_pin: 6
  clk_pin: 4

display:
- platform: gc9a01
  reset_pin: GPIO0
  cs_pin: 7
  dc_pin: 5
  rotation: 180
  eight_bit_color: false
  auto_clear_enabled: false
  lambda: |-
    it.line(0, 0, 100, 50);
```

## I2Cx2

Switch between two I2C data (SDA) lines using an additional GPIO pin and MOSFETs

Adapted from [this StackOverflow circuit][so-i2c-multiplex-curcuit]
with an added MOSFET as a NOT gate (I used 2N7000 MOSFETs):

![Schematic](esphome/components/i2cx2/schematic.png)

Example usage:

```yaml
external_components:
- source: github://smkent/esphome@main
  components: [ i2cx2 ]

i2c:
- id: i2c0
  sda: 8
  scl: 9
  scan: false

i2cx2:
  i2c_id: i2c0
  # Bus selector pin number
  pin: 20
  # true if the first I2C bus needs the selector pin to be HIGH, false otherwise
  first_bus_pin_state: true
  # Always reactivate the first virtual bus after using the second virtual bus
  always_restore_first_bus: true
  virtual_buses:
  - bus_id: i2c0_high
    scan: true
  - bus_id: i2c0_low
    scan: true
```


[esphome-issue-1699]: https://github.com/esphome/feature-requests/issues/1699
[so-i2c-multiplex-curcuit]: https://electronics.stackexchange.com/a/209031
