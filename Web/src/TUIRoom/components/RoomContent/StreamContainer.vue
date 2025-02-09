<template>
  <div :class="streamContainerClass">
    <div v-show="showIconControl" ref="enlargedContainerRef" class="enlarged-stream-container">
      <stream-region
        v-if="enlargeStream"
        :stream="enlargeStream"
        :style="enlargedStreamStyle"
      ></stream-region>
    </div>
    <div :class="['stream-list-container', `${showSideList ? '' : 'hide-list'}`]">
      <div ref="streamListRef" class="stream-list">
        <stream-region
          v-for="(stream, index) in streamList"
          :key="index"
          :stream="stream"
          :enlarge-dom-id="enlargeDomId"
          class="single-stream"
          :style="streamStyle"
          @dblclick="handleEnlargeStreamRegion(stream)"
        ></stream-region>
      </div>
    </div>
    <div v-if="showIconControl && showRoomTool" :class="arrowClass" @click="handleClickIcon">
      <svg-icon icon-name="line-arrow-up" size="medium"></svg-icon>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted, Ref, watch, nextTick, computed } from 'vue';
import TUIRoomCore, {
  TRTCVideoResolution,
  ETUIRoomEvents,
  TRTCVideoResolutionMode,
  ETUIStreamType,
  TRTCVideoEncParam,
} from '../../tui-room-core';
import { storeToRefs } from 'pinia';
import { StreamInfo, useStreamStore } from '../../stores/stream';
import { useBasicStore } from '../../stores/basic';
import { LAYOUT } from '../../constants/render';
import StreamRegion from './StreamRegion.vue';
import SvgIcon from '../common/SvgIcon.vue';

defineProps<{
  showRoomTool: boolean,
}>();

const streamStyle: Ref<Record<string, any>> = ref({});
const enlargedStreamStyle: Ref<Record<string, any>> = ref({});
const streamStore = useStreamStore();
const { streamList, streamNumber } = storeToRefs(streamStore);
const basicStore = useBasicStore();
const { layout } = storeToRefs(basicStore);
const showSideList = ref(true);

const enlargeStream: Ref<StreamInfo | null> = ref(null);
const enlargeDomId = computed(() => (enlargeStream.value ? `${enlargeStream.value.userId}_${enlargeStream.value.type}` : ''));

// ----- 以下处理流布局 ---------
const streamContainerClass = ref('');
const enlargedContainerRef = ref();
const streamListRef = ref();

const arrowClass = computed(() => {
  let arrowDirection = '';
  if (layout.value === LAYOUT.TOP_SIDE_LIST) {
    arrowDirection = showSideList.value ? 'up' : 'down';
  }
  if (layout.value === LAYOUT.RIGHT_SIDE_LIST) {
    arrowDirection = showSideList.value ? 'right' : 'left';
  }
  return `icon-control arrow-${arrowDirection}`;
});

function handleClickIcon() {
  showSideList.value = !showSideList.value;
  if (!showSideList.value) {
    let width = 0;
    let height = 0;
    const containerWidth = document.getElementById('roomContainer')!.offsetHeight;
    const containerHeight = document.getElementById('roomContainer')!.offsetHeight;
    const scaleWidth = containerWidth / 16;
    const scaleHeight = containerHeight / 9;
    if (scaleWidth > scaleHeight) {
      width = (containerHeight / 9) * 16;
      height = containerHeight;
    }
    if (scaleWidth < scaleHeight) {
      width = containerWidth;
      height = (containerWidth / 16) * 9;
    }
    enlargedStreamStyle.value.width = `${width}px`;
    enlargedStreamStyle.value.height = `${height}px`;
    // 修改 enlargedContainer 的宽和高
    if (layout.value === LAYOUT.RIGHT_SIDE_LIST) {
      enlargedContainerRef.value.style.width = '100%';
      enlargedContainerRef.value.style.height = '100%';
      return;
    }
    if (layout.value === LAYOUT.TOP_SIDE_LIST) {
      enlargedContainerRef.value.style.top = '0px';
      enlargedContainerRef.value.style.width = '100%';
      enlargedContainerRef.value.style.height = '100%';
      return;
    }
  } else {
    if (layout.value  === LAYOUT.RIGHT_SIDE_LIST) {
      handleRightSideListLayout();
      enlargedContainerRef.value.style.width = 'calc(100% - 260px)';
      enlargedContainerRef.value.style.height = '100%';
      return;
    }
    if (layout.value === LAYOUT.TOP_SIDE_LIST) {
      handleTopSideListLayout();
      enlargedContainerRef.value.style.top = '175px';
      enlargedContainerRef.value.style.width = '100%';
      enlargedContainerRef.value.style.height = 'calc(100% - 175px)';
      return;
    }
  }
}

