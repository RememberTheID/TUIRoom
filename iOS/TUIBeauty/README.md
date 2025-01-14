本文档主要介绍如何运行TUIBeauty组件的，以及如何在别的工程中集成TUIBeauty组件

## 目录结构

```
TUIBeauty
├─ Example              // 工程模块，主要提供 TUIBeauty 的测试页面
├─ Resources            // 美颜 Demo 中所用的资源文件
├─ Source               // TUIBeauty 组件的拉流逻辑封装
└─ TUIBeauty.podspec    // TUIBeauty 组件 pod 接入文件
```

## 环境准备
- iOS 13.0，Xcode 11.0 以上

## 运行示例

### 前提条件
您已 [注册腾讯云](https://cloud.tencent.com/document/product/378/17985) 账号，并完成 [实名认证](https://cloud.tencent.com/document/product/378/3629)。

### 申请 SDKAPPID 和 SECRETKEY
1. 登录实时音视频控制台，选择【开发辅助】>【[快速跑通Demo](https://console.cloud.tencent.com/trtc/quickstart)】。
2. 单击【立即开始】，输入您的应用名称，例如`TestTRTC`，单击【创建应用】。
<img src="https://main.qcloudimg.com/raw/169391f6711857dca6ed8cfce7b391bd.png" width="650" height="295"/>
3. 创建应用完成后，单击【我已下载，下一步】，可以查看 SDKAppID 和密钥信息。

### 开通移动直播服务
1. [开通直播服务并绑定域名](https://console.cloud.tencent.com/live/livestat) 如果还没开通，点击申请开通，之后在域名管理中配置推流域名和拉流域名
2. [获取SDK的测试License](https://console.cloud.tencent.com/live/license) 
3. [配置推拉流域名](https://console.cloud.tencent.com/live/domainmanage)

### 配置工程文件
1. 使用终端，cd 到 TUIBeauty/Example 路径，使用命令 "sh init.sh" 下载 SDK 及 Pod 更新
2. 使用 Xcode（11.0及以上的版本）打开源码工程`TUIBeautyApp.xcworkspace`。
3. 找到并打开`TUIBeauty/Debug/GenerateTestUserSig.swift`文件。
4. 设置`GenerateTestUserSig.swift`文件中的相关参数：
  - `SDKAPPID`：默认为 PLACEHOLDER ，请设置为实际的 SDKAppID；
  - `SECRETKEY`：默认为空字符串，请设置为实际的密钥信息；
  - `LICENSEURL`：默认为 PLACEHOLDER ，请设置为实际的License Url信息；
  - `LICENSEURLKEY`：默认为 PLACEHOLDER ，请设置为实际的License Key信息；
5. 修改 Bundle Id 为 License 信息所对应的包名


### 编译运行
用 Xcode 打开该项目，连上 iOS 设备，编译并运行。


## 集成TUIBeauty
如果需要将 TUIBeauty 组件集成到自己的项目中，可按如下步骤接入

### 工程配置
1. 将 `TUIBeauty/Resources`、`TUIBeauty/Source`、`TUIBeauty/TUIBeauty.podspec` 模块导入到自己的工程中
2. 在项目的 `Podfile` 文件中添加我们的 TUIBeauty 模块

```
pod 'TUIBeauty', :path => "../"
```
3. 进入`Podfile`文件所在目录，执行`pod install` 

### 功能使用

1. [XMagic License 授权](https://cloud.tencent.com/document/product/616/65878)

```
[[TUIBeautyView getBeautyService] setLicenseUrl:@"" key:@"" completion:^(NSInteger authResult, NSString * _Nonnull errorMsg) {
    if (authResult == 0) {
        NSLog(@"XMagic 授权成功");
    } else {
        NSLog(@"XMagic 授权失败");
    }
}];
```
2. 创建 `TUIBeautyView`

```
TUIBeautyView *mBeautyView = [[TUIBeautyView alloc] initWithBeautyManager:beautyManager];

```

3. 弹出美颜面板

```
[mBeautyView show];
```

#### 开启自定义美颜 - V2TXLivePusher
我们在自己的[小直播](git地址)工程中也使用了该TUIBeauty组件，可以参考。

1. 开启自定义美颜

```
V2TXLivePusher *pusher = [[V2TXLivePusher alloc] init];
[pusher enableCustomVideoProcess:YES pixelFormat:V2TXLivePixelFormatTexture2D bufferType:V2TXLiveBufferTypeTexture];
[pusher setObserver:self];
```

2. 实现自定义美颜回调

```
 - (void)onProcessVideoFrame:(V2TXLiveVideoFrame *)srcFrame dstFrame:(V2TXLiveVideoFrame *)dstFrame {
    dstFrame.textureId = [[TUIBeautyView getBeautyService] processVideoFrameWithTextureId:srcFrame.textureId textureWidth:srcFrame.width textureHeight:srcFrame.height];
}
```

####  开启自定义美颜 - TRTCCloud

1. 开启自定义美颜

```
[[TRTCCloud sharedInstance] setLocalVideoProcessDelegete:self pixelFormat:TRTCVideoPixelFormat_Texture_2D bufferType:TRTCVideoBufferType_Texture];
```

2. 实现自定义美颜回调

```
- (uint32_t)onProcessVideoFrame:(TRTCVideoFrame *)srcFrame dstFrame:(TRTCVideoFrame *)dstFrame {
    dstFrame.textureId = [[TUIBeautyView getBeautyService] processVideoFrameWithTextureId:srcFrame.textureId textureWidth:srcFrame.width textureHeight:srcFrame.height];
    return 0;
}
```

## 问题答疑
1、我们官网文档[常见问题](https://cloud.tencent.com/document/product/454/7998)中整理了一些常见的问题，如果遇到相同的问题，可以参考上面的解决方案
2、可以加入我们的 TUIKIT 答疑群，在群里我们有专人进行答疑
