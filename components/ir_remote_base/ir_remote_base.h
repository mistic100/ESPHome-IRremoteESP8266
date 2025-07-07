#pragma once

#include "esphome/components/remote_transmitter/remote_transmitter.h"

namespace esphome
{
    namespace ir_remote_base
    {
        class IrRemoteBase
        {
        protected:
            void sendGeneric(
                remote_base::RemoteTransmitterBase *transmitter_,
                const uint16_t headermark, const uint32_t headerspace,
                const uint16_t onemark, const uint32_t onespace,
                const uint16_t zeromark, const uint32_t zerospace,
                const uint16_t footermark, const uint32_t gap,
                const uint8_t *message, const uint16_t length,
                const uint16_t frequency)
            {
                auto transmit = transmitter_->transmit();
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
                for (uint8_t i = 0; i < length; i++)
                {
                    uint8_t d = *(message + i);
                    for (uint8_t bit = 0; bit < 8; bit++, d >>= 1)
                    {
                        if (d & 1)
                        {
                            data->mark(onemark);
                            data->space(onespace);
                        }
                        else
                        {
                            data->mark(zeromark);
                            data->space(zerospace);
                        }
                    }
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
        };
    }
}