// 处理九宫格布局
async function handleNineEqualPointsLayout() {
  streamContainerClass.value = 'stream-container-flatten';

  enlargeStream.value = null;

  await nextTick();

  const number = streamNumber.value;
  let width = 0;
  let height = 0;
  const roomContainerElement = document.getElementById('roomContainer');
  let containerWidth = roomContainerElement!.offsetWidth;
  let containerHeight = roomContainerElement!.offsetHeight;
  if (number <= 4) {
    containerWidth = number < 2 ? roomContainerElement!.offsetWidth / number : roomContainerElement!.offsetWidth / 2;
    containerHeight = roomContainerElement!.offsetHeight / Math.ceil(number / 2);
  } else if (number > 4) {
    containerWidth = number < 3 ? roomContainerElement!.offsetWidth / number : roomContainerElement!.offsetWidth / 3;
    containerHeight = roomContainerElement!.offsetHeight / Math.ceil(number / 3);
  }

  const scaleWidth = containerWidth / 16;
  const scaleHeight = containerHeight / 9;
  if (scaleWidth > scaleHeight) {
    width = (containerHeight / 9) * 16;
    height = containerHeight;
  }
  if (scaleWidth <= scaleHeight) {
    width = containerWidth;
    height = (containerWidth / 16) * 9;
  }
  streamStyle.value.width = `${width}px`;
  streamStyle.value.height = `${height}px`;
}

// 处理侧边栏布局
async function handleRightSideListLayout() {
  streamContainerClass.value = 'stream-container-right';

  if (!enlargeStream.value) {
    [enlargeStream.value] = streamList.value;
  }

  await nextTick();

  streamStyle.value = {};

  if (enlargedContainerRef.value) {
    const containerWidth = enlargedContainerRef.value.offsetWidth;
    const containerHeight = enlargedContainerRef.value.offsetHeight;
    let width = 0;
    let height = 0;
    const scaleWidth = containerWidth / 16;
    const scaleHeight = containerHeight / 9;
    if (scaleWidth > scaleHeight) {
      width = (containerHeight / 9) * 16;
      height = containerHeight;
    }
    if (scaleWidth < scaleHeight) {
      width = containerWidth;
      height = (containerWidth / 16) * 9;
    }
    enlargedStreamStyle.value.width = `${width}px`;
    enlargedStreamStyle.value.height = `${height}px`;
  }
}

// 处理顶部栏布局
async function handleTopSideListLayout() {
  streamContainerClass.value = 'stream-container-top';

  if (!enlargeStream.value) {
    [enlargeStream.value] = streamList.value;
  }

  await nextTick();

  streamStyle.value = {};

  if (enlargedContainerRef.value) {
    const containerWidth = enlargedContainerRef.value.offsetWidth;
    const containerHeight = enlargedContainerRef.value.offsetHeight;
    let width = 0;
    let height = 0;
    const scaleWidth = containerWidth / 16;
    const scaleHeight = containerHeight / 9;
    if (scaleWidth > scaleHeight) {
      width = (containerHeight / 9) * 16;
      height = containerHeight;
    }
    if (scaleWidth < scaleHeight) {
      width = containerWidth;
      height = (containerWidth / 16) * 9;
    }
    enlargedStreamStyle.value.width = `${width}px`;
    enlargedStreamStyle.value.height = `${height}px`;
  }
}

function handleEnlargeStreamRegion(stream: StreamInfo) {
  if (layout.value === LAYOUT.NINE_EQUAL_POINTS) {
    return;
  }
  enlargeStream.value = stream;
}

function handleResize() {
  switch (layout.value as any) {
    case LAYOUT.NINE_EQUAL_POINTS:
      if (streamListRef.value) {
        streamListRef.value.style.maxWidth = '100%';
        streamListRef.value.style.maxHeight = '100%';
      }
      handleNineEqualPointsLayout();
      break;
    case LAYOUT.RIGHT_SIDE_LIST:
      showSideList.value = true;
      enlargedContainerRef.value.style.width = 'calc(100% - 260px)';
      enlargedContainerRef.value.style.height = '100%';
      streamListRef.value.style.maxWidth = '100%';
      streamListRef.value.style.maxHeight = `${document.getElementById('roomContainer')!.offsetHeight - 128}px`;
      handleRightSideListLayout();
      break;
    case LAYOUT.TOP_SIDE_LIST:
      showSideList.value = true;
      enlargedContainerRef.value.style.width = '100%';
      enlargedContainerRef.value.style.height = 'calc(100% - 175px)';
      streamListRef.value.style.maxHeight = '100%';
      streamListRef.value.style.maxWidth = `${document.getElementById('roomContainer')!.offsetWidth - 80}px`;
      handleTopSideListLayout();
      break;
    default:
      break;
  }
}

onMounted(() => {
  handleResize();
  ['resize', 'pageshow'].forEach((event) => {
    window.addEventListener(event, handleResize);
  });
});

onUnmounted(() => {
  ['resize', 'pageshow'].forEach((event) => {
    window.removeEventListener(event, handleResize);
  });
});

watch(streamNumber, () => {
  if (layout.value === LAYOUT.NINE_EQUAL_POINTS) {
    handleNineEqualPointsLayout();
  }
});

watch(layout, () => {
  handleResize();
});

const showIconControl = computed(() => [LAYOUT.RIGHT_SIDE_LIST, LAYOUT.TOP_SIDE_LIST].indexOf(layout.value) >= 0);

// --- 以下处理流事件 ----

const onUserEnterRoom = (streamInfo: any) => {
  streamStore.addRemoteUser(streamInfo);
};

const onUserLeaveRoom = (streamInfo: any) => {
  streamStore.removeRemoteUser(streamInfo);
  if (streamInfo.userId === enlargeStream.value?.userId || streamStore.remoteStreamMap.size === 0) {
    basicStore.setLayout(LAYOUT.NINE_EQUAL_POINTS);
    enlargeStream.value = null;
  }
};

const onUserVideoAvailable = (eventInfo: { userId: string, available: number, streamType: number }) => {
  const { userId, available, streamType } = eventInfo;
  if (userId === basicStore.userId) {
    streamStore.updateLocalStream({ isVideoStreamAvailable: available });
    return;
  }
  if (streamType === ETUIStreamType.CAMERA) {
    streamStore.updateRemoteVideoStream(eventInfo);
    // 处理 web 端屏幕分享
    if (userId.indexOf('share_') === 0 && userId !== `share_${basicStore.userId}`) {
      enlargeStream.value = streamStore.remoteStreamMap.get(`${userId}_main`) as StreamInfo;
      if (layout.value !== LAYOUT.RIGHT_SIDE_LIST && layout.value !== LAYOUT.TOP_SIDE_LIST) {
        basicStore.setLayout(LAYOUT.RIGHT_SIDE_LIST);
      }
    }
  } else if (streamType === ETUIStreamType.SCREEN) {
    streamStore.updateRemoteScreenStream(eventInfo);
    enlargeStream.value = streamStore.remoteStreamMap.get(`${userId}_screen`) as StreamInfo;
    if (layout.value !== LAYOUT.RIGHT_SIDE_LIST && layout.value !== LAYOUT.TOP_SIDE_LIST) {
      basicStore.setLayout(LAYOUT.RIGHT_SIDE_LIST);
    }
  }
};


const onUserAudioAvailable = (eventInfo: { userId: string, available: number}) => {
  const { userId, available } = eventInfo;
  if (userId === basicStore.userId) {
    streamStore.updateLocalStream({ isAudioStreamAvailable: available });
    return;
  }
  streamStore.updateRemoteAudioStream(eventInfo);
};

const { isDefaultOpenCamera, isDefaultOpenMicrophone } = storeToRefs(streamStore);

watch(isDefaultOpenCamera, async (val) => {
  if (val) {
    const previewDom = document.getElementById(`${streamStore.localStream.userId}_main`);
    if (previewDom) {
      // 设置设备id
      if (!streamStore.currentCameraId) {
        const cameraList = await TUIRoomCore.getCameraList();
        streamStore.setCurrentCameraId(cameraList[0].deviceId);
      }
      await TUIRoomCore.setCurrentCamera(streamStore.currentCameraId);
      // 设置视频参数
      const param = new TRTCVideoEncParam();
      param.videoResolution = TRTCVideoResolution.TRTCVideoResolution_1280_720;
      param.videoFps = 15;
      param.videoBitrate = 1500;
      param.resMode = TRTCVideoResolutionMode.TRTCVideoResolutionModeLandscape;
      param.minVideoBitrate = 0;
      param.enableAdjustRes = true;
      TUIRoomCore.setVideoEncoderParam(param);
      // 开启本地摄像头
      await TUIRoomCore.startCameraPreview(previewDom);
      streamStore.setHasStartedCamera(true);
    }
  }
});

watch(isDefaultOpenMicrophone, async (val) => {
  if (val) {
    const microphoneList = await TUIRoomCore.getMicrophoneList();
    if (!streamStore.currentMicrophoneId) {
      streamStore.setCurrentMicrophoneId(microphoneList[0].deviceId);
    }
    if (!streamStore.currentSpeakerId) {
      streamStore.setCurrentSpeakerId(microphoneList[0].deviceId);
    }
    await TUIRoomCore.setCurrentMicrophone(streamStore.currentMicrophoneId);
    await TUIRoomCore.startMicrophone();
    streamStore.setHasStartedMicrophone(true);
  }
});

