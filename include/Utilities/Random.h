//
// Created by joost on 6/5/26.
//

#pragma once

#include <random>
#include "glm/glm.hpp"


namespace Spelt {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    /**
    * Generates random int inside [min, max]
    */
    inline int getRandomInt(int min, int max){
        std::uniform_int_distribution<int> distr(min, max);
        return distr(gen);
    }

    /**
    * Generates a random bool
    */
    inline bool getRandomBool(){
        static std::bernoulli_distribution distr(0.5);
        return distr(gen);
    }


    /**
    * Generates random float inside [min, max]
    */
    inline float getRandomFloat(float min, float max) {
        std::uniform_real_distribution<float> distr(min, max);
        return distr(gen);
    }

    /**
    * Generates random vec3 with each respective element inside [min, max]
    */
    inline glm::vec3 getRandomVec3(glm::vec3 min, glm::vec3 max){
        return glm::vec3{getRandomFloat(min.x, max.x), getRandomFloat(min.y, max.y), getRandomFloat(min.z, max.z)};
    }

    /**
    * Generates random float inside a normal distribution with, mean and std_dev as the standard deviation
    */
    inline float getRandomFloatNormal(float mean, float std_dev) {
        std::normal_distribution<float> dist(mean, std_dev);

        return dist(gen);
    }

    /**
    * Generates random float inside an inverse normal distribution with inside [min, max]
    */
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

    /**
    * Generates random float in an expentional distribution with a given lambda
    */
    inline float getRandomFloatExponential(float lambda) {
        std::exponential_distribution<float> dist(lambda);

        return dist(gen);
    }
}
