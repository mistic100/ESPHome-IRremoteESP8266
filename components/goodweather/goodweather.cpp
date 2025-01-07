#include "goodweather.h"

namespace esphome
{
    namespace goodweather
    {
        static const char *const TAG = "goodweather.climate";

        void GoodweatherClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void GoodweatherClimate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void GoodweatherClimate::send()
        {
            uint64_t message = this->ac_.getRaw();

            auto transmit = this->transmitter_->transmit();
            auto *data = transmit.get_data();

            data->set_carrier_frequency(38000);

            // Header
            data->mark(kGoodweatherHdrMark);
            data->space(kGoodweatherHdrSpace);

            // Data
            for (int16_t i = 0; i < kGoodweatherBits; i += 8)
            {
                uint16_t chunk = (message >> i) & 0xFF; // Grab a byte at a time.
                chunk = (~chunk) << 8 | chunk; // Prepend a inverted copy of the byte.
                sendData(data,
                         kGoodweatherBitMark, kGoodweatherOneSpace,
                         kGoodweatherOneSpace, kGoodweatherZeroSpace,
                         chunk, 16);
            }

            // Footer
            data->mark(kGoodweatherBitMark);
            data->space(kGoodweatherHdrSpace);
            data->mark(kGoodweatherBitMark);
            data->space(kDefaultMessageGap);

            transmit.perform();
        }

        void GoodweatherClimate::apply_state()
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
                    this->ac_.setMode(kGoodweatherAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kGoodweatherHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kGoodweatherCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kGoodweatherDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kGoodweatherFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kGoodweatherFanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kGoodweatherFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kGoodweatherFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kGoodweatherFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwing(kGoodweatherSwingOff);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwing(kGoodweatherSwingSlow);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }

    } // namespace goodweather
} // namespace esphome
