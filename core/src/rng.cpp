#include "rng.hpp"

namespace axiom {

RandomNumberGenerator::RandomNumberGenerator() {
    std::random_device rd;
    engine_.seed(rd());
}

RandomNumberGenerator::RandomNumberGenerator(uint64_t seed) {
    engine_.seed(seed);
}

void RandomNumberGenerator::SetSeed(uint64_t seed) {
    engine_.seed(seed);
}

int64_t RandomNumberGenerator::NextInt(int64_t min, int64_t max) {
    std::uniform_int_distribution<int64_t> dist(min, max);
    return dist(engine_);
}

double RandomNumberGenerator::NextDouble() {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine_);
}

double RandomNumberGenerator::NextDouble(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(engine_);
}

} // namespace axiom
