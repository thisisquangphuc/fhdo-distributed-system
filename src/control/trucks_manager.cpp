/*
 * Author : Phuc Le
 *
 * Created on Mon Jan 06 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#include "trucks_manager.h"
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

// Add a truck to the platoon
void TruckManager::addTruck(const std::string& truck_id, int socket) {
    trucks[++currentOrder] = {truck_id, socket};
    printPlatoon();
}

// Remove a truck by ID and update orders
void TruckManager::removeTruck(const std::string& truck_id) {
    int removeOrder = -1;
    for (const auto& [order, data] : trucks) {
        if (data.first == truck_id) {
            removeOrder = order;
            break;
        }
    }
    if (removeOrder == -1) {
        // throw std::invalid_argument("Truck ID not found.");
        spdlog::warn("Truck ID not found or Truck is already removed/disconnected.");
        return;
    }

    // Remove the truck
    trucks.erase(removeOrder);

    // Re-index the orders
    std::map<int, std::pair<std::string, int>> updatedTrucks;
    int newOrder = 0;
    for (const auto& [order, data] : trucks) {
        updatedTrucks[++newOrder] = data;
    }
    trucks = std::move(updatedTrucks);
    currentOrder = trucks.size();
    printPlatoon();
}

// Get all trucks behind a specific truck
std::vector<std::pair<std::string, int>> TruckManager::getTrucksBehind(const std::string& truck_id) const {
    // int truckOrder = -1;
    int truckOrder = find(truck_id);

    // Collect all trucks with orders greater than the specified truck
    std::vector<std::pair<std::string, int>> result;
    for (const auto& [order, data] : trucks) {
        if (order > truckOrder) {
            result.push_back(data);
        }
    }
    return result;
}

int TruckManager::find(const std::string& truck_id) const {
    for (const auto& [order, data] : trucks) {
        if (data.first == truck_id) {
            return order;
        }
    }
    return -1;
}

int TruckManager::getSocketId(const std::string& truck_id) const {
    for (const auto& [order, data] : trucks) {
        if (data.first == truck_id) {
            return data.second;
        }
    }
    return -1;
    // throw std::invalid_argument("Truck ID not found.");
}

bool TruckManager::isSlotAvailable() const {
    //compare trucks size with total trucks config
    int total_trucks = env_get_int("NUM_TRUCKS", TOTAL_TRUCKS);
    return trucks.size() < total_trucks;
}

std::vector<std::pair<std::string, int>> TruckManager::getTrucks() const {
    std::vector<std::pair<std::string, int>> result;
    for (const auto& [order, data] : trucks) {
        result.push_back(data);
    }
    return result;
}