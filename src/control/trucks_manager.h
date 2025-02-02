/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#ifndef TRUCKS_MANAGER_H
#define TRUCKS_MANAGER_H

#include "utils/logger.h"
#include "communication/comm_msg.h"
#include "utils/env.h"
#include "utils/config.h"

class TruckManager {
    private:
        std::map<int, std::pair<std::string, int>> trucks; // Map: order -> truck ID, socket 
        int currentOrder = 0; // Tracks the next available order

        // Private constructor to prevent instantiation
        TruckManager() = default;

        // Delete copy constructor and assignment operator to prevent copying
        TruckManager(const TruckManager&) = delete;
        TruckManager& operator=(const TruckManager&) = delete;

    public:
        // Static method to get the singleton instance
        static TruckManager& getInstance() {
            static TruckManager instance;
            return instance;
        }

        // Add a truck to the platoon
        void addTruck(const std::string& truck_id, int socket);

        // Remove a truck by ID and update orders
        void removeTruck(const std::string& truck_id);

        // Check if slot is available
        bool isSlotAvailable() const;

        // Get all trucks behind a specific truck
        std::vector<std::pair<std::string, int>> getTrucksBehind(const std::string& truck_id) const;

        //Get all trucks
        std::vector<std::pair<std::string, int>> getTrucks() const;

        int find(const std::string& truck_id) const;

        int getSocketId(const std::string& truck_id) const;

        // Print all trucks in the platoon
        void printPlatoon() const {
            std::cout << "Current Platoon:\n";
            // PRint total number of trucks
            std::cout << "Total number of trucks: " << trucks.size() << "\n";
            for (const auto& [order, data] : trucks) {
                std::cout << "Order: " << order 
                      << ", Truck ID: " << data.first 
                      << ", Socket ID: " << data.second << "\n";
            }
        }


};

#endif