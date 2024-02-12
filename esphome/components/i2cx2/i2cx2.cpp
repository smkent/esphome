#include "i2cx2.h"
#include "esphome/core/log.h"

namespace esphome {
namespace i2cx2 {

static const char *const TAG = "i2cx2";

i2c::ErrorCode I2Cx2Channel::readv(uint8_t address, i2c::ReadBuffer *buffers, size_t cnt) {
  auto err = this->parent_->switch_to_channel(channel_);
  if (err != i2c::ERROR_OK)
    return err;
  err = this->parent_->bus_->readv(address, buffers, cnt);
  this->parent_->disable_all_channels();
  return err;
}
i2c::ErrorCode I2Cx2Channel::writev(uint8_t address, i2c::WriteBuffer *buffers, size_t cnt, bool stop) {
  auto err = this->parent_->switch_to_channel(channel_);
  if (err != i2c::ERROR_OK)
    return err;
  err = this->parent_->bus_->writev(address, buffers, cnt, stop);
  this->parent_->disable_all_channels();
  return err;
}

void I2Cx2Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2Cx2...");
  uint8_t status = 0;
  if (this->read(&status, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "I2Cx2 failed");
    this->mark_failed();
    return;
  }
  ESP_LOGD(TAG, "Channels currently open: %d", status);
}
void I2Cx2Component::dump_config() {
  ESP_LOGCONFIG(TAG, "I2Cx2:");
  LOG_I2C_DEVICE(this);
}

i2c::ErrorCode I2Cx2Component::switch_to_channel(uint8_t channel) {
  if (this->is_failed())
    return i2c::ERROR_NOT_INITIALIZED;

  uint8_t channel_val = 1 << channel;
  return this->write(&channel_val, 1);
}

void I2Cx2Component::disable_all_channels() {
  if (this->write(&I2Cx2_DISABLE_CHANNELS_COMMAND, 1) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed to disable all channels.");
    this->status_set_error();  // couldn't disable channels, set error status
  }
}

}  // namespace i2cx2
}  // namespace esphome
