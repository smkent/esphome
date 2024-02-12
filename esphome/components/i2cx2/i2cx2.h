#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace i2cx2 {

static const uint8_t I2Cx2_DISABLE_CHANNELS_COMMAND = 0x00;

class I2Cx2Component;
class I2Cx2Channel : public i2c::I2CBus {
 public:
  void set_channel(uint8_t channel) { channel_ = channel; }
  void set_parent(I2Cx2Component *parent) { parent_ = parent; }

  i2c::ErrorCode readv(uint8_t address, i2c::ReadBuffer *buffers, size_t cnt) override;
  i2c::ErrorCode writev(uint8_t address, i2c::WriteBuffer *buffers, size_t cnt, bool stop) override;

 protected:
  uint8_t channel_;
  I2Cx2Component *parent_;
};

class I2Cx2Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::IO; }
  void update();

  i2c::ErrorCode switch_to_channel(uint8_t channel);
  void disable_all_channels();

 protected:
  friend class I2Cx2Channel;
};
}  // namespace i2cx2
}  // namespace esphome
