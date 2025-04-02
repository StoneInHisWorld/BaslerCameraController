#include "camera_config.h"

#ifdef mGrab_BCamera

void input_parameters()
{
    int input;
    cout << "输入要拍照的张数：";
    cin >> input;
    if (input != '\n') n_ImagesToGrab = input;
}

int main(int /*argc*/, char* /*argv*/[])
{
    // 退出代号
    int exitCode = 0;

    // 在使用任何pylon函数前，需要初始化pylon运行环境
    PylonInitialize();
    //input_parameters();

    try
    {
        PylonCamera camera;
        // 相机事件注册
        // 该相机事件处理器通过`OnImageGrabbed()`定义了相机拿到帧后执行的动作
        // 该相机事件处理器通过`OnImagesSkipped()`定义了相机发现帧被跳过后执行的动作
        FrameEventHandler* frameEventHandler = new FrameEventHandler();
        // 进行连续拍照
        // `CameraConfigHandler`是一个事件处理器，负责进行相机打开时相机参数的设置
        camera.grabContinuously(frameEventHandler, new CameraConfigHandler());
        cout << endl << "开始保存图片……" << endl;
        auto grabbing_start = std::chrono::high_resolution_clock::now();
        // 删除事件处理器时，事件处理器才会将所有的图片保存
        delete frameEventHandler;
        auto grabbing_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = grabbing_end - grabbing_start;
        std::cout << "保存图片花费了" << duration.count() << "秒" << endl;
    }
    catch (const string& errmsg) {
        cerr << errmsg << endl;
        exitCode = 1;
    }
    catch (const Pylon::GenericException& e)
    {
        cerr << "调用GenICam接口时发生了异常：" << e.what() << endl;
        exitCode = 2;
    }
    catch (const std::exception& e)
    {
        cerr << "发生了未知异常：" << e.what() << endl;
        exitCode = 3;
    }
    if (exitCode != 0) system("pause");
    return exitCode;
}
#endif // mGrab_BCamera
