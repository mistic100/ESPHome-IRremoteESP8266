#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/optional.h"
#include "esphome/components/ir_remote_base/ir_remote_base.h"
#include "ir_Haier.h"

namespace esphome
{
    namespace haier
    {
        enum Model
        {
            HAIER_AC,
            HAIER_AC160,
            HAIER_AC176,
            HAIER_AC_YRW02
        };

        class HaierClimate : public ir_remote_base::IrRemoteBase
        {
        public:
            HaierClimate()
                : IrRemoteBase(kHaierAcMinTemp, kHaierAcMaxTemp, 1.0f, true, true,
                               {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH},
                               {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL}) {}

            void set_model(const Model model);
            void setup() override;

        protected:
            void transmit_state() override;
            void send() override;
            void apply_state() override;

        private:
            void apply_state_ac();
            void apply_state_ac160();
            void apply_state_ac176();
            void apply_state_acyrw02();

            Model model_ = Model::HAIER_AC;
            IRHaierAC ac_ = IRHaierAC(255); // pin is not used
            IRHaierAC160 ac160_ = IRHaierAC160(255);
            IRHaierAC176 ac176_ = IRHaierAC176(255);
            IRHaierACYRW02 acyrw02_ = IRHaierACYRW02(255);
        };

    } // namespace haier
} // namespace esphome
