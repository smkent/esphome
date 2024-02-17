#pragma once

#include "esphome/core/component.h"
#include "esphome/core/gpio.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace i2cx2 {

class I2Cx2Component;
class I2Cx2VirtualBus : public Component, public i2c::I2CBus {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BUS - 1; }
  void update();

  void set_parent(I2Cx2Component *parent) { parent_ = parent; }
  void set_virtual_bus_num(bool virtual_bus_num) { virtual_bus_num_ = virtual_bus_num; }
  void set_scan(bool scan) { scan_ = scan; }

  i2c::ErrorCode readv(uint8_t address, i2c::ReadBuffer *buffers, size_t cnt) override;
  i2c::ErrorCode writev(uint8_t address, i2c::WriteBuffer *buffers, size_t cnt, bool stop) override;

 protected:
  uint8_t virtual_bus_num_{0};
  bool scan_{true};
  I2Cx2Component *parent_;
};

class I2Cx2Component : public Component, public i2c::I2CDevice {
 public:
  void set_pin(GPIOPin *pin) { pin_ = pin; }
  void set_bus0_pin_state(bool bus0_pin_state) { bus0_pin_state_ = bus0_pin_state; }
  void set_bus0_restore(bool bus0_restore) { bus0_restore_ = bus0_restore; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BUS; }
  void update();

  void switch_to_virtual_bus(uint8_t virtual_bus_num = 0);

 protected:
  bool bus0_pin_state_;
  bool bus0_restore_;
  uint8_t active_virtual_bus_num_{0xFF};
  GPIOPin *pin_;
  friend class I2Cx2VirtualBus;
};
}  // namespace i2cx2
}  // namespace esphome
