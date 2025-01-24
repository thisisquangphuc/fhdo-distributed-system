/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#ifndef MONITOR_H
#define MONITOR_H

#include <string>
#include <map>
#include <mutex>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>
#include "utils/env.h"

using json = nlohmann::json;
struct PlatoonData {
    double latitude;
    double longitude;
    double speed;
    double brakeForce;
    double acceleration;
    int gearPosition; // 0: N, 1: P, 2: D, 3: R
    double fuelLevel;
    time_t operationTime;
    std::string obstacleInfo;
    std::string trafficSignal;
};

struct EmergencyEvent {
    std::string truckID;
    time_t timestamp;
    std::string emergencyType;
    double latitude;
    double longitude;
    double speed;
    std::string obstacleInfo;
    int priority; // Higher value = higher priority
    int verificationCount; // Number of times the condition is verified
    bool confirmed;        // True if the emergency is confirmed
};

class PlatoonDataManager {
    private:
        PlatoonData platoonData; // TruckID -> TruckData
        std::mutex dataMutex; // Mutex to ensure thread-safety

        PlatoonDataManager() = default;
    public:
        PlatoonDataManager(const PlatoonDataManager&) = delete;
        PlatoonDataManager& operator=(const PlatoonDataManager&) = delete;

        static PlatoonDataManager& getInstance() {
            static PlatoonDataManager instance;
            return instance;
        }
        
        void init();
        // Add or update a truck's data
        void updateTruckData(const PlatoonData& data);

        // Get data for a specific truck
        PlatoonData getPlatoonData() const;

        nlohmann::json getPlatoonDataJSON();

        // Update a specific field in a truck's data
        template <typename T>
        void updateTruckField(T PlatoonData::*field, const T& value) {
            std::lock_guard<std::mutex> lock(dataMutex);
            (platoonData.*field) = value;
        }
};

#endif // MONITOR_H