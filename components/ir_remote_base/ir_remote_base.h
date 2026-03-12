#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome
{
    namespace ir_remote_base
    {
        class IrRemoteBase : public climate_ir::ClimateIR
        {
        public:
            IrRemoteBase(
                float minimum_temperature, float maximum_temperature, float temperature_step,
                bool supports_dry, bool supports_fan_only,
                climate::ClimateFanModeMask fan_modes,
                climate::ClimateSwingModeMask swing_modes)
                : ClimateIR(minimum_temperature, maximum_temperature, temperature_step,
                            supports_dry, supports_fan_only,
                            fan_modes, swing_modes) {}

        protected:
            void sendGeneric(
                const uint16_t headermark, const uint32_t headerspace,
                const uint16_t onemark, const uint32_t onespace,
                const uint16_t zeromark, const uint32_t zerospace,
                const uint16_t footermark, const uint32_t gap,
                const uint8_t *message, const uint16_t nbytes,
                const uint16_t frequency)
            {
                auto transmit = this->transmitter_->transmit();
                auto *data = transmit.get_data();

                data->set_carrier_frequency(frequency);

                // Header
                if (headermark)
                    data->mark(headermark);
                else if (headerspace)
                    data->mark(1);
                if (headerspace)
                    data->space(headerspace);

                // Data
                for (uint16_t i = 0; i < nbytes; i++)
                {
                    sendData(data,
                             onemark, onespace,
                             zeromark, zerospace,
                             *(message + i), 8);
                }

                // Footer
                if (footermark)
                    data->mark(footermark);
                else if (gap)
                    data->mark(1);
                if (gap)
                    data->space(gap);

                transmit.perform();
            }

            void sendData(
                esphome::remote_base::RemoteTransmitData *transmit_data,
                uint16_t onemark, uint32_t onespace,
                uint16_t zeromark, uint32_t zerospace,
                uint64_t data, uint16_t nbits)
            {
                for (uint16_t bit = 0; bit < nbits; bit++, data >>= 1)
                {
                    if (data & 1)
                    {
                        transmit_data->mark(onemark);
                        transmit_data->space(onespace);
                    }
                    else
                    {
                        transmit_data->mark(zeromark);
                        transmit_data->space(zerospace);
                    }
                }
            }
        };
    }
}
