<template>
  <div class="phone-login">
    <input class="input" :placeholder="placeholder" auto-complete="true" v-model="phoneNumber">
    <div class="area-container">
      <svg-icon icon-name="phone-icon" size="medium"></svg-icon>
    </div>
  </div>
</template>

<script setup lang="ts">
import SvgIcon from '../common/SvgIcon.vue';
import VerifyCode from './VerifyCode.vue';
import { ref,watch} from 'vue';
const emit = defineEmits(["updatePhoneNumber"]);
const placeholder = "请输入您的手机号";
const phoneNumber = ref('');
watch(() => phoneNumber.value, (val) => {
  phoneNumber.value = val.replace(/\D/g, '');
  emit("updatePhoneNumber",phoneNumber.value);
});
</script>

<style lang="scss" scoped>
.phone-login{
  position:relative;
  width:100%;
  height:100%;
  .input{
    -webkit-appearance:none;
    background-color:#292D38;
    background-image:none;
    border-radius:8px;
    border:1px solid #292D38;
    box-sizing:border-box;
    color: #B3B8C8;
    display:inline-block;
    font-size:inherit;
    height:60px;
    line-height:60px;
    outline:none;
    padding:0 15px 0 40px;
    transition:border-color .2s cubic-bezier(.645,.045,.355,1);
    width:100%;
  }
  .area-container{
    width:20px;
    height:20px;
    // border: 1px solid pink;
    position:absolute;
    top:32%;
    left:4%;
  }
  .verify-code{
    margin-top:30px;
  }
}
</style>
