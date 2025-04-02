#include "declaration.h"

namespace fs = std::filesystem;

double calculateMean(const cv::Mat& image) {
    cv::Scalar mean = cv::mean(image);
    return (mean[0] + mean[1] + mean[2]) / 3.0;
}

// Function to split a string by a delimiter
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

int main() {
    std::string path = "../BaslerCameraController/fruits/";
    std::vector<std::pair<std::string, double>> imageMeans;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            cv::Mat image = cv::imread(entry.path().string());
            if (!image.empty()) {
                double mean = calculateMean(image);
                std::string filename = entry.path().string();
                filename = splitString(filename, '/').back();
                filename = *splitString(filename, '.').begin();
                imageMeans.emplace_back(filename, mean);
            }
        }
    }

    std::sort(imageMeans.begin(), imageMeans.end(), [](const auto& a, const auto& b) {
        return std::stoi(a.first) < std::stoi(b.first);
    });

    int seq_len = 1;
    std::vector<int> Seq_Lengths;
    for (size_t i = 1; i < imageMeans.size(); ++i) {
        if (imageMeans[i - 1].second > imageMeans[i].second) {
            seq_len++;
        } else {
            Seq_Lengths.push_back(seq_len);
            seq_len = 1;
        }
    }

    for (int len : Seq_Lengths) {
        std::cout << len << " ";
    }

    system("pause");
    return 0;
}