onMounted(async () => {
  TUIRoomCore.on(ETUIRoomEvents.onUserEnterRoom, onUserEnterRoom);
  TUIRoomCore.on(ETUIRoomEvents.onUserLeaveRoom, onUserLeaveRoom);
  TUIRoomCore.on(ETUIRoomEvents.onUserVideoAvailable, onUserVideoAvailable);
  TUIRoomCore.on(ETUIRoomEvents.onUserAudioAvailable, onUserAudioAvailable);
});

onUnmounted(() => {
  TUIRoomCore.off(ETUIRoomEvents.onUserEnterRoom, onUserEnterRoom);
  TUIRoomCore.off(ETUIRoomEvents.onUserLeaveRoom, onUserLeaveRoom);
  TUIRoomCore.off(ETUIRoomEvents.onUserVideoAvailable, onUserVideoAvailable);
  TUIRoomCore.on(ETUIRoomEvents.onUserAudioAvailable, onUserAudioAvailable);
});
</script>

<style lang="scss" scoped>
@import '../../assets/style/var.scss';

.stream-container-flatten {
  width: 100%;
  height: 100%;
  background-color: $roomBackgroundColor;
  overflow: hidden;
  .stream-list-container {
    width: 100%;
    height: 100%
  }
  .stream-list {
    width: 100%;
    height: 100%;
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    align-items: center;
    align-content: center;
    .single-stream {
      padding: 4px;
    }
  }
}

.icon-control {
  background-color: $toolBarBackgroundColor;
  position: absolute;
  cursor: pointer;
  display: flex;
  justify-content: center;
  align-items: center;
}
.arrow-up {
  width: 52px;
  height: 20px;
  position: fixed;
  left: 50%;
  transform: translate(-50%);
  top: 175px;
  border-radius: 0 0 4px 4px;
}
.arrow-down {
  width: 52px;
  height: 20px;
  position: fixed;
  left: 50%;
  transform: translate(-50%);
  top: 48px;
  border-radius: 0 0 4px 4px;
  svg {
    transform: rotate(180deg);
  }
}
.arrow-right {
  width: 20px;
  height: 52px;
  position: fixed;
  right: 250px;
  transform: translate(-50%);
  top: calc((100% - 148px) / 2 + 58px);
  border-radius: 4px 0 0 4px;
  svg {
    transform: rotate(90deg);
  }
}
.arrow-left {
  width: 20px;
  height: 52px;
  position: fixed;
  right: -10px;
  transform: translate(-50%);
  top: calc((100% - 148px) / 2 + 58px);
  border-radius: 4px 0 0 4px;
  svg {
    transform: rotate(270deg);
  }
}

.stream-container-top {
  width: 100%;
  height: 100%;
  background-color: $roomBackgroundColor;
  overflow: hidden;
  position: relative;
  .enlarged-stream-container {
    width: 100%;
    height: calc(100% - 175px);
    position: absolute;
    top: 175px;
    display: flex;
    justify-content: center;
    align-items: center;
  }
  .stream-list-container {
    width: 100%;
    height: 175px;
    background-color: $toolBarBackgroundColor;
    position: absolute;
    top: 0;
    left: 0;
    padding: 20px 40px;
    display: flex;
    justify-content: center;
    &.hide-list {
      transform: translateY(-175px);
    }
    .stream-list {
      display: flex;
      overflow-x: scroll;
      &::-webkit-scrollbar {
        display: none;
      }
      .single-stream {
        width: 240px;
        height: 135px;
        border-radius: 4px;
        overflow: hidden;
        flex-shrink: 0;
        &:not(:first-child) {
          margin-left: 14px;
        }
      }
    }
  }
}

.stream-container-right {
  width: 100%;
  height: 100%;
  background-color: $roomBackgroundColor;
  overflow: hidden;
  display: flex;
  flex-wrap: nowrap;
  justify-content: space-between;
  align-content: center;
  .enlarged-stream-container {
    width: calc(100% - 260px);
    height: 100%;
    display: flex;
    justify-content: center;
    align-items: center;
  }
  .stream-list-container {
    width: 260px;
    height: 100%;
    position: absolute;
    top: 0;
    right: 0;
    padding: 48px 10px 80px;
    background-color: $toolBarBackgroundColor;
    display: flex;
    align-items: center;
    &.hide-list {
      transform: translateX(260px);
    }
    &::before {
      content: '';
      width: 100%;
      height: 40px;
      position: absolute;
      top: 48px;
      left: 0;
      opacity: 0.1;
    }
  }
  .stream-list {
    overflow-y: scroll;
    padding: 10px 0;
    &::-webkit-scrollbar {
      display: none;
    }
    .single-stream {
      width: 240px;
      height: 135px;
      border-radius: 4px;
      overflow: hidden;
      &:not(:first-child) {
        margin-top: 14px;
      }
    }
  }
}
</style>
