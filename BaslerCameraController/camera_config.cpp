#include "camera_config.h"

void displayProgressBar(int current, int total)
{
    int barWidth = 50;
    float progress = (float)current / total;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

template <typename T, typename Func>
void map_async(const std::vector<T>& vec, Func process_func, const size_t num_threads = std::thread::hardware_concurrency())
{
    cout << "调用" << num_threads << "个线程" << endl;
    // 线程数
    size_t chunk_size = vec.size() / num_threads;

    std::vector<std::thread> threads;

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == num_threads - 1) ? vec.size() : start + chunk_size;

        std::vector<T> chunk(vec.begin() + start, vec.begin() + end);

        threads.emplace_back(process_func, chunk);
    }

    // 等待所有线程完成
    for (size_t i = 0; i < num_threads; i++)
    {
        threads.at(i).join();
        displayProgressBar(i + 1, num_threads);
    }
    //for (auto& t : threads) 
    //{
    //    t.join();

    //}
}

PylonCamera::PylonCamera()
{
    // 退出代号
    int exitCode = 0;
    // 在使用任何pylon函数前，需要初始化pylon运行环境
    PylonInitialize();
    try
    {
        // 为找到的第一个相机设备创建一个InstantCamera对象
        this->camera = new CBaslerUniversalInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
        // 输出相机的型号
        cout << "使用设备" << this->camera->GetDeviceInfo().GetModelName() << endl << endl;

    }
    catch (const GenericException& e)
    {
        // 处理异常
        cerr << "运行遇到了异常：" << e.GetDescription() << endl;
        cerr << "可能办法：" << endl << "检查相机的以太网连接有无设置动态IP地址，请设置为动态IP地址" << endl;
        throw "相机启动失败，运行中止！";
    }
}

PylonCamera::~PylonCamera()
{
    // 释放所有Pylon资源
    PylonTerminate();
}

void PylonCamera::setTriggerMode()
{
	// 设定触发模式为硬件触发（这个成员只有CBaslerUniversalInstantCamera有）
    this->camera->TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
    this->camera->TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
    this->camera->TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_Line2);
	//// 选择“曝光启用信号”为line 1输出（默认情况下已启用该设置）
	//camera.LineSelector.SetValue(LineSelector_Out1);
	//camera.LineSource.SetValue(LineSource_ExposureActive);
	// 选择“曝光启用信号”为line 2输出
    this->camera->LineSelector.SetValue(LineSelector_Out2);
    this->camera->LineSource.SetValue(LineSource_ExposureActive);
}

