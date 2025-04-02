#pragma once

#include <iostream>
// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using cout.
using namespace std;

//#define TEST
//#define CV_TEST
//#define GRAB
#define PC
//#define PC_AutoFuctions
//#define PC_Grab
//#define PC_Configuration

//#define mPC_Grab
#define mGrab_BCamera
//#define SIG_THREAD
//#define MUL_THREAD
#define MUL_THREAD1

//void displayProgressBar(int current, int total);

#ifdef MUL_THREAD1
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

//class ThreadPool {
//public:
//    ThreadPool(size_t numThreads);
//    ~ThreadPool();
//    void enqueue(std::function<void()> task);
//
//private:
//    std::vector<std::thread> workers;
//    std::queue<std::function<void()>> tasks;
//    std::mutex queueMutex;
//    std::condition_variable condition;
//    bool stop;
//};
//
//ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
//    for (size_t i = 0; i < numThreads; ++i) {
//        workers.emplace_back([this]() {
//            while (true) {
//                std::function<void()> task;
//                {
//                    std::unique_lock<std::mutex> lock(this->queueMutex);
//                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
//                    if (this->stop && this->tasks.empty()) return;
//                    task = std::move(this->tasks.front());
//                    this->tasks.pop();
//                }
//                task();
//            }
//        });
//    }
//}
//
//ThreadPool::~ThreadPool() {
//    {
//        std::unique_lock<std::mutex> lock(queueMutex);
//        stop = true;
//    }
//    condition.notify_all();
//    for (std::thread& worker : workers) {
//        worker.join();
//    }
//}
//
//void ThreadPool::enqueue(std::function<void()> task) {
//    {
//        std::unique_lock<std::mutex> lock(queueMutex);
//        tasks.emplace(std::move(task));
//    }
//    condition.notify_one();
//}

//// 定义模板接口
//template <typename Func, typename Arg>
//void execute_function(Func func, Arg arg) {
//    func(arg); // 调用用户函数并传递参数
//}

#endif // MUL_THREAD


#ifdef PC
#include <pylon/BaslerUniversalInstantCamera.h>
// Namespace for using pylon universal instant camera parameters.
using namespace Basler_UniversalCameraParams;
#endif // PC



