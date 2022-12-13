/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <aidl/android/hardware/audio/effect/BnEffect.h>
#include <fmq/AidlMessageQueue.h>
#include <cstdlib>
#include <memory>

#include "effect-impl/EffectImpl.h"
#include "effect-impl/EffectUUID.h"

namespace aidl::android::hardware::audio::effect {

class LoudnessEnhancerSwContext final : public EffectContext {
  public:
    LoudnessEnhancerSwContext(int statusDepth, const Parameter::Common& common)
        : EffectContext(statusDepth, common) {
        LOG(DEBUG) << __func__;
    }

    RetCode setLeGainMb(int gainMb) {
        // TODO : Add implementation to apply new gain
        mGainMb = gainMb;
        return RetCode::SUCCESS;
    }
    int getLeGainMb() const { return mGainMb; }

  private:
    int mGainMb = 0;  // Default Gain
};

class LoudnessEnhancerSw final : public EffectImpl {
  public:
    LoudnessEnhancerSw() { LOG(DEBUG) << __func__; }
    ~LoudnessEnhancerSw() {
        cleanUp();
        LOG(DEBUG) << __func__;
    }

    ndk::ScopedAStatus getDescriptor(Descriptor* _aidl_return) override;
    ndk::ScopedAStatus setParameterSpecific(const Parameter::Specific& specific) override;
    ndk::ScopedAStatus getParameterSpecific(const Parameter::Id& id,
                                            Parameter::Specific* specific) override;

    std::shared_ptr<EffectContext> createContext(const Parameter::Common& common) override;
    std::shared_ptr<EffectContext> getContext() override;
    RetCode releaseContext() override;

    IEffect::Status effectProcessImpl(float* in, float* out, int samples) override;
    std::string getEffectName() override { return kEffectName; }

  private:
    const std::string kEffectName = "LoudnessEnhancerSw";
    std::shared_ptr<LoudnessEnhancerSwContext> mContext;
    /* capabilities */
    const LoudnessEnhancer::Capability kCapability;
    /* Effect descriptor */
    const Descriptor kDescriptor = {
            .common = {.id = {.type = kLoudnessEnhancerTypeUUID,
                              .uuid = kLoudnessEnhancerSwImplUUID,
                              .proxy = std::nullopt},
                       .flags = {.type = Flags::Type::INSERT,
                                 .insert = Flags::Insert::FIRST,
                                 .volume = Flags::Volume::CTRL},
                       .name = kEffectName,
                       .implementor = "The Android Open Source Project"},
            .capability = Capability::make<Capability::loudnessEnhancer>(kCapability)};

    ndk::ScopedAStatus getParameterLoudnessEnhancer(const LoudnessEnhancer::Tag& tag,
                                                    Parameter::Specific* specific);
};
}  // namespace aidl::android::hardware::audio::effect