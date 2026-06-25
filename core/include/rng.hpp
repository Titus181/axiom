#pragma once
#include <random>
#include <cstdint>
#include <vector>
#include <algorithm>

namespace axiom {

class RandomNumberGenerator {
public:
    // 預設構造函數，使用隨機裝置初始化
    RandomNumberGenerator();
    
    // 使用指定種子初始化
    explicit RandomNumberGenerator(uint64_t seed);

    // 設定種子
    void SetSeed(uint64_t seed);

    // 產生 [min, max] 區間內的隨機整數 (包含 min 和 max)
    int64_t NextInt(int64_t min, int64_t max);

    // 產生 [0.0, 1.0) 區間內的隨機雙精度浮點數
    double NextDouble();

    // 產生 [min, max) 區間內的隨機雙精度浮點數
    double NextDouble(double min, double max);

    // 隨機打亂一個向量
    template<typename T>
    void Shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), engine_);
    }

private:
    std::mt19937_64 engine_;
};

} // namespace axiom
