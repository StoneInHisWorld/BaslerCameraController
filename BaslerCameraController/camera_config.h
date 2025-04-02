#pragma once
#include "declaration.h"
#include "opencvDeclaration.h"
#include <queue>

// 存储路径
static const string save_root = "C:\\Users\\hpdqd\\Documents\\Tencent Files\\1195290789\\FileRecv\\研究生期间研究\\Ultra_DIMPLE\\linear_fruits\\RdURL_QRCodes\\camdis12.0cm\\test\\";
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

class PylonCamera {
private:
	CBaslerUniversalInstantCamera* camera = nullptr;
    void setTriggerMode();

public:
	PylonCamera();
	~PylonCamera();
	// 连续拍照
    int grabContinuously(
        Pylon::CImageEventHandler* imgEventHdlPtr = nullptr,
        Pylon::CConfigurationEventHandler* configEventHdlPtr = nullptr,
        const uint32_t timeout = 5000
    );
};

// 设置事件处理器，负责进行相机打开时相机参数的设置
class CameraConfigHandler : public Pylon::CConfigurationEventHandler {
public:
    void OnOpened(Pylon::CInstantCamera& camera) override;
};

//void displayProgressBar(int current, int total);

#define FRAME_SUCCESSFUL 0
#define FRAME_SKIPPED 1
#define FRAME_ERROR 2

// 该实现能做到15943.5Hz帧率，DMD需要设置为16000Hz
#ifdef MUL_THREAD1
#include <algorithm>
#include <execution>

struct FrameStamp {
    std::shared_ptr<Pylon::CGrabResultPtr> grabResult;
    //const std::chrono::steady_clock::time_point& t;
    const int status;
};

// 多线程帧事件处理器
class FrameEventHandler : public Pylon::CImageEventHandler {
public:
    FrameEventHandler() {
        //persistenceThread = std::thread(&FrameEventHandler::saveImage, this);
        persistenceThread = std::thread(&FrameEventHandler::m_saveImage, this);
    }
    ~FrameEventHandler() {
        std::unique_lock<std::mutex> lock(queueMutex);
        // 提示线程任务已经完成
        this->stopThread = true;
        // 等待中途结果队列被消化完
        queueCondition.wait(lock, [this] {
            return ptrQueue.empty();
        });
        lock.unlock();

        queueCondition.notify_all();
        persistenceThread.join();
        cout << "所有图片已保存完毕！" << endl;
    }
    void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& ptrGrabResult) override;
    void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages) override;
    void clear();

private:
    std::thread persistenceThread;
    std::queue<FrameStamp> ptrQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    bool stopThread = false;
    void saveImage();
    void m_saveImage();
};
#endif // MUL_THREAD

// 该实现能做到15930+Hz帧率，DMD需要设置为16000Hz
#ifdef SIG_THREAD
struct FrameData {
    //const Pylon::CGrabResultPtr &ptr;
    //const CGrabResultData& data;
    int status;
    const cv::Mat image;
    string exception;
};

// 帧事件处理器
class FrameEventHandler : public Pylon::CImageEventHandler {
private:
    int frame_counter = 0;
    std::queue<FrameData> ptrQueue;

public:
    void OnImagesSkipped(Pylon::CInstantCamera& camera, size_t countOfSkippedImages) override;
    void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& ptrGrabResult) override;
    void clear();

};
#endif // SIG_THREAD