int PylonCamera::grabContinuously(
    Pylon::CImageEventHandler* imgEventHdlPtr, 
    Pylon::CConfigurationEventHandler* configEventHdlPtr,
    const uint32_t timeout)  
{
    int exitCode = 0;
    try {
        // 注册标准配置事件处理器来设置相机进行连续获取
        // 设置注册模式为`RegistrationMode_ReplaceAll`时，新的设置处理器会替代默认的设置处理器
        // 默认的设置处理器在创建`InstantCamera`对象的时候注册到该对象中
        // 设置`Cleanup_Delete`时，如果处理器被解绑或者注册信息被清除，处理器会被自动释放
        this->camera->RegisterConfiguration(new CAcquireContinuousConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
        // 相机事件注册
        if (imgEventHdlPtr) {
            this->camera->RegisterImageEventHandler(imgEventHdlPtr, RegistrationMode_ReplaceAll, Cleanup_None);
        }
        if (configEventHdlPtr) {
            this->camera->RegisterConfiguration(configEventHdlPtr, RegistrationMode_Append, Cleanup_None);
        }
        // Open()函数会调用配置处理器的OnOpened()方法，这个方法会应用所需的参数修改
        this->camera->Open();
        // 设置相机触发
        this->setTriggerMode();
        // 关闭测试图像
        // （CBaslerUniversalInstantCamera对象在拍照前需要进行此操作）
        this->camera->TestImageSelector.TrySetValue(TestImageSelector_Off);
        this->camera->TestPattern.TrySetValue(TestPattern_Off);

        // 开始抓取c_countOfImagesToGrab张图片
        this->camera->StartGrabbing(n_ImagesToGrab);
        // 智能指针能够抓取结果数据。
        CBaslerUniversalGrabResultPtr ptrGrabResult;
        size_t n_grabbed_frame = 0;
        // 开始计时
        auto grabbing_start = std::chrono::high_resolution_clock::now();
        try
        {
            while (this->camera->IsGrabbing())
            {
                if (n_grabbed_frame == 0) { cout << "相机已准备好拍摄" << endl; }
                // 等待一张图片并获取，第一个参数为等待时限，单位ms
                this->camera->RetrieveResult(timeout, ptrGrabResult, TimeoutHandling_ThrowException);
                n_grabbed_frame++;
            }
        }
        catch (const TimeoutException& e)
        {
            cout << "获取帧超时！" << endl;
            exitCode = 1;
        }
        auto grabbing_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = grabbing_end - grabbing_start;
        double frame_rate = 0;
        if (n_grabbed_frame != 0)
        {
            frame_rate = 1.0 / (duration.count() / n_grabbed_frame);
        }
        // 打印花费时间
        std::cout << "拍摄" << n_grabbed_frame << "帧花费" << duration.count() << "秒，";
        std::cout << "帧率为 " << frame_rate << "Hz" << endl;
        // 关闭相机
        this->camera->Close();
        this->camera->DeregisterImageEventHandler(imgEventHdlPtr);
        this->camera->DeregisterConfiguration(configEventHdlPtr);
    }
    catch (const std::exception& e)
    {
        cerr << "An Unknown exception occurred." << endl
            << e.what() << endl;
        exitCode = 2;
    }
    return exitCode;
}

// 相机打开事件处理函数，进行相机各种参数的设置
void CameraConfigHandler::OnOpened(Pylon::CInstantCamera& camera)
{
    try
    {
        // 使得GenApi命名空间下的所有名字都可以直接使用
        using namespace Pylon;

        // 获取相机控制对象
        GenApi::INodeMap& nodemap = camera.GetNodeMap();

        // 获取设置图像AOI的参数
        CIntegerParameter width(nodemap, "Width");
        CIntegerParameter height(nodemap, "Height");
        CIntegerParameter offsetX(nodemap, "OffsetX");
        CIntegerParameter offsetY(nodemap, "OffsetY");

        // 最大化图片AOI
        offsetX.TrySetToMinimum(); // Set to minimum if writable.
        offsetY.TrySetToMinimum(); // Set to minimum if writable.
        width.SetValue(cam_width);
        height.SetValue(cam_height);
        //width.SetToMaximum();
        //height.SetToMinimum();
        cout << "相机的AOI更改为：" << "高" << height.GetValue() << "宽" << width.GetValue() << endl;

        // 设置像素格式为Mono8
        // 注：这会导致CImagePersistence::Save()函数无法使用
        CEnumParameter(nodemap, "PixelFormat").SetValue("Mono8");
        cout << "像素格式设置为：Mono8" << endl;

        // 设置拍照参数
        CFloatParameter expTime(nodemap, "ExposureTimeAbs");
        CIntegerParameter gain(nodemap, "GainRaw");
        expTime.SetValue(cam_exposure);
        gain.SetValue(cam_gain);
        cout << "相机的曝光时间更改为：" << expTime.GetValue() << endl;
        cout << "相机的增益更改为：" << gain.GetValue() << endl;

        // 控制单帧缓冲区的数量，默认值为10
        // 这个参数可以影响帧获取的成功与否
        camera.MaxNumBuffer = buffer_per_frame;

        cout << endl;
    }
    catch (const Pylon::GenericException& e)
    {
		throw RUNTIME_EXCEPTION("无法应用配置！在OnOpened方法中捕获到GenericException异常 msg=%hs", e.what());
    }
}

#ifdef SIG_THREAD
// 图片跳过事件处理函数
void FrameEventHandler::OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
{
    this->ptrQueue.push({ FRAME_SKIPPED, Mat{}, "skipped"});
    this->frame_counter++;
}

// 图片获取事件处理函数
void FrameEventHandler::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& ptrGrabResult)
{
    //Pylon::CGrabResultPtr ptr(ptrGrabResult);
	// 成功获取图像
	if (ptrGrabResult->GrabSucceeded())
	{
        //std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
        //std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;

        // string filename = save_root + to_string(frame_counter++) + filename_perfix;
        // saveImage(filename, ptrGrabResult->GetBuffer(), ptrGrabResult->GetWidth(), ptrGrabResult->GetHeight(), CV_8UC1);

		//uint32_t width = ptrGrabResult->GetWidth();
		//uint32_t height = ptrGrabResult->GetHeight();

        // The pylon grab result smart pointer classes provide a cast operator to the IImage
        // interface. This makes it possible to pass a grab result directly to the
        // function that saves an image to disk.
        //string filename = "./fruits/" + to_string(frame_counter++) + ".png";
        //CImagePersistence::Save(ImageFileFormat_Png, filename.c_str(), ptrGrabResult);
        
		cv::Mat image = Mat{ (int)ptrGrabResult->GetHeight(), (int)ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t *)ptrGrabResult->GetBuffer() };
        this->ptrQueue.push({ FRAME_SUCCESSFUL, image.clone(), ""});
	}
	else
	{
		string exception_desc = "Error: " + std::to_string(ptrGrabResult->GetErrorCode()) + " " + String(ptrGrabResult->GetErrorDescription());
        this->ptrQueue.push({ FRAME_ERROR, Mat{}, exception_desc });
		//cerr << exception_desc << endl;
	}
}

