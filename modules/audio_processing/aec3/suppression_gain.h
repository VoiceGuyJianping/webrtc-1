/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_SUPPRESSION_GAIN_H_
#define MODULES_AUDIO_PROCESSING_AEC3_SUPPRESSION_GAIN_H_

#include <array>
#include <vector>

#include "api/audio/echo_canceller3_config.h"
#include "modules/audio_processing/aec3/aec3_common.h"
#include "modules/audio_processing/aec3/aec_state.h"
#include "modules/audio_processing/aec3/render_signal_analyzer.h"
#include "rtc_base/constructormagic.h"

namespace webrtc {

class SuppressionGain {
 public:
  SuppressionGain(const EchoCanceller3Config& config,
                  Aec3Optimization optimization);
  void GetGain(const std::array<float, kFftLengthBy2Plus1>& nearend,
               const std::array<float, kFftLengthBy2Plus1>& echo,
               const std::array<float, kFftLengthBy2Plus1>& comfort_noise,
               const RenderSignalAnalyzer& render_signal_analyzer,
               const AecState& aec_state,
               const std::vector<std::vector<float>>& render,
               float* high_bands_gain,
               std::array<float, kFftLengthBy2Plus1>* low_band_gain);

  // Toggles the usage of the initial state.
  void SetInitialState(bool state);

 private:
  void LowerBandGain(bool stationary_with_low_power,
                     const rtc::Optional<int>& narrow_peak_band,
                     const AecState& aec_state,
                     const std::array<float, kFftLengthBy2Plus1>& nearend,
                     const std::array<float, kFftLengthBy2Plus1>& echo,
                     const std::array<float, kFftLengthBy2Plus1>& comfort_noise,
                     std::array<float, kFftLengthBy2Plus1>* gain);

  // Limits the gain increase.
  void UpdateGainIncrease(
      bool low_noise_render,
      bool linear_echo_estimate,
      const std::array<float, kFftLengthBy2Plus1>& echo,
      const std::array<float, kFftLengthBy2Plus1>& new_gain);

  class LowNoiseRenderDetector {
   public:
    bool Detect(const std::vector<std::vector<float>>& render);

   private:
    float average_power_ = 32768.f * 32768.f;
  };

  const Aec3Optimization optimization_;
  const EchoCanceller3Config config_;
  const int state_change_duration_blocks_;
  float one_by_state_change_duration_blocks_;
  std::array<float, kFftLengthBy2Plus1> last_gain_;
  std::array<float, kFftLengthBy2Plus1> last_masker_;
  std::array<float, kFftLengthBy2Plus1> gain_increase_;
  std::array<float, kFftLengthBy2Plus1> last_echo_;

  LowNoiseRenderDetector low_render_detector_;
  size_t no_saturation_counter_ = 0;
  bool initial_state_ = true;
  int initial_state_change_counter_ = 0;
  RTC_DISALLOW_COPY_AND_ASSIGN(SuppressionGain);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_SUPPRESSION_GAIN_H_
