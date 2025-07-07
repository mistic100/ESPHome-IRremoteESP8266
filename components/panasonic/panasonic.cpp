#define _IR_ENABLE_DEFAULT_ false
#define SEND_PANASONIC_AC true
#define DECODE_PANASONIC_AC false

#include "esphome.h"
#include "ir_Panasonic.h"
#include "panasonic.h"

namespace esphome
{
    namespace panasonic
    {
        // copied from ir_Panasonic.cpp
        const uint16_t kPanasonicHdrMark = 3456;
        const uint16_t kPanasonicHdrSpace = 1728;
        const uint16_t kPanasonicBitMark = 432;
        const uint16_t kPanasonicOneSpace = 1296;
        const uint16_t kPanasonicZeroSpace = 432;
        const uint16_t kPanasonicAcSectionGap = 10000;
        const uint16_t kPanasonicAcSection1Length = 8;
        const uint32_t kPanasonicAcMessageGap = kDefaultMessageGap;

        static const char *const TAG = "panasonic.climate";

        void PanasonicClimate::set_model(const Model model)
        {
            this->ac_.setModel((panasonic_ac_remote_model_t) model);
        }

        void PanasonicClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        climate::ClimateTraits PanasonicClimate::traits()
        {
            auto traits = climate_ir::ClimateIR::traits();
            if (this->ac_.getModel() == panasonic_ac_remote_model_t::kPanasonicDke || this->ac_.getModel() == panasonic_ac_remote_model_t::kPanasonicRkr)
            {
                traits.add_supported_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);
                traits.add_supported_swing_mode(climate::CLIMATE_SWING_BOTH);
            }
            return traits;
        }

        void PanasonicClimate::transmit_state()
        {
            this->apply_state();

            uint8_t *message = this->ac_.getRaw();
            uint8_t length = kPanasonicAcStateLength;

            sendGeneric(
                this->transmitter_,
                kPanasonicHdrMark, kPanasonicHdrSpace,
                kPanasonicBitMark, kPanasonicOneSpace,
                kPanasonicBitMark, kPanasonicZeroSpace,
                kPanasonicBitMark, kPanasonicAcSectionGap,
                message, length,
                kPanasonicFreq
            );
            sendGeneric(
                this->transmitter_,
                kPanasonicHdrMark, kPanasonicHdrSpace,
                kPanasonicBitMark, kPanasonicOneSpace,
                kPanasonicBitMark, kPanasonicZeroSpace,
                kPanasonicBitMark, kPanasonicAcMessageGap,
                message + kPanasonicAcSection1Length,
                length - kPanasonicAcSection1Length,
                kPanasonicFreq
            );
        }

        void PanasonicClimate::apply_state()
        {
            if (this->mode == climate::CLIMATE_MODE_OFF)
            {
                this->ac_.off();
            }
            else
            {
                this->ac_.setTemp(this->target_temperature);

                switch (this->mode)
                {
                case climate::CLIMATE_MODE_HEAT_COOL:
                    this->ac_.setMode(kPanasonicAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kPanasonicAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kPanasonicAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kPanasonicAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kPanasonicAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kPanasonicAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setFan(kPanasonicAcFanMin);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kPanasonicAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kPanasonicAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kPanasonicAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVMiddle);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVAuto);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHMiddle);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVMiddle);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHAuto);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwingVertical(kPanasonicAcSwingVAuto);
                    this->ac_.setSwingHorizontal(kPanasonicAcSwingHAuto);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, this->ac_.toString().c_str());
        }

    } // namespace panasonic_general
} // namespace esphome
