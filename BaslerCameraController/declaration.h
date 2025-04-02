#pragma once
#include <iostream>
// Include files to use the pylon API.
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

// Namespace for using pylon objects.
using namespace Pylon;

// Namespace for using cout.
using namespace std;

// Namespace for using pylon universal instant camera parameters.
using namespace Basler_UniversalCameraParams;

#define mGrab_BCamera
// 设备如果不支持多线程，可以注释掉下面的宏定义以启用单线程
// 经测试，使用单线程时，性能不会有多大影响
//#define SIG_THREAD
#define MUL_THREAD1

#ifdef MUL_THREAD1
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#endif // MUL_THREAD



