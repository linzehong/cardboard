#include "cardboard_display_unity_api.h"

namespace cardboard::unity {

#ifdef __cplusplus
    extern "C" {
#endif

    void
    CardboardUnity_setScreenParams(int screen_width, int screen_height, int viewport_x, int viewport_y, int viewport_width, int viewport_height) {
        cardboard::unity::CardboardDisplayApi::SetUnityScreenParams(screen_width, screen_height, viewport_x, viewport_y, viewport_width, viewport_height);
    }

    void CardboardUnity_setDeviceParametersChanged() {
        cardboard::unity::CardboardDisplayApi::SetDeviceParametersChanged();
    }

    void CardboardUnity_setWidgetCount(int count) {
        cardboard::unity::CardboardDisplayApi::SetWidgetCount(count);
    }

    void CardboardUnity_setWidgetParams(int i, void *texture, int x, int y, int width, int height) {
        cardboard::unity::Renderer::WidgetParams params;

        params.texture = reinterpret_cast<intptr_t>(texture);
        params.x = x;
        params.y = y;
        params.width = width;
        params.height = height;
        cardboard::unity::CardboardDisplayApi::SetWidgetParams(i, params);
    }

    void CardboardUnity_setGraphicsApi(CardboardGraphicsApi graphics_api) {
        switch (graphics_api) {
            case CardboardGraphicsApi::kOpenGlEs2:
                LOGD("Configured OpenGL ES2.0 as Graphics API.");
                cardboard::unity::CardboardDisplayApi::SetGraphicsApi(graphics_api);
                break;
            case CardboardGraphicsApi::kOpenGlEs3:
                LOGD("Configured OpenGL ES3.0 as Graphics API.");
                cardboard::unity::CardboardDisplayApi::SetGraphicsApi(graphics_api);
                break;
#if defined(__APPLE__)
                case CardboardGraphicsApi::kMetal:
      LOGD("Configured Metal as Graphics API.");
      cardboard::unity::CardboardDisplayApi::SetGraphicsApi(graphics_api);
      break;
#endif
            default:
                LOGE(
                        "Misconfigured Graphics API. Neither OpenGL ES 2.0 nor OpenGL ES 3.0 "
                        "nor Metal was selected.");
        }
    }

#ifdef __cplusplus
    }
#endif

}  // namespace cardboard::unity
