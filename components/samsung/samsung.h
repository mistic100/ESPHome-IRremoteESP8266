#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Samsung.h"

namespace esphome
{
    namespace samsung
    {
        class SamsungClimate : public ir_remote_base::IrRemoteBase
        {
        public:
            SamsungClimate()
                : IrRemoteBase(kSamsungAcMinTemp, kSamsungAcMaxTemp, 1.0f, true, true,
                               {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                               {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}) {}

            void setup() override;

        protected:
            void transmit_state() override;

        private:
            void send(const uint16_t nbytes);
            void send_extended();
            void apply_state();

            IRSamsungAc ac_ = IRSamsungAc(255); // pin is not used
            bool _lastsentpowerstate;
        };

    } // namespace samsung
} // namespace esphome
