<template>
  <div class="setting-control-container">
    <icon-button
      title="设置"
      :icon-name="iconName"
      @click="handleShowSettingDialog"
    />
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import IconButton from '../common/IconButton.vue';
import { useBasicStore } from '../../stores/basic';
import { storeToRefs } from 'pinia';

const basicStore = useBasicStore();
const { showSettingDialog } = storeToRefs(basicStore);

const iconName = computed(() => (showSettingDialog.value ? 'setting-active' : 'setting'));

function handleShowSettingDialog() {
  basicStore.setShowSettingDialog(!basicStore.showSettingDialog);
}
</script>

<style lang="scss" scoped>

@import '../../assets/style/var.scss';

.setting-dialog {
  width: 660px;
  height: 574px;
  position: absolute;
  display: flex;
  .dialog-tabs {
    width: 194px;
    height: 574px;
    background-color: $toolBarBackgroundColor;
  }
  .dialog-content {
    flex-grow: 1;
  }
}
</style>
