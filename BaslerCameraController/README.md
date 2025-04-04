# BaslerCameraController

该项目提供了一个简单的接口，用于控制Basler相机。用户可以配置相机参数并使用pylon API进行图像采集。

## 需要了解的是

请确保您已认真阅读项目的[配置说明](../README.md)
- **`main.cpp`**：应用程序的主入口。此文件包含初始化相机、创建相机事件处理器、通知相机开始拍照的逻辑。
- **`camera_config.h`**：此文件首部包含相机的可配置参数，例如分辨率、帧率和曝光设置。
- **`declaration.h`**：整个项目的依赖以及编译选项

## 使用方法

1. 配置相机参数  
    打开 `camera_config.h` 文件，根据需要查找变量名以修改对应相机参数，目前支持的相机参数包括:
    ```cpp
    // 存储路径
    static const string save_root = "where you save grabbed images";
    static const string filename_perfix = ".png";
    static int start_frame_index = 0;

    // 拍照参数
    // 单次拍照的张数
    static uint32_t n_ImagesToGrab = 20000;
    static float cam_exposure = 90;
    static float cam_gain = 256;
    static const float cam_width = 2048;
    static const float cam_height = 1;
    static const uint32_t buffer_per_frame = 500;

    // 保存参数
    static const size_t n_threads = 10;
    ```

2. 编译并运行项目即可  

## 注意事项
- 可以自定义`FrameEventHandler`和`CameraConfigHandler`来指定相机拍摄照片后以及相机被打开时执行的操作 
- 本项目只支持相机电信号触发的拍照的情形，有关触发配置的更改，请参见`PylonCamera::setTriggerMode()`。触发电路信号的连接，请参照相机说明书。
- 相机其他调用模式，请参考[官方示例和编程文档](https://docs.baslerweb.com/pylonapi/cpp/)