void FrameEventHandler::clear() {
    // 保存所有图片
    int frame_counter = 0;
    int total = ptrQueue.size();
    while (!ptrQueue.empty()) {
        auto data = ptrQueue.front();
        int status = data.status;
        if (status == FRAME_SUCCESSFUL) {
            cv::Mat im = data.image;
            if (!im.empty()) {
                try
                {
                    std::string filename = save_root + std::to_string(frame_counter++) + filename_perfix;
                    cv_func::saveImage(filename, im);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "保存图片时发生异常：" << e.what() << std::endl;
                    return;
                }
            }
            else
            {
                std::cerr << "拍摄帧" << frame_counter++ << "为空！" << endl;
            }
        }
        else if (status == FRAME_SKIPPED)
        {
            std::cerr << "帧" << frame_counter++ << "被跳过！" << endl;
        }
        else
        {
            std::cerr << "拍摄帧" << frame_counter++ << "失败！遇到异常：";
            std::cerr << data.exception << std::endl;
        }
        ptrQueue.pop();
        displayProgressBar(frame_counter, total);
    }
}
#endif // SIG_THREAD

#ifdef MUL_THREAD1
// Sample configuration of event handler.
void FrameEventHandler::OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    ptrQueue.push({ std::make_shared<Pylon::CGrabResultPtr>(), FRAME_SKIPPED});
    queueCondition.notify_one();
}

void FrameEventHandler::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& ptrGrabResult)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    auto grabResultCopy = std::make_shared<Pylon::CGrabResultPtr>(ptrGrabResult);
    // 成功获取图像
    if (ptrGrabResult->GrabSucceeded())
    {
        ptrQueue.push({ grabResultCopy, FRAME_SUCCESSFUL });
        queueCondition.notify_one();
    }
    else
    {
        string exception_desc = "Error: " + std::to_string(ptrGrabResult->GetErrorCode()) + " " + String(ptrGrabResult->GetErrorDescription());
        //ptrQueue.push({  nullptr, 0, 0, exception_desc });
        ptrQueue.push({ grabResultCopy, FRAME_ERROR });
        queueCondition.notify_one();
    }
}

void FrameEventHandler::saveImage()
{
    queue<pair<string, cv::Mat>> results;
    while (true) {
        try {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] {
                return !ptrQueue.empty() || stopThread;
            });

            if (stopThread && ptrQueue.empty()) {
                break;
            }

            FrameStamp res = ptrQueue.front();
            ptrQueue.pop();
            lock.unlock();

            auto grabResult = *res.grabResult.get();
            // 在拍摄帧的过程中不能输出信息到控制台，否则会严重拖累拍照帧率
            if (res.status == FRAME_SUCCESSFUL) {
                // 经实验验证，opencv比CImagePersitence类保存图片速率要快
                cv::Mat image(grabResult->GetHeight(), grabResult->GetWidth(), CV_8UC1, grabResult->GetBuffer());
                // 使用clone以保证缓冲区的数据不被销毁
                results.push({ "successful", image.clone()});
            }
            else if (res.status == FRAME_SKIPPED) {
                // 图片被跳过
                results.push({ "skipped", Mat{} });
            }
            else {
                // 图片拍摄出错
                results.push({ String(grabResult->GetErrorDescription()), Mat{} });
            }

            queueCondition.notify_one();
        }
        catch (const std::exception& e) {
            std::cerr << "保存图片时发生异常：" << e.what() << std::endl;
            return;
        }
    }
    // 保存所有图片
    int frame_counter = start_frame_index;
    int total = results.size() + frame_counter;
    while (!results.empty()) {
        auto data = results.front();
        if (data.first == "successful") {
            if (!data.second.empty()) {
                try
                {
                    std::string filename = save_root + std::to_string(frame_counter++) + filename_perfix;
                    cv_func::saveImage(filename, data.second);
                }
                catch (const std::exception& e)
                {
                    std::cerr << "保存图片时发生异常：" << e.what() << std::endl;
                    return;
                }
            }
            else
            {
                std::cerr << "拍摄帧" << frame_counter++ << "为空！" << endl;
            }
        }
        else if (data.first == "skipped")
        {
            std::cerr << "帧" << frame_counter++ << "被跳过！" << endl;
        }
        else
        {
            std::cerr << "拍摄帧" << frame_counter++ << "失败！遇到异常：";
            std::cerr << data.first << std::endl;
        }
        results.pop();
        displayProgressBar(frame_counter - start_frame_index, total - start_frame_index);
    }
}

