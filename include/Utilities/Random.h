//
// Created by joost on 6/5/26.
//

#ifndef CGVCPROJECT_RANDOM_H
#define CGVCPROJECT_RANDOM_H
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

inline float getRandomFloat(float min, float max) {
    std::uniform_real_distribution<float> distr(min, max);
    return distr(gen);
}

inline float getRandomFloatNormal(float mean, float std_dev) {
    std::normal_distribution<float> dist(mean, std_dev);

    return dist(gen);
}

inline float getRandomFloatNormalInverse(float min, float max) {
    static std::normal_distribution<float> dist(0.0f, 1.0f);

    float val = dist(gen);

    // Use the Cumulative Distribution Function (CDF) approximation via std::erf
    // This maps the normal distribution evenly between 0.0 and 1.0
    float uniform_mapped = 0.5f * (1.0f + std::erf(val / std::sqrt(2.0f)));

    // Push the values to the edges (Create the U-Shape)
    // If it's close to 0.5 (the middle), we push it toward 0 or 1
    float u_shaped = (uniform_mapped < 0.5f)
        ? std::pow(uniform_mapped * 2.0f, 0.3f) * 0.5f
        : 1.0f - std::pow((1.0f - uniform_mapped) * 2.0f, 0.3f) * 0.5f;

    // Scale to the desired min/max range
    return min + u_shaped * (max - min);
}

inline float getRandomFloatExponential(float lambda) {
    std::exponential_distribution<float> dist(lambda);

    return dist(gen);
}

#endif //CGVCPROJECT_RANDOM_H