//
// Created by keiyo_000 on 2021/2/1.
//

#ifndef VISION_VISION_H
#define VISION_VISION_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
//#include <pose_data.h>
//#include <dof_structure.h>
//#include <gesture_data.h>
//#include "image_tracker_info.h"
//#include <controller_event.h>
//
//#include <android/native_window_jni.h>
//#include <common_defines.h>
//#include <calib_params_info.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /**
     * Quaternion
     */
    float rx, ry, rz, rw = 1.0f;
    /**
     * Translation
     */
    float tx, ty, tz;
} Pose;

typedef struct {
    Pose head_pose;

    /**
    * The event trigger time in nanoseconds.
    */
    int64_t timestamp;
    /**
     *  glass imu timestamp in nanoseconds;
     */
    int64_t  device_timestamp;

    void extractFloat(float values[9]) const {
        values[0] = head_pose.tx;
        values[1] = head_pose.ty;
        values[2] = head_pose.tz;
        values[3] = head_pose.rw;
        values[4] = head_pose.rx;
        values[5] = head_pose.ry;
        values[6] = head_pose.rz;

    }

    void setFloat(float values[9]) {
        //@modify 2021/11/24 order change:tx/ty/tz/rw/rx/ry/rz
        head_pose.tx = values[0];
        head_pose.ty = values[1];
        head_pose.tz = values[2];
        head_pose.rw = values[3];
        head_pose.rx = values[4];
        head_pose.ry = values[5];
        head_pose.rz = values[6];


    }
} PoseData;

enum DofDataType {
    ANDROID_DOF,
    CONTROLLER_DOF,
    GLASS_DOF
};

/**
* Get the Controller Pose
* @param dofDataType
* @return
*/
PoseData VisionController_getControllerPose(DofDataType dofDataType);


//
///**
// * Initialize async time wrap render environment, called once the app starts.
// * @return
// */
//bool VisionRendering_init();
//
///**
// * Deinitialize time wrap render environment, called once when the app exits.
// * @return
// */
//bool VisionRendering_unInit();
//
//
//void VisionRendering_preSetEyeTexture(unsigned int left_eye_texture_name, unsigned int right_eye_texture_name);
//
///**
// * Set eye textures
// * @param left_eye_texture
// * @param right_eye_texture
// */
//void VisionRendering_setEyeTexture(const render::GLTexture &left_eye_texture,
//                                   const render::GLTexture &right_eye_texture);
//
///**
// * Create a new time wrap render instance, called every time the render surface changes.
// * @param display
// * @param context
// * @param surface
// * @param texture_name texture that screen should show
// * @return
// */
//bool VisionRendering_create(EGLDisplay display, EGLContext context, EGLSurface surface);
//
///**
// * Destroy the current time wrap render instance, , called every time the render surface destroyed.
// * @param release_context if true, the associated opengl context and surface will be released.
// */
//void VisionRendering_destroy(bool release_context);
//
///**
//* Gets the predicted head pose for a given timestamp.
//* @param timestamp The timestamp for the pose in nanoseconds in system realtime clock.
//* @return
//*/
//PoseData VisionHeadTracker_getGlassPose(int64_t timestamp);
//
///**
// * Config the alg mode , the server could received the config and turn on/turn off the related algrithom.
// */
//void VisionAlg_configAlgMode(DofModeConfig *configMode);
//
//
///**
// * Register device ready callback.
// * call from Unity.
// * @param callback
// */
//void VisionDevice_RegisterDeviceReadyCallback(VisionDevice_DeviceReadyCallback callback);
//
//
///**
//* Get device parameters.
//* @return
//*/
//DeviceParams VisionDevice_getDeviceParams();
//
///**
// * Get the Gesture Info
// * @return
// */
//GestureInfo VisionGestureTracker_getGestureInfo(_HandType handType);
//
///**
// * Get the L3 Level Gesture Info
// * @param handType
// * @return
// */
//L3_Hand VisionGestureTracker_getL3GestureInfo(_HandType handType);
//
///**
// * Get the Controller Pose
// * @param dofDataType
// * @return
// */
//PoseData VisionController_getControllerPose(DofDataType dofDataType);
//
//
///**
// * Get the Controller KeyEvent
// * @return
// */
//ControllerKeyEvent VisionController_getControllerKeyEvent();
//
///**
// * Get the Controller TouchEvent
// * @return
// */
//ControllerTouchEvent VisionController_getControllerTouchEvent();
//
///**
// * Get the Glass Type
// */
//
//GlassTypeEnum VisionDevice_getGlassType();
//
///**
// * @brief Get the Rendering Way.
// * @return
// */
//RenderingWayEnum VisionRendering_getRenderingWay();
//
///**
// * @brief Get the ImageTracker pose info.
// * @return
// */
//ImageTrackerPose VisionImageTracker_getImageTrackerPoseInfo();
//
//
///**
// * get the fisheye calibration params.
// * @param position  left or right
// * @return
// */
//MCameraCalib VisionCalib_getFishEyeCalib(_Position position);
//
///**
// * get the rgb camera calibration params
// * @return
// */
//MCameraCalib VisionCalib_getRGBCalib();
//
///**
// * get the virtual camera calibration params.
// * @param position  left or right
// * @return
// */
//MVCameraCalib VisionCalib_getVirCamCalib(_Position position);

#ifdef __cplusplus
}
#endif

#endif //VISION_VISION_H
