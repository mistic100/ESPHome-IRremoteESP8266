#define _IR_ENABLE_DEFAULT_ false
#define SEND_FUJITSU_AC264 true
#define DECODE_FUJITSU_AC264 false

#include "esphome.h"
#include "ir_Fujitsu.h"
#include "fujitsu_264.h"

namespace esphome
{
    namespace fujitsu_264
    {
        // copied from ir_Fujitsu.cpp
        const uint16_t kFujitsuAcHdrMark = 3324;
        const uint16_t kFujitsuAcHdrSpace = 1574;
        const uint16_t kFujitsuAcBitMark = 448;
        const uint16_t kFujitsuAcOneSpace = 1182;
        const uint16_t kFujitsuAcZeroSpace = 390;
        const uint16_t kFujitsuAcMinGap = 8100;

        static const char *const TAG = "fujitsu_264.climate";

        void Fujitsu264Climate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void Fujitsu264Climate::set_fan_angle(const uint8_t fan_angle)
        {
            this->ac_.setFanAngle(fan_angle);
            ESP_LOGI(TAG, "Set fan angle to %d", fan_angle);
            this->send();
        }

        void Fujitsu264Climate::toggle_powerful()
        {
            this->ac_.togglePowerful();
            ESP_LOGI(TAG, "Toggled powerful mode");
            this->send();
        }

        void Fujitsu264Climate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void Fujitsu264Climate::send()
        {
            uint8_t *message = this->ac_.getRaw();
            uint8_t length = this->ac_.getStateLength();

            sendGeneric(
                this->transmitter_,
                kFujitsuAcHdrMark, kFujitsuAcHdrSpace,
                kFujitsuAcBitMark, kFujitsuAcOneSpace,
                kFujitsuAcBitMark, kFujitsuAcZeroSpace,
                kFujitsuAcBitMark, kFujitsuAcMinGap,
                message, length,
                38000
            );
        }

        void Fujitsu264Climate::apply_state()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_.off();
            }
            else
            {
                this->ac_.setTemp(this->target_temperature);

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFanSpeed(kFujitsuAc264FanSpeedAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setFanSpeed(kFujitsuAc264FanSpeedQuiet);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFanSpeed(kFujitsuAc264FanSpeedLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFanSpeed(kFujitsuAc264FanSpeedMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFanSpeed(kFujitsuAc264FanSpeedHigh);
                        break;
                    default:
                        ESP_LOGW(TAG, "Unknown fan mode: %d", this->fan_mode.value());
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwing(false);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwing(true);
                    break;
                default:
                    this->ac_.setSwing(true);
                    break;
                }

                // this->ac_.on() is not needed as it is already handled by the following mode switch
                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_.setMode(kFujitsuAc264ModeAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kFujitsuAc264ModeHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kFujitsuAc264ModeCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kFujitsuAc264ModeDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kFujitsuAc264ModeFan);
                    break;
                default:
                    ESP_LOGW(TAG, "Unknown mode: %d", this->mode);
                    break;
                }
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

    }  // namespace fujitsu_264
}  // namespace esphome
