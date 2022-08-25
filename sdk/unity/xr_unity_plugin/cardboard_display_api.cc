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
#include "unity/xr_unity_plugin/cardboard_display_api.h"

#include <time.h>
#include <unistd.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>  // NOLINT(build/c++11)
#include <string>
#include <vector>

#include "include/cardboard.h"

namespace cardboard::unity {

std::atomic<CardboardDisplayApi::ScreenParams>
    CardboardDisplayApi::unity_screen_params_({0, 0, 0, 0, 0, 0});

std::vector<Renderer::WidgetParams> CardboardDisplayApi::widget_params_;

std::mutex CardboardDisplayApi::widget_mutex_;

std::atomic<bool> CardboardDisplayApi::device_params_changed_(true);

std::atomic<CardboardGraphicsApi> CardboardDisplayApi::selected_graphics_api_(kNone);

IUnityInterfaces* CardboardDisplayApi::xr_interfaces_{nullptr};

CardboardDisplayApi::CardboardDisplayApi() {
  // 目前只支持OpenGLEs3，直接设置
  selected_graphics_api_ = kOpenGlEs3;

  switch (selected_graphics_api_) {
    case CardboardGraphicsApi::kOpenGlEs2:
      renderer_ = MakeOpenGlEs2Renderer();
      break;
    case CardboardGraphicsApi::kOpenGlEs3:
      renderer_ = MakeOpenGlEs3Renderer();
      break;
#if defined(__APPLE__)
    case CardboardGraphicsApi::kMetal:
      renderer_ = MakeMetalRenderer(xr_interfaces_);
      break;
#endif
    default:
      LOGF(
          "The Cardboard SDK cannot be initialized given that the selected "
          "Graphics API (%d) is not supported. Please use OpenGL ES 2.0, "
          "OpenGL ES 3.0 or Metal.",
          static_cast<int>(selected_graphics_api_));
      break;
  }
}

CardboardDisplayApi::~CardboardDisplayApi() { RenderingResourcesTeardown(); }

void CardboardDisplayApi::ScanDeviceParams() {
  CardboardQrCode_scanQrCodeAndSaveDeviceParams();
}

void CardboardDisplayApi::UpdateDeviceParams() {
  if (selected_graphics_api_ == kNone) {
    LOGE(
        "Misconfigured Graphics API. Neither OpenGL ES 2.0 nor OpenGL ES 3.0 "
        "nor Metal was selected.");
    return;
  }
  LOGD("[testing] UpdateDeviceParams");

  // Updates the screen size.
  screen_params_ = unity_screen_params_;

  // Get saved device parameters
  uint8_t* data;
  int size;
  CardboardQrCode_getSavedDeviceParams(&data, &size);
  CardboardLensDistortion* lens_distortion;
  if (size == 0) {
    // Loads Cardboard V1 device parameters when no device parameters are
    // available.
    CardboardQrCode_getCardboardV1DeviceParams(&data, &size);
    lens_distortion = CardboardLensDistortion_create(
        data, size, screen_params_.viewport_width,
        screen_params_.viewport_height);
  } else {
    lens_distortion = CardboardLensDistortion_create(
        data, size, screen_params_.viewport_width,
        screen_params_.viewport_height);
    CardboardQrCode_destroy(data);
  }
  device_params_changed_ = false;

  RenderingResourcesSetup();

  switch (selected_graphics_api_) {
    case CardboardGraphicsApi::kOpenGlEs2:
      distortion_renderer_.reset(CardboardOpenGlEs2DistortionRenderer_create());
      break;
    case CardboardGraphicsApi::kOpenGlEs3:
      // #gles3 - This call is only needed if OpenGL ES 3.0 support is
      // desired. Remove the following line if OpenGL ES 3.0 is not needed.
      distortion_renderer_.reset(CardboardOpenGlEs3DistortionRenderer_create());
      break;
#if defined(__APPLE__)
    case CardboardGraphicsApi::kMetal:
      distortion_renderer_.reset(
          MakeCardboardMetalDistortionRenderer(xr_interfaces_));
      break;
#endif
    default:
      LOGF(
          "The distortion renderer cannot be initialized given that the "
          "selected Graphics API (%d) is not supported. Please use OpenGL ES "
          "2.0, OpenGL ES 3.0 or Metal.",
          static_cast<int>(selected_graphics_api_));
      break;
  }

  CardboardLensDistortion_getDistortionMesh(
      lens_distortion, CardboardEye::kLeft,
      &eye_data_[CardboardEye::kLeft].distortion_mesh);
  CardboardLensDistortion_getDistortionMesh(
      lens_distortion, CardboardEye::kRight,
      &eye_data_[CardboardEye::kRight].distortion_mesh);

  CardboardDistortionRenderer_setMesh(
      distortion_renderer_.get(),
      &eye_data_[CardboardEye::kLeft].distortion_mesh, CardboardEye::kLeft);
  CardboardDistortionRenderer_setMesh(
      distortion_renderer_.get(),
      &eye_data_[CardboardEye::kRight].distortion_mesh, CardboardEye::kRight);

  // Get eye matrices
  CardboardLensDistortion_getEyeFromHeadMatrix(
      lens_distortion, CardboardEye::kLeft,
      eye_data_[CardboardEye::kLeft].eye_from_head_matrix);
  CardboardLensDistortion_getEyeFromHeadMatrix(
      lens_distortion, CardboardEye::kRight,
      eye_data_[CardboardEye::kRight].eye_from_head_matrix);
  CardboardLensDistortion_getFieldOfView(lens_distortion, CardboardEye::kLeft,
                                         eye_data_[CardboardEye::kLeft].fov);
  CardboardLensDistortion_getFieldOfView(lens_distortion, CardboardEye::kRight,
                                         eye_data_[CardboardEye::kRight].fov);

  CardboardLensDistortion_destroy(lens_distortion);
}

void CardboardDisplayApi::GetEyeMatrices(int eye, float* eye_from_head,
                                         float* fov) {
  std::memcpy(eye_from_head, eye_data_[eye].eye_from_head_matrix,
              sizeof(float) * 16);
  std::memcpy(fov, eye_data_[eye].fov, sizeof(float) * 4);
}

void CardboardDisplayApi::RenderEyesToDisplay() {
  const Renderer::ScreenParams screen_params{
      screen_params_.width,          screen_params_.height,
      screen_params_.viewport_x,     screen_params_.viewport_y,
      screen_params_.viewport_width, screen_params_.viewport_height};
  renderer_->RenderEyesToDisplay(distortion_renderer_.get(), screen_params,
                                 &eye_data_[CardboardEye::kLeft].texture,
                                 &eye_data_[CardboardEye::kRight].texture);
}

void CardboardDisplayApi::RenderWidgets() {
  std::lock_guard<std::mutex> l(widget_mutex_);
  Renderer::ScreenParams screen_params{
      screen_params_.width,          screen_params_.height,
      screen_params_.viewport_x,     screen_params_.viewport_y,
      screen_params_.viewport_width, screen_params_.viewport_height};
  renderer_->RenderWidgets(screen_params, widget_params_);
}

uint64_t CardboardDisplayApi::GetLeftTextureColorBufferId() {
  return render_textures_[CardboardEye::kLeft].color_buffer;
}

uint64_t CardboardDisplayApi::GetRightTextureColorBufferId() {
  return render_textures_[CardboardEye::kRight].color_buffer;
}

uint64_t CardboardDisplayApi::GetLeftTextureDepthBufferId() {
  return render_textures_[CardboardEye::kLeft].depth_buffer;
}

uint64_t CardboardDisplayApi::GetRightTextureDepthBufferId() {
  return render_textures_[CardboardEye::kRight].depth_buffer;
}

void CardboardDisplayApi::GetScreenParams(int* width, int* height) {
  const ScreenParams screen_params = unity_screen_params_;
  *width = screen_params.viewport_width;
  *height = screen_params.viewport_height;
}

void CardboardDisplayApi::SetUnityScreenParams(int screen_width,
                                               int screen_height,
                                               int viewport_x, int viewport_y,
                                               int viewport_width,
                                               int viewport_height) {
    LOGD("[testing] SetUnityScreenParams screen:(%d, %d),viewport:(%d,%d,%d,%d)",
         screen_width, screen_height, viewport_x, viewport_y, viewport_width, viewport_height);
  unity_screen_params_ =
      ScreenParams{screen_width, screen_height,  viewport_x,
                   viewport_y,   viewport_width, viewport_height};
}

void CardboardDisplayApi::SetDeviceParametersChanged() {
  device_params_changed_ = true;
}

bool CardboardDisplayApi::GetDeviceParametersChanged() {
  return device_params_changed_;
}

void CardboardDisplayApi::SetWidgetCount(int count) {
  std::lock_guard<std::mutex> l(widget_mutex_);
  widget_params_.resize(count);
}

void CardboardDisplayApi::SetWidgetParams(
    int i, const Renderer::WidgetParams& params) {
  std::lock_guard<std::mutex> l(widget_mutex_);
  if (i < 0 || i >= static_cast<int>(widget_params_.size())) {
    LOGE("SetWidgetParams parameter i=%d, out of bounds (size=%d)", i,
         static_cast<int>(widget_params_.size()));
    return;
  }

  widget_params_[i] = params;
}

void CardboardDisplayApi::SetGraphicsApi(CardboardGraphicsApi graphics_api) {
  selected_graphics_api_ = graphics_api;
}

CardboardGraphicsApi CardboardDisplayApi::GetGraphicsApi() {
  return selected_graphics_api_;
}

void CardboardDisplayApi::SetUnityInterfaces(IUnityInterfaces* xr_interfaces) {
  xr_interfaces_ = xr_interfaces;
}

// @brief Configures rendering resources.
void CardboardDisplayApi::RenderingResourcesSetup() {
      LOGD("[testing] UpdateDeviceParams");
  if (selected_graphics_api_ == kNone) {
    LOGE(
        "Misconfigured Graphics API. Neither OpenGL ES 2.0 nor OpenGL ES 3.0 "
        "nor Metal was selected.");
    return;
  }

  if (render_textures_[0].color_buffer != 0) {
    RenderingResourcesTeardown();
  }

  // Create render texture, depth buffer for both eyes and setup widgets.
  renderer_->CreateRenderTexture(&render_textures_[CardboardEye::kLeft],
                                 screen_params_.viewport_width,
                                 screen_params_.viewport_height);
  renderer_->CreateRenderTexture(&render_textures_[CardboardEye::kRight],
                                 screen_params_.viewport_width,
                                 screen_params_.viewport_height);
  renderer_->SetupWidgets();

  // Set texture description structures.
  eye_data_[CardboardEye::kLeft].texture.texture =
      render_textures_[CardboardEye::kLeft].color_buffer;
  eye_data_[CardboardEye::kLeft].texture.left_u = 0;
  eye_data_[CardboardEye::kLeft].texture.right_u = 1;
  eye_data_[CardboardEye::kLeft].texture.top_v = 1;
  eye_data_[CardboardEye::kLeft].texture.bottom_v = 0;

  eye_data_[CardboardEye::kRight].texture.texture =
      render_textures_[CardboardEye::kRight].color_buffer;
  eye_data_[CardboardEye::kRight].texture.left_u = 0;
  eye_data_[CardboardEye::kRight].texture.right_u = 1;
  eye_data_[CardboardEye::kRight].texture.top_v = 1;
  eye_data_[CardboardEye::kRight].texture.bottom_v = 0;
}

void CardboardDisplayApi::RenderingResourcesTeardown() {
  if (render_textures_[0].color_buffer == 0) {
    return;
  }
  renderer_->DestroyRenderTexture(&render_textures_[CardboardEye::kLeft]);
  renderer_->DestroyRenderTexture(&render_textures_[CardboardEye::kRight]);
  renderer_->TeardownWidgets();
}

}  // namespace cardboard::unity
