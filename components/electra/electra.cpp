#define _IR_ENABLE_DEFAULT_ false
#define SEND_ELECTRA_AC true
#define DECODE_ELECTRA_AC false

#include "esphome.h"
#include "ir_Electra.h"
#include "electra.h"

namespace esphome
{
    namespace electra
    {
        // copied from ir_Electra.cpp
        const uint16_t kElectraAcHdrMark = 9166;
        const uint16_t kElectraAcBitMark = 646;
        const uint16_t kElectraAcHdrSpace = 4470;
        const uint16_t kElectraAcOneSpace = 1647;
        const uint16_t kElectraAcZeroSpace = 547;
        const uint32_t kElectraAcMessageGap = kDefaultMessageGap;  // Just a guess.

        static const char *const TAG = "electra.climate";

        void ElectraClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void ElectraClimate::transmit_state()
        {
            this->apply_state();
            this->send();
        }

        void ElectraClimate::send()
        {
            uint8_t *message = this->ac_.getRaw();
            const uint16_t kElectraAcStateLength = 13;

            auto transmit = this->transmitter_->transmit();
            auto *data = transmit.get_data();

            data->set_carrier_frequency(38000);

            // Header
            data->mark(kElectraAcHdrMark);
            data->space(kElectraAcHdrSpace);

            // Data
            for (uint8_t i = 0; i < kElectraAcStateLength; i++)
            {
                uint8_t d = *(message + i);
                for (uint8_t bit = 0; bit < 8; bit++, d >>= 1)
                {
                    if (d & 1)
                    {
                        data->mark(kElectraAcBitMark);
                        data->space(kElectraAcOneSpace);
                    }
                    else
                    {
                        data->mark(kElectraAcBitMark);
                        data->space(kElectraAcZeroSpace);
                    }
                }
            }

            // Footer
            data->mark(kElectraAcBitMark);
            data->space(kElectraAcMessageGap);

            transmit.perform();
        }

        void ElectraClimate::apply_state()
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
                case climate::CLIMATE_MODE_AUTO:
                    this->ac_.setMode(kElectraAcFanAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kElectraAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kElectraAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kElectraAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kElectraAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kElectraAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kElectraAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kElectraAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kElectraAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwingH(false);
                    this->ac_.setSwingV(false);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwingH(false);
                    this->ac_.setSwingV(true);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwingH(true);
                    this->ac_.setSwingV(false);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwingH(true);
                    this->ac_.setSwingV(true);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "State: %s", LOG_STR_ARG(this->ac_.toString().c_str()));
        }
    } // namespace electra
} // namespace esphome
