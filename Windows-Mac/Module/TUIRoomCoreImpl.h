﻿// Copyright (c) 2021 Tencent. All rights reserved.
#ifndef MODULE_TUIROOMCOREIMPL_H_
#define MODULE_TUIROOMCOREIMPL_H_

#include "include/TUIRoomCore.h"
#include "ITRTCCloud.h"
#include "TRTCCloudCallback.h"
#include "IM/IMCoreCallback.h"
#include "IM/IMCore.h"
#include "CommonDef.h"
#include <unordered_map>

class TUIRoomCoreImpl : public TUIRoomCore
    , public liteav::ITRTCCloudCallback
    , public liteav::ITRTCLogCallback
    , public liteav::ITRTCAudioFrameCallback
    , public IMCoreCallback {
 public:
    TUIRoomCoreImpl();
    ~TUIRoomCoreImpl() override;

    /*************************
    * 回调事件接口。
    *************************/
    void SetCallback(TUIRoomCoreCallback* callback) override;

    /*************************
    * 基础接口。
    *************************/
    int Login(int sdk_appid, const std::string& user_id, const std::string& user_sig) override;
    int Logout() override;
    int CreateRoom(const std::string& room_id, TUISpeechMode speech_mode) override;
    int DestroyRoom() override;
    int EnterRoom(const std::string& room_id) override;
    int LeaveRoom() override;
    TUIRoomInfo GetRoomInfo() override;
    std::vector<TUIUserInfo> GetRoomUsers() override;
    const TUIUserInfo* GetUserInfo(const std::string& user_id) override;
    int SetSelfProfile(const std::string& user_name, const std::string& avatar_url) override;
    int TransferRoomMaster(const std::string& user_id) override;

    /*************************
    * 获取SDK版本。
    *************************/
    const char* GetSDKVersion() override;

    /*************************
    * 本地推流接口。
    *************************/
    int StartCameraPreview(const liteav::TXView& view) override;
    int StopCameraPreview() override;
    int UpdateCameraPreview(const liteav::TXView& view) override;
    int StartLocalAudio(const liteav::TRTCAudioQuality& quality) override;
    int StopLocalAudio() override;
    int StartSystemAudioLoopback() override;
    int StopSystemAudioLoopback() override;
    int SetVideoMirror(bool mirror) override;
    int OpenAINoiseReduction() override;
    int CloseAINoiseReduction() override;

    /*************************
    * 远端用户接口。
    *************************/
    int StartRemoteView(const std::string& user_id, const liteav::TXView& view, TUIStreamType type = TUIStreamType::kStreamTypeCamera) override;
    int StopRemoteView(const std::string& user_id, TUIStreamType type = TUIStreamType::kStreamTypeCamera) override;
    int UpdateRemoteView(const std::string& user_id, TUIStreamType type, const liteav::TXView& view) override;
    /*************************
    * 发送消息。
    *************************/
    int SendChatMessage(const std::string& message) override;
    int SendCustomMessage(const std::string& message) override;

    /*************************
    * 场控相关。
    *************************/
    // 主持人禁成员麦克风，成员端回收到OnMuteMic回调。
    int MuteUserMicrophone(const std::string& user_id, bool mute, Callback callback) override;
    int MuteAllUsersMicrophone(bool mute) override;

    // 禁用禁成员摄像头，成员端回收到OnMuteCamera回调。
    int MuteUserCamera(const std::string& user_id, bool mute, Callback callback) override;
    int MuteAllUsersCamera(bool mute) override;

    // 主持人禁止房间内IM聊天，成员端会收到OnMuteChatMessage回调。
    int MuteChatRoom(bool mute) override;

    // 主持人踢人，成员端会收到OnExitRoom(1, "")回调。
    int KickOffUser(const std::string& user_id, Callback callback) override;

    // 主持人点名
    int StartCallingRoll() override;
    int StopCallingRoll() override;

    // 成员回复主持人点名
    int ReplyCallingRoll(Callback callback) override;

    // 主持人邀请成员发言。
    int SendSpeechInvitation(const std::string& user_id, Callback callback) override;
    // 主持人取消邀请成员发言。
    int CancelSpeechInvitation(const std::string& user_id, Callback callback) override;
    // 成员同意/拒绝主持人的发言邀请
    int ReplySpeechInvitation(bool agree, Callback callback) override;

    // 成员申请发言
    int SendSpeechApplication(Callback callback) override;
    // 成员取消申请发言
    int CancelSpeechApplication(Callback callback) override;
    // 主持人回复成员的发言申请
    int ReplySpeechApplication(const std::string& user_id, bool agree, Callback callback) override;
    // 主持人禁止申请发言
    int ForbidSpeechApplication(bool forbid) override;

    // 主持人令成员停止发言
    int SendOffSpeaker(const std::string& user_id, Callback callback) override;
    // 主持人令所有成员停止发言
    int SendOffAllSpeakers(Callback callback) override;

    // 成员直接停止发言，如果成员在台上，则直接停止发言。
    int ExitSpeechState() override;

    /*************************
    * 基础组件接口。
    *************************/
    liteav::ITXDeviceManager* GetDeviceManager() override;
    IScreenShareManager* GetScreenShareManager() override;

    /*************************
    * 云录制接口。
    *************************/
    int StartCloudRecord() override;
    int StopCloudRecord() override;

    /*************************
    * 美颜功能接口。
    *************************/
    int SetBeautyStyle(liteav::TRTCBeautyStyle style, uint32_t beauty_level,
        uint32_t whiteness_level, uint32_t ruddiness_level) override;

    /*************************
    * 设置网络流控相关参数（例如弱网下是“保清晰”还是“保流畅”）。
    *************************/
    int SetVideoQosPreference(TUIVideoQosPreference preference) override;

    /*************************
    * 设置显示仪表盘。
    *************************/
    int ShowDebugView(int showType) override;
 public:
    // TRTC回调函数
    void onEnterRoom(int result) override;
    void onExitRoom(int reason) override;

    void onUserVideoAvailable(const char* user_id, bool available) override;
    void onUserSubStreamAvailable(const char* user_id, bool available) override;
    void onScreenCaptureStoped(int reason) override;
    void onScreenCaptureStarted() override;
    void onUserAudioAvailable(const char* user_id, bool available) override;
    void onFirstVideoFrame(const char* user_id, const liteav::TRTCVideoStreamType stream_type,
        const int width, const int height) override;
    void onRemoteUserEnterRoom(const char* user_id) override;
    void onRemoteUserLeaveRoom(const char* user_id, int reason) override;
    void onUserVoiceVolume(liteav::TRTCVolumeInfo* user_volumes, uint32_t user_volumes_count, uint32_t total_volume) override;
    void onError(TXLiteAVError error_code, const char* error_message, void* extra_info) override;
    void onWarning(TXLiteAVWarning warning_code, const char* warning_message, void* extra_info) override;
    void onLog(const char* log, liteav::TRTCLogLevel level, const char* module) override;

    void onTestSpeakerVolume(uint32_t volume) override;
    void onTestMicVolume(uint32_t volume) override;
    void onAudioDeviceCaptureVolumeChanged(uint32_t volume, bool muted) override;
    void onAudioDevicePlayoutVolumeChanged(uint32_t volume, bool muted) override;

    void onNetworkQuality(liteav::TRTCQualityInfo local_quality, liteav::TRTCQualityInfo* remote_quality,
        uint32_t remote_quality_count) override;
    void onStatistics(const liteav::TRTCStatistics& statistics) override;

    // IM回调函数
    void OnIMError(int code, const std::string& message) override;
    void OnIMLogin(int code, const std::string& message) override;
    void OnIMLogout(int code, const std::string& message) override;
    void OnIMCreateRoom(int code, const std::string& message) override;
    void OnIMDestroyRoom(int code, const std::string& message) override;
    void OnIMEnterRoom(int code, const std::string& message) override;
    void OnIMExitRoom(TUIExitRoomType code, const std::string& message) override;
    void OnIMUserEnterRoom(int code, const std::string& user_id, const std::string& user_name) override;
    void OnIMUserExitRoom(int code, const std::string& user_id) override;
    void OnIMRoomMasterChanged(const std::string& user_id) override;
    void OnIMGetRoomMemberInfoList(const std::vector<TUIUserInfo>& member_array) override;
    void OnIMGetRoomInfo(const TUIRoomInfo& info) override;

    void OnIMReceiveChatMessage(const std::string& user_id, const std::string& message) override;
    void OnIMReceiveCustomMessage(const std::string& user_id, const std::string& message) override;
    void OnIMChatRoomMuted(bool muted) override;

    // 邀请相关
    void OnIMReceiveSpeechInvitation() override;
    void OnIMReceiveInvitationCancelled() override;
    void OnIMReceiveReplyToSpeechInvitation(const std::string& user_id, bool agree) override;

    // 申请相关
    void OnIMReceiveSpeechApplication(const std::string& user_id) override;
    void OnIMSpeechApplicationCancelled(const std::string& user_id) override;
    void OnIMReceiveReplyToSpeechApplication(bool agree) override;
    void OnIMSpeechApplicationForbidden(bool forbidden) override;

    void OnIMOrderedToExitSpeechkState() override;
    // 点名相关
    void OnIMCallingRollStarted() override;
    void OnIMCallingRollStopped() override;
    void OnIMMemberReplyCallingRoll(const std::string& user_id) override;
    void OnIMMicrophoneMuted(bool muted) override;
    void OnIMAllUsersMicrophoneMuted(bool muted) override;
    void OnIMCameraMuted(bool muted) override;
    void OnIMAllUsersCameraMuted(bool muted) override;

 private:
    void EnterTRTCRoom();
    void ClearRoomInfo();

 private:
    liteav::ITRTCCloud*           trtc_cloud_ = nullptr;
    TUIRoomCoreCallback*          room_core_callback_ = nullptr;
    liteav::ITXDeviceManager*     device_manager_ = nullptr;
    IScreenShareManager*          screen_share_manager_ = nullptr;

    TUIUserInfo                   local_user_info_;
    TUIRoomInfo                   room_info_;
    std::unordered_map<std::string, TUIUserInfo> room_user_map_;

    IMCore*         im_core_ = nullptr;
    bool            enter_room_success_ = false;
    bool            camera_mirror_ = false;
    int             sdk_app_id_;
    std::string     user_sig_;
    std::string     sdk_version_;
    liteav::TRTCAudioQuality audio_quality_ = liteav::TRTCAudioQualityDefault;
};

#endif  //  MODULE_TUIROOMCOREIMPL_H_
