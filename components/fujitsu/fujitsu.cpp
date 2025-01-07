#define _IR_ENABLE_DEFAULT_ false
#define SEND_FUJITSU_AC true
#define DECODE_FUJITSU_AC false

#include "esphome.h"
#include "ir_Fujitsu.h"
#include "fujitsu.h"

namespace esphome
{
    namespace fujitsu
    {
        // copied from ir_Fujitsu.cpp
        const uint16_t kFujitsuAcHdrMark = 3324;
        const uint16_t kFujitsuAcHdrSpace = 1574;
        const uint16_t kFujitsuAcBitMark = 448;
        const uint16_t kFujitsuAcOneSpace = 1182;
        const uint16_t kFujitsuAcZeroSpace = 390;
        const uint16_t kFujitsuAcMinGap = 8100;

        static const char *const TAG = "fujitsu.climate";

        void FujitsuClimate::set_model(const Model model)
        {
            this->ac_.setModel((fujitsu_ac_remote_model_t) model);
        }

        void FujitsuClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        climate::ClimateTraits FujitsuClimate::traits()
        {
            auto traits = climate_ir::ClimateIR::traits();
            if (this->ac_.getModel() == fujitsu_ac_remote_model_t::ARRAH2E || this->ac_.getModel() == fujitsu_ac_remote_model_t::ARJW2)
            {
                traits.add_supported_swing_mode(climate::CLIMATE_SWING_HORIZONTAL);
                traits.add_supported_swing_mode(climate::CLIMATE_SWING_BOTH);
            }
            return traits;
        }

        void FujitsuClimate::transmit_state()
        {
            this->apply_state();

            uint8_t *message = this->ac_.getRaw();
            uint8_t length = this->ac_.getStateLength();

            auto transmit = this->transmitter_->transmit();
            auto *data = transmit.get_data();

            data->set_carrier_frequency(38000);

            // Header
            data->mark(kFujitsuAcHdrMark);
            data->space(kFujitsuAcHdrSpace);

            // Data
            for (uint8_t i = 0; i < length; i++)
            {
                uint8_t d = *(message + i);
                for (uint8_t bit = 0; bit < 8; bit++, d >>= 1)
                {
                    if (d & 1)
                    {
                        data->mark(kFujitsuAcBitMark);
                        data->space(kFujitsuAcOneSpace);
                    }
                    else
                    {
                        data->mark(kFujitsuAcBitMark);
                        data->space(kFujitsuAcZeroSpace);
                    }
                }
            }

            // Footer
            data->mark(kFujitsuAcBitMark);
            data->space(kFujitsuAcMinGap);

            transmit.perform();
        }

        void FujitsuClimate::apply_state()
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
                    this->ac_.setMode(kFujitsuAcModeAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kFujitsuAcModeHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kFujitsuAcModeCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kFujitsuAcModeDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kFujitsuAcModeFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFanSpeed(kFujitsuAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_QUIET:
                        this->ac_.setFanSpeed(kFujitsuAcFanQuiet);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFanSpeed(kFujitsuAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFanSpeed(kFujitsuAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFanSpeed(kFujitsuAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwing(kFujitsuAcSwingOff);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwing(kFujitsuAcSwingVert);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwing(kFujitsuAcSwingHoriz);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwing(kFujitsuAcSwingBoth);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, this->ac_.toString().c_str());
        }

    } // namespace fujitsu_general
} // namespace esphome
