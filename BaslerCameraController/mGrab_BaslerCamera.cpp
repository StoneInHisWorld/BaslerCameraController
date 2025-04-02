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
        FrameEventHandler* frameEventHandler = new FrameEventHandler();
        //camera.setTriggerMode();
        camera.grabContinuously(frameEventHandler, new CameraConfigHandler());
        //frameEventHandler->clear();
        cout << endl << "开始保存图片……" << endl;
        auto grabbing_start = std::chrono::high_resolution_clock::now();
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
