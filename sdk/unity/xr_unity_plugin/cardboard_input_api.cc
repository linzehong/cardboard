/*
 * Copyright 2021 Google LLC
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
#include "unity/xr_unity_plugin/cardboard_input_api.h"

#include <atomic>
#include <cstdint>

#include "include/cardboard.h"

namespace cardboard::unity {

std::atomic<CardboardViewportOrientation> CardboardInputApi::selected_viewport_orientation_(kLandscapeLeft);

std::atomic<bool> CardboardInputApi::head_tracker_recenter_requested_(false);

void CardboardInputApi::InitHeadTracker() {
  if (head_tracker_ == nullptr) {
    head_tracker_.reset(CardboardHeadTracker_create());
  }
  CardboardHeadTracker_resume(head_tracker_.get());
}

void CardboardInputApi::PauseHeadTracker() {
  if (head_tracker_ == nullptr) {
    LOGW("Uninitialized head tracker was paused.");
    return;
  }
  CardboardHeadTracker_pause(head_tracker_.get());
}

void CardboardInputApi::ResumeHeadTracker() {
  if (head_tracker_ == nullptr) {
    LOGW("Uninitialized head tracker was resumed.");
    return;
  }
  CardboardHeadTracker_resume(head_tracker_.get());
}

void CardboardInputApi::GetHeadTrackerPose(float* position,
                                           float* orientation) {
  if (head_tracker_ == nullptr) {
    LOGW("Uninitialized head tracker was queried for the pose.");
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = 0.0f;
    orientation[0] = 0.0f;
    orientation[1] = 0.0f;
    orientation[2] = 0.0f;
    orientation[3] = 1.0f;
    return;
  }

  // Checks whether a head tracker recentering has been requested.
  if (head_tracker_recenter_requested_) {
    CardboardHeadTracker_recenter(head_tracker_.get());
    head_tracker_recenter_requested_ = false;
  }

  CardboardHeadTracker_getPose(
      head_tracker_.get(), GetBootTimeNano() + kPredictionTimeWithoutVsyncNanos,
      selected_viewport_orientation_, position, orientation);
}

void CardboardInputApi::SetViewportOrientation(
    CardboardViewportOrientation viewport_orientation) {
  selected_viewport_orientation_ = viewport_orientation;
}

void CardboardInputApi::SetHeadTrackerRecenterRequested() {
  head_tracker_recenter_requested_ = true;
}

int64_t CardboardInputApi::GetBootTimeNano() {
  struct timespec res;
#if defined(__ANDROID__)
  clock_gettime(CLOCK_BOOTTIME, &res);
#elif defined(__APPLE__)
  clock_gettime(CLOCK_UPTIME_RAW, &res);
#endif
  return (res.tv_sec * CardboardInputApi::kNanosInSeconds) + res.tv_nsec;
}

}  // namespace cardboard::unity