void FrameEventHandler::m_saveImage()
{
    queue<pair<string, cv::Mat>> results;
    while (true) {
        try {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] {
                return !ptrQueue.empty() || stopThread;
            });

            if (stopThread && ptrQueue.empty()) {
                break;
            }

            FrameStamp res = ptrQueue.front();
            ptrQueue.pop();
            lock.unlock();

            auto grabResult = *res.grabResult.get();
            // 在拍摄帧的过程中不能输出信息到控制台，否则会严重拖累拍照帧率
            if (res.status == FRAME_SUCCESSFUL) {
                // 经实验验证，opencv比CImagePersitence类保存图片速率要快
                cv::Mat image(grabResult->GetHeight(), grabResult->GetWidth(), CV_8UC1, grabResult->GetBuffer());
                // 使用clone以保证缓冲区的数据不被销毁
                results.push({ "successful", image.clone() });
            }
            else if (res.status == FRAME_SKIPPED) {
                // 图片被跳过
                results.push({ "skipped", Mat{} });
            }
            else {
                // 图片拍摄出错
                results.push({ String(grabResult->GetErrorDescription()), Mat{} });
            }

            queueCondition.notify_one();
        }
        catch (const std::exception& e) {
            std::cerr << "保存图片时发生异常：" << e.what() << std::endl;
            return;
        }
    }

    // 对保存列表进行进一步筛选
    int frame_counter = start_frame_index;
    vector<pair<string, cv::Mat>> rets;
    while (!results.empty())
    {
        auto res = results.front();
        results.pop();

        string status = res.first;
        cv::Mat image = res.second;
        // 成功采集到的帧，保存为图片
        if (status == "successful") {
            if (!image.empty()) {
                try {
                    std::string filename = save_root + std::to_string(frame_counter++) + filename_perfix;
                    rets.push_back({ filename, image });
                }
                catch (const std::exception& e) {
                    std::cerr << "保存帧" << frame_counter++ << "时发生异常："
                        << e.what() << std::endl;
                    return;
                }
            }
            else {
                std::cerr << "拍摄帧" << frame_counter++ << "为空！" << endl;
            }
        }
        // 如果帧被跳过
        else if (status == "skipped") {
            std::cerr << "帧" << frame_counter++ << "被跳过！" << endl;
        }
        // 帧出错
        else {
            std::cerr << "拍摄帧" << frame_counter++ << "失败！遇到异常：";
            std::cerr << status << std::endl;
        }
    }

    // 图片保存的部分
    std::mutex p_mutex;
    size_t progress = 0;
    size_t total = rets.size();
    //auto save_impl = [&p_mutex, &progress, &total](const vector<pair<string, cv::Mat>>& data) {
    //    for (auto& d : data) 
    //    {
    //        cv_func::saveImage(d.first, d.second);
    //        std::lock_guard<std::mutex> lock(p_mutex);
    //        displayProgressBar(++progress, total);
    //    }
    //};
    auto save_impl = [](const vector<pair<string, cv::Mat>>& data) {
        for (auto& d : data) cv_func::saveImage(d.first, d.second);
    };
    map_async(rets, save_impl, n_threads);
}

void FrameEventHandler::clear()
{
        std::unique_lock<std::mutex> lock(queueMutex);
        this->stopThread = true;
        //std::condition_variable stopCondition;
        queueCondition.wait(lock, [this] {
            return ptrQueue.empty();
        });
        // Calculate duration and frame rate
        //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(this->last_frame_ts - this->first_frame_ts).count();
        //double frame_rate = 1 / (duration / static_cast<double>(n_ImagesToGrab));

        //cout << "拍摄" << n_ImagesToGrab << "张图片用时" << duration << "秒" << endl;
        //cout << "帧率为" << frame_rate << "Hz" << endl;
        lock.unlock();
        //ptrQueue.push({ Mat{}, "所有图片已保存完毕！" });

        queueCondition.notify_all();
        persistenceThread.join();
        cout << "所有图片已保存完毕！" << endl;
}
#endif  // MUL_THREAD
