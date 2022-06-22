#ifndef CARDBOARD_CARDBOARD_INPUT_UNITY_API_H
#define CARDBOARD_CARDBOARD_INPUT_UNITY_API_H

#include "include/cardboard.h"
#include "cardboard_xr_plugin_log.h"
#include "cardboard_input_api.h"

namespace cardboard::unity {

#ifdef __cplusplus
    extern "C" {
#endif

/// @brief Sets the orientation of the device viewport to use.
/// @param viewport_orientation The orientation of the viewport to use.
    void CardboardUnity_setViewportOrientation(CardboardViewportOrientation viewport_orientation);

/// @brief Flags a head tracker recentering request.
    void CardboardUnity_recenterHeadTracker();

#ifdef __cplusplus
    }
#endif

}
#endif //CARDBOARD_CARDBOARD_INPUT_UNITY_API_H
