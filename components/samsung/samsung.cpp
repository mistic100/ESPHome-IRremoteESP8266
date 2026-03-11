#include "samsung.h"

namespace esphome
{
    namespace samsung
    {
        // copie from ir_Samsung.cpp
        const uint16_t kSamsungAcHdrMark = 690;
        const uint16_t kSamsungAcHdrSpace = 17844;
        const uint8_t kSamsungAcSections = 2;
        const uint16_t kSamsungAcSectionMark = 3086;
        const uint16_t kSamsungAcSectionSpace = 8864;
        const uint16_t kSamsungAcSectionGap = 2886;
        const uint16_t kSamsungAcBitMark = 586;
        const uint16_t kSamsungAcOneSpace = 1432;
        const uint16_t kSamsungAcZeroSpace = 436;

        static const char *const TAG = "samsung.climate";

        void SamsungClimate::setup()
        {
            climate_ir::ClimateIR::setup();
            this->apply_state();
        }

        void SamsungClimate::transmit_state()
        {
            this->apply_state();

            if (this->ac_.getPower() != _lastsentpowerstate)
            {
                _lastsentpowerstate = this->ac_.getPower();
                send_extended();
            }
            else
            {
                send(kSamsungAcStateLength);
            }
        }

        void SamsungClimate::send(const uint16_t nbytes)
        {
            uint8_t *message = this->ac_.getRaw();

            auto transmit = this->transmitter_->transmit();
            auto *data = transmit.get_data();

            data->set_carrier_frequency(38000);

            // Header
            data->mark(kSamsungAcHdrMark);
            data->space(kSamsungAcHdrSpace);

            // Send in 7 byte sections.
            for (uint16_t offset = 0; offset < nbytes; offset += kSamsungAcSectionLength)
            {
                // Header
                data->mark(kSamsungAcSectionMark);
                data->space(kSamsungAcSectionSpace);

                // Data
                for (uint16_t i = 0; i < kSamsungAcSectionLength; i++)
                {
                    sendData(data,
                             kSamsungAcBitMark, kSamsungAcOneSpace,
                             kSamsungAcBitMark, kSamsungAcZeroSpace,
                             *(message + offset + 1), 8);
                }

                // Footer
                data->mark(kSamsungAcBitMark);
                data->space(kSamsungAcSectionGap);
            }

            // Complete made up guess at inter-message gap.
            data->space(kDefaultMessageGap - kSamsungAcSectionGap);
        }

        void SamsungClimate::send_extended()
        {
            uint8_t *raw = this->ac_.getRaw();

            static const uint8_t extended_middle_section[kSamsungAcSectionLength] = {0x01, 0xD2, 0x0F, 0x00, 0x00, 0x00, 0x00};
            // Copy/convert the internal state to an extended state by copying the second section
            // to the third section, and inserting the extended middle (second) section.
            std::memcpy(raw + 2 * kSamsungAcSectionLength, raw + kSamsungAcSectionLength, kSamsungAcSectionLength);
            std::memcpy(raw + kSamsungAcSectionLength, extended_middle_section, kSamsungAcSectionLength);
            // Send it.
            send(kSamsungAcExtendedStateLength);
            // Now revert it by copying the third section over the second section.
            std::memcpy(raw + kSamsungAcSectionLength, raw + 2 * kSamsungAcSectionLength, kSamsungAcSectionLength);
        }

        void SamsungClimate::apply_state()
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
                    this->ac_.setMode(kSamsungAcAuto);
                    break;
                case climate::CLIMATE_MODE_HEAT:
                    this->ac_.setMode(kSamsungAcHeat);
                    break;
                case climate::CLIMATE_MODE_COOL:
                    this->ac_.setMode(kSamsungAcCool);
                    break;
                case climate::CLIMATE_MODE_DRY:
                    this->ac_.setMode(kSamsungAcDry);
                    break;
                case climate::CLIMATE_MODE_FAN_ONLY:
                    this->ac_.setMode(kSamsungAcFan);
                    break;
                }

                if (this->fan_mode.has_value())
                {
                    switch (this->fan_mode.value())
                    {
                    case climate::CLIMATE_FAN_AUTO:
                        this->ac_.setFan(kSamsungAcFanAuto);
                        break;
                    case climate::CLIMATE_FAN_LOW:
                        this->ac_.setFan(kSamsungAcFanLow);
                        break;
                    case climate::CLIMATE_FAN_MEDIUM:
                        this->ac_.setFan(kSamsungAcFanMed);
                        break;
                    case climate::CLIMATE_FAN_HIGH:
                        this->ac_.setFan(kSamsungAcFanHigh);
                        break;
                    }
                }

                switch (this->swing_mode)
                {
                case climate::CLIMATE_SWING_OFF:
                    this->ac_.setSwing(false);
                    this->ac_.setSwingH(false);
                    break;
                case climate::CLIMATE_SWING_VERTICAL:
                    this->ac_.setSwing(true);
                    this->ac_.setSwingH(false);
                    break;
                case climate::CLIMATE_SWING_HORIZONTAL:
                    this->ac_.setSwing(false);
                    this->ac_.setSwingH(true);
                    break;
                case climate::CLIMATE_SWING_BOTH:
                    this->ac_.setSwing(true);
                    this->ac_.setSwingH(true);
                    break;
                }

                this->ac_.on();
            }

            ESP_LOGI(TAG, "%s", this->ac_.toString().c_str());
        }
    } // namespace samsung
} // namespace esphome
