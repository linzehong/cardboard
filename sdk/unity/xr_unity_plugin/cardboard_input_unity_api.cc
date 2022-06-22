#include "cardboard_input_unity_api.h"

namespace cardboard::unity {

#ifdef __cplusplus
    extern "C" {
#endif

    void CardboardUnity_setViewportOrientation(CardboardViewportOrientation viewport_orientation) {
        switch (viewport_orientation) {
            case CardboardViewportOrientation::kLandscapeLeft:
                LOGD("Configured viewport orientation as landscape left.");
                cardboard::unity::CardboardInputApi::SetViewportOrientation(viewport_orientation);
                break;
            case CardboardViewportOrientation::kLandscapeRight:
                LOGD("Configured viewport orientation as landscape right.");
                cardboard::unity::CardboardInputApi::SetViewportOrientation(viewport_orientation);
                break;
            case CardboardViewportOrientation::kPortrait:
                LOGD("Configured viewport orientation as portrait.");
                cardboard::unity::CardboardInputApi::SetViewportOrientation(viewport_orientation);
                break;
            case CardboardViewportOrientation::kPortraitUpsideDown:
                LOGD("Configured viewport orientation as portrait upside down.");
                cardboard::unity::CardboardInputApi::SetViewportOrientation(viewport_orientation);
                break;
            default:
                LOGE(
                        "Misconfigured viewport orientation. Neither landscape left nor "
                        "lanscape right "
                        "nor portrait, nor portrait upside down was selected.");
        }
    }

    void CardboardUnity_recenterHeadTracker() {
        cardboard::unity::CardboardInputApi::SetHeadTrackerRecenterRequested();
    }

#ifdef __cplusplus
    }
#endif

}
