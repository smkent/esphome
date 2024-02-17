#include "i2cx2.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace i2cx2 {

static const char *const TAG = "i2cx2";

void I2Cx2VirtualBus::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2Cx2VirtualBus...");
  if (this->scan_) {
    ESP_LOGI(TAG, "Scanning virtual i2c bus for active devices...");
    this->i2c_scan_();
  } else {
    ESP_LOGI(TAG, "Skipping i2c scan");
  }
  ESP_LOGCONFIG(TAG, "Done setting up I2Cx2VirtualBus...");
}

void I2Cx2VirtualBus::dump_config() {
  ESP_LOGCONFIG(TAG, "I2Cx2VirtualBus:");
  if (this->scan_) {
    ESP_LOGI(TAG, "Results from i2c bus scan");
    if (scan_results_.empty()) {
      ESP_LOGI(TAG, "Found no i2c devices!");
    } else {
      for (const auto &s : scan_results_) {
        if (s.second) {
          ESP_LOGI(TAG, "Found i2c device at address 0x%02X", s.first);
        } else {
          ESP_LOGE(TAG, "Unknown error at address 0x%02X", s.first);
        }
      }
    }
  }
  this->parent_->switch_to_virtual_bus();
}

i2c::ErrorCode I2Cx2VirtualBus::readv(uint8_t address, i2c::ReadBuffer *buffers, size_t cnt) {
  this->parent_->switch_to_virtual_bus(this->virtual_bus_num_);
  auto err = this->parent_->bus_->readv(address, buffers, cnt);
  if (this->parent_->bus0_restore_) {
    this->parent_->switch_to_virtual_bus();
  }
  return err;
}
i2c::ErrorCode I2Cx2VirtualBus::writev(uint8_t address, i2c::WriteBuffer *buffers, size_t cnt, bool stop) {
  this->parent_->switch_to_virtual_bus(this->virtual_bus_num_);
  auto err = this->parent_->bus_->writev(address, buffers, cnt, stop);
  if (this->parent_->bus0_restore_) {
    this->parent_->switch_to_virtual_bus();
  }
  return err;
}

void I2Cx2Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2Cx2...");
  this->pin_->setup();
  this->switch_to_virtual_bus();
}
void I2Cx2Component::dump_config() {
  ESP_LOGCONFIG(TAG, "I2Cx2:");
  LOG_PIN("  Pin: ", this->pin_);
}

void I2Cx2Component::switch_to_virtual_bus(uint8_t virtual_bus_num) {
  if (virtual_bus_num == this->active_virtual_bus_num_) {
    return;
  }
  bool pin_state = virtual_bus_num == 0 ? this->bus0_pin_state_ : !this->bus0_pin_state_;
  this->pin_->digital_write(pin_state);
  this->active_virtual_bus_num_ = virtual_bus_num;
}

}  // namespace i2cx2
}  // namespace esphome
