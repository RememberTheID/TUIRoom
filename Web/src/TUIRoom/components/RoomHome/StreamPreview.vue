<template>
  <div v-loading="loading" element-loading-background="#000000" class="stream-container">
    <div ref="streamPreviewRef" class="stream-preview"></div>
    <div v-if="isCameraMuted" class="stream-info">
      <span class="info">摄像头已关闭</span>
    </div>
    <div class="control-region">
      <icon-button
        ref="audioIconButtonRef"
        title="麦克风"
        :hide-hover-effect="true"
        :has-more="true"
        @click-icon="toggleMuteAudio"
        @click-more="handleMicSetting"
      >
        <audio-icon :audio-volume="localStream.audioVolume" :is-muted="isMicMuted"></audio-icon>
      </icon-button>
      <icon-button
        ref="videoIconButtonRef"
        class="icon"
        title="摄像头"
        :icon-name="cameraIconName"
        :hide-hover-effect="true"
        :has-more="true"
        @click-icon="toggleMuteVideo"
        @click-more="handleCameraSetting"
      />
    </div>
  </div>
  <div class="drawer-container">
    <el-drawer
      v-model="isSettingOpen"
      :modal="false"
      :title="settingTitle"
      direction="rtl"
      custom-class="room-sidebar"
      :before-close="handleDrawerClose"
      :size="480"
      style="pointer-events: none"
    >
      <audio-setting-tab v-if="settingTab === 'audio'" :mode="SettingMode.DETAIL"></audio-setting-tab>
      <video-setting-tab v-if="settingTab === 'video'" :mode="SettingMode.DETAIL"></video-setting-tab>
    </el-drawer>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue';
import TUIRoomCore, { ETUIRoomEvents } from '../../tui-room-core';
import IconButton from '../common/IconButton.vue';
import { onBeforeRouteLeave } from 'vue-router';
import { SettingMode } from '../../constants/render';
import AudioSettingTab from '../base/AudioSettingTab.vue';
import VideoSettingTab from '../base/VideoSettingTab.vue';
import AudioIcon from '../base/AudioIcon.vue';
import { useStreamStore } from '../../stores/stream';
import { storeToRefs } from 'pinia';

const streamStore = useStreamStore();
const { localStream } = storeToRefs(streamStore);

defineExpose({
  getRoomParam,
});

const tuiRoomParam = {
  isOpenCamera: true,
  isOpenMicrophone: true,
  defaultCameraId: '',
  defaultMicrophoneId: '',
  defaultSpeakerId: '',
};

const streamPreviewRef = ref();
const loading = ref(true);

const isMicMuted = ref(false);
const isCameraMuted = ref(false);

const cameraIconName = computed(() => (isCameraMuted.value ? 'camera-off' : 'camera-on'));

function toggleMuteAudio() {
  isMicMuted.value = !isMicMuted.value;
  tuiRoomParam.isOpenMicrophone = !isMicMuted.value;
}

function toggleMuteVideo() {
  isCameraMuted.value = !isCameraMuted.value;
  TUIRoomCore.muteLocalCamera(isCameraMuted.value);
  tuiRoomParam.isOpenCamera = !isCameraMuted.value;
}

function getRoomParam() {
  tuiRoomParam.defaultCameraId = streamStore.currentCameraId;
  tuiRoomParam.defaultMicrophoneId = streamStore.currentMicrophoneId;
  tuiRoomParam.defaultSpeakerId = streamStore.currentSpeakerId;
  return tuiRoomParam;
}

const onUserVoiceVolume = (eventInfo: []) => {
  streamStore.setAudioVolume(eventInfo);
};

onMounted(async () => {
  const cameraList = await TUIRoomCore.getCameraList();
  const microphoneList = await TUIRoomCore.getMicrophoneList();
  streamStore.setCurrentCameraId(cameraList[0].deviceId);
  streamStore.setCurrentMicrophoneId(microphoneList[0].deviceId);
  streamStore.setCurrentSpeakerId(microphoneList[0].deviceId);
  TUIRoomCore.setCurrentCamera(cameraList[0].deviceId);
  TUIRoomCore.setCurrentMicrophone(microphoneList[0].deviceId);
  await TUIRoomCore.startCameraPreview(streamPreviewRef.value);
  TUIRoomCore.enableAudioVolumeEvaluation(100);
  await TUIRoomCore.startMicrophone();
  loading.value = false;
  TUIRoomCore.on(ETUIRoomEvents.onUserVoiceVolume, onUserVoiceVolume);
});

onBeforeRouteLeave(async () => {
  await TUIRoomCore.stopCameraPreview();
  await TUIRoomCore.stopMicrophone();
  TUIRoomCore.off(ETUIRoomEvents.onUserVoiceVolume, onUserVoiceVolume);
});

// ----- 处理音视频设备设置
const isSettingOpen = ref(false);
const settingTitle = ref('');
const settingTab = ref('');

// 处理音频设备设置
function handleMicSetting() {
  isSettingOpen.value = true;
  settingTitle.value = '麦克风设置';
  settingTab.value = 'audio';
}

// 处理视频设备设置
function handleCameraSetting() {
  isSettingOpen.value = true;
  settingTitle.value = '摄像头设置';
  settingTab.value = 'video';
}

function handleDrawerClose(done: any) {
  settingTab.value = '';
  done();
}

</script>

<style lang="scss">
.stream-container {
  width: 740px;
  height: 476px;
  background-color: #12141A;
  border-radius: 10px;
  position: relative;
  overflow: hidden;
  border: 2px solid #1B1E26;
  .stream-preview {
    width: 100%;
    height: 100%;
  }
  .stream-info {
    width: 100%;
    height: 100%;
    position: absolute;
    top: 0;
    display: flex;
    justify-content: center;
    align-items: center;
    .info {
      width: 132px;
      height: 34px;
      font-family: PingFangSC-Regular;
      font-weight: 400;
      font-size: 22px;
      color: #676C80;
      line-height: 34px;
    }
  }
  .control-region {
    height: 74px;
    background: rgba(13,16,21,0.60);
    border-radius: 43px;
    position: absolute;
    bottom: 24px;
    left: 50%;
    transform: translate(-50%);
    padding: 0 50px;
    display: flex;
    .icon {
      &:not(:first-child) {
        margin-left: 30px;
      }
    }
  }
}

.drawer-container > div {
  pointer-events: none;
}

.room-sidebar {
  background-color: #1D2029;
  pointer-events: auto;
}

.el-drawer__header {
  height: 88px;
  border-bottom: 1px solid #2f313b;
  box-sizing: border-box;
  margin-bottom: 0;
  padding: 32px;
}

.el-drawer__body {
  padding: 32px;
}
</style>
