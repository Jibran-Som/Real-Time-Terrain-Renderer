// TimeManager.h
#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class TimeManager {
private:
    float currentTime;           // 0 to 24 hours
    float cycleDuration;         // Real-time seconds for a full day cycle
    bool isCycling;
    float timeSpeedMultiplier;

public:
    TimeManager();

    void update(float deltaTime);

    // Getters
    float getTime() const { return currentTime; }
    float getNormalizedTime() const { return currentTime / 24.0f; }
    bool isPaused() const { return !isCycling; }
    float getTimeSpeed() const { return timeSpeedMultiplier; }

    // Setters
    void setTime(float time) { currentTime = glm::clamp(time, 0.0f, 24.0f); }
    void setCycleSpeed(float secondsPerFullCycle) {
        cycleDuration = glm::max(secondsPerFullCycle, 1.0f);
    }
    void setTimeSpeed(float speed) {
        timeSpeedMultiplier = glm::clamp(speed, 0.0f, 100.0f);
    }

    // Control methods
    void startCycle() { isCycling = true; }
    void stopCycle() { isCycling = false; }
    void togglePause() { isCycling = !isCycling; }
    void speedUp() { timeSpeedMultiplier = glm::min(timeSpeedMultiplier * 1.1f, 100.0f); }
    void slowDown() { timeSpeedMultiplier = glm::max(timeSpeedMultiplier * 0.9f, 0.1f); }

    // Get blend factor for day/night transitions
    float getDayNightBlend() const;
};
