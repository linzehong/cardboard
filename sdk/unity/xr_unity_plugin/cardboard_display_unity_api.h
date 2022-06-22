#ifndef CARDBOARD_CARDBOARD_DISPLAY_UNITY_API_H
#define CARDBOARD_CARDBOARD_DISPLAY_UNITY_API_H

#include "cardboard_display_api.h"

namespace cardboard::unity {

#ifdef __cplusplus
    extern "C" {
#endif

/// @brief Sets screen dimensions and rendering area rectangle in pixels.
/// @details It is expected to be called at
///          CardboardXRLoader::Initialize() from C# code when loading the
///          provider. Provided parameters will be returned by
///          CardboardDisplayApi::GetScreenParams().
/// @param[in] screen_width The width of the screen in pixels.
/// @param[in] screen_height The height of the screen in pixels.
/// @param[in] viewport_x x coordinate in pixels of the lower left corner of the
///            rendering area rectangle.
/// @param[in] viewport_y y coordinate in pixels of the lower left corner of the
///            rendering area rectangle.
/// @param[in] viewport_width The width of the rendering area rectangle in
///            pixels.
/// @param[in] viewport_height The height of the rendering area rectangle in
///            pixels.
    void CardboardUnity_setScreenParams(int screen_width, int screen_height, int viewport_x, int viewport_y, int viewport_width, int viewport_height);

/// @brief Sets the total number of widgets to draw.
///
/// @param[in] count The number of widgets that will be drawn.
    void CardboardUnity_setWidgetCount(int count);

/// @brief Sets the the parameters of how to draw a specific widget.
///
/// @param[in] i The widget index to set.
/// @param[in] texture The widget texture as a Texture.GetNativeTexturePtr, @see
///            https://docs.unity3d.com/ScriptReference/Texture.GetNativeTexturePtr.html.
/// @param[in] x x coordinate in pixels of the lower left corner of the
///            rectangle.
/// @param[in] y y coordinate in pixels of the lower left corner of the
///            rectangle.
/// @param[in] width The width in pixels of the rectangle.
/// @param[in] height The height in pixels of the rectangle.
    void CardboardUnity_setWidgetParams(int i, void* texture, int x, int y, int width, int height);

/// @brief Flags a change in device parameters configuration.
    void CardboardUnity_setDeviceParametersChanged();

/// @brief Sets the graphics API to use.
/// @param graphics_api The graphics API to use.
    void CardboardUnity_setGraphicsApi(CardboardGraphicsApi graphics_api);

#ifdef __cplusplus
    }
#endif
}  // namespace cardboard::unity

#endif //CARDBOARD_CARDBOARD_DISPLAY_UNITY_API_H