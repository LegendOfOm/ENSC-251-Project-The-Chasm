#ifndef RNG_HPP
#define RNG_HPP

#include <random>
#include <chrono>
namespace rng {
    inline std::mt19937 randomNumber(std::chrono::steady_clock::now().time_since_epoch().count());
}
#endif // RNG_HPP