#define _IR_ENABLE_DEFAULT_ false
#define SEND_GOODWEATHER true
#define DECODE_GOODWEATHER false

#include "esphome.h"
#include "ir_Goodweather.h"
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

        climate::ClimateTraits GoodweatherClimate::traits()
        {
            return climate_ir::ClimateIR::traits();
        }

        void GoodweatherClimate::transmit_state()
        {
            this->apply_state();

            uint64_t message = this->ac_.getRaw();
            uint8_t length = kGoodweatherBits;

            auto transmit = this->transmitter_->transmit();
            auto *data = transmit.get_data();

            data->set_carrier_frequency(38000);

            // Header
            data->mark(kGoodweatherHdrMark);
            data->space(kGoodweatherHdrSpace);

            // Data
            for (int16_t i = 0; i < length; i += 8)
            {
                uint16_t d = (message >> i) & 0xFF;
                d = (~d) << 8 | d;
                for (uint16_t bit = 0; bit < 16; bit++, d >>= 1)
                {
                    if (d & 1)
                    {
                        data->mark(kGoodweatherBitMark);
                        data->space(kGoodweatherOneSpace);
                    }
                    else
                    {
                        data->mark(kGoodweatherBitMark);
                        data->space(kGoodweatherZeroSpace);
                    }
                }
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
                    this->ac_.setMode(kGoodweatherHeat);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kGoodweatherHeat);
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

            // FIXME does not compile on some frameworks ?
            // ESP_LOGI(TAG, this->ac_.toString().c_str());
        }

    } // namespace goodweather_general
} // namespace esphome
