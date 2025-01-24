/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */
#include <unistd.h>
#include "monitor.h"
#include "communication/comm_msg.h"

// void send_braking_signal();
// double get_leading_pos();
// double get_following_pos();

double calculate_distance() {
    double dummy(5.5);
    return dummy;
}

bool is_braking_needed(double dis) {
    return false;
}

bool is_connection_lost() {
    // Check connection timeout or connection lost
    //return false 
    return false;
}

void PlatoonDataManager::init() {
    try
    {
        platoonData.latitude = std::stod(env_get("LEADING_LAT",0)); 
        platoonData.longitude = std::stod(env_get("LEADING_LON",0));
        platoonData.speed = std::stod(env_get("LEADING_SPEED",0));
        platoonData.brakeForce = std::stod(env_get("LEADING_BRAKE_FORCE",0));
        platoonData.acceleration = std::stod(env_get("LEADING_ACCELERATION",0));
        platoonData.gearPosition = std::stoi(env_get("LEADING_GEAR",0));
        platoonData.fuelLevel = std::stod(env_get("LEADING_FUEL",0));
        platoonData.operationTime = TruckMessage::generateTimestamp();
        // platoonData.obstacleInfo = env_get("LEADING_OBSTACLE",0);
        // platoonData.trafficSignal = env_get("LEADING_SIGNAL",0);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error reading environment variable: " << e.what() << '\n';
    }
    
}

nlohmann::json PlatoonDataManager::getPlatoonDataJSON() {
    std::lock_guard<std::mutex> lock(dataMutex);

    // Format the status based on speed and brakeForce
    std::string status = "normal";
    if (platoonData.speed == 0 && platoonData.brakeForce >= 1) {
        status = "stopped";
    } else if (platoonData.brakeForce > 0.8) {
        status = "emergency";
    } else if (platoonData.speed > 100) {
        status = "error";
    }

    // Convert the data to JSON
    nlohmann::json jsonData = {
        {"contents", {
            {"location", {
                {"lat", platoonData.latitude},
                {"lon", platoonData.longitude}
            }},
            {"speed", platoonData.speed},
            {"status", status},
            {"brake_force", platoonData.brakeForce},
            {"error_code", "0000"}
        }}
    };

    return jsonData;
}

PlatoonData PlatoonDataManager::getPlatoonData() const {
    return platoonData;
}