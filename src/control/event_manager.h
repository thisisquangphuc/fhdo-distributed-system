/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 07 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include "utils/logger.h"
#include "communication/comm_msg.h"
#include "communication/platoon_server.h"
#include "utils/env.h"
#include <nlohmann/json.hpp>
#include "control/trucks_manager.h"
#include "control/monitor.h"

using json = nlohmann::json;
using namespace std;

class MsgQueue {
    protected:
        std::queue<std::string> queue;       // Command queue
        std::mutex mutex;                    // Protects the queue
        std::condition_variable cv;          // Notifies worker threads
        bool stopFlag = false;               // Indicates if queue processing should stop

    public:
        virtual ~MsgQueue() = default;

        // Add a command to the queue
        virtual void enqueueCommand(const std::string& command) = 0;

        // Retrieve and remove a command from the queue
        virtual std::string dequeueCommand() = 0;

        // Stop processing the queue
        virtual void stop() {
            std::lock_guard<std::mutex> lock(mutex);
            stopFlag = true;
            cv.notify_all();
        }

        // Check if the queue is empty
        bool isEmpty() {
            std::lock_guard<std::mutex> lock(mutex);
            return queue.empty();
        }   
};

class RegularCommandQueue : public MsgQueue {
    public:
        void enqueueCommand(const std::string& command) override {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(command);
            cv.notify_one();
        }

        std::string dequeueCommand() override {
            cout << "Popping from RegularCommandQueue" << endl;
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return !queue.empty() || stopFlag; });
            if (stopFlag && queue.empty()) return "";
            std::string command = queue.front();
            queue.pop();
            return command;
        }
};

class EmergencyCommandQueue : public MsgQueue {
    public:
        void enqueueCommand(const std::string& command) override {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(command);
            cv.notify_one();
        }

        std::string dequeueCommand() override {
            cout << "Popping from EmergencyCommandQueue" << endl;
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return !queue.empty() || stopFlag; });
            if (stopFlag && queue.empty()) return "";
            std::string command = queue.front();
            queue.pop();
            return command;
        }
};

class TruckEventFSM {
    public:
        enum class State { Idle, Authen, Join, Leave, Communicate, Emergency, Obstacle };
        std::mutex databaseMutex;

        TruckEventFSM() : currentState(State::Idle) {}

        void handleEvent(const TruckMessage& msg) {
            switch (currentState) {
                case State::Idle:
                    handleIdle(msg);
                    break;
                case State::Authen:
                    // Auth(msg);
                    break;
                case State::Join:
                    handleJoin(msg);
                    break;
                case State::Leave:
                    handleLeave(msg);
                    break;
                case State::Communicate:
                    handleCommunicate(msg);
                    break;
                case State::Emergency:
                    handleEmergency(msg);
                    break;
                default:
                    spdlog::warn("Unhandled state");
            }
        }

        void setState(const std::string& command) {
            const auto& stateMap = getStateMap();
            auto it = stateMap.find(command);

            if (it != stateMap.end()) {
                currentState = it->second;
                std::cout << "State set to: " << command << std::endl;
            } else {
                throw std::invalid_argument("Invalid command name: " + command);
            }
        }

        bool isEmergencyEnabled() const { return emergencyEnabled; }
        
        State getState() const { return currentState; }

    private:
        State currentState;
        bool emergencyEnabled = false;

        static const std::unordered_map<std::string, State>& getStateMap() {
            static const std::unordered_map<std::string, State> stateMap = {
                {"Idle", State::Idle},
                {"authen", State::Authen},
                {"join", State::Join},
                {"leave", State::Leave},
                {"communicate", State::Communicate},
                {"state", State::Idle},
                {"emergency", State::Emergency},
                {"obstacle", State::Obstacle}
            };
            return stateMap;
        }
    
        // void setState(State newState) { currentState = newState; }

        void handleIdle(const TruckMessage& msg);
        void handleJoin(const TruckMessage& msg);
        void handleLeave(const TruckMessage& msg);
        void handleCommunicate(const TruckMessage& msg);
        void handleEmergency(const TruckMessage& msg);
        bool Auth(const TruckMessage& msg, string truckID);
};

void processCommands(MsgQueue& queue, TruckEventFSM& stateMachine);
inline RegularCommandQueue regularQueue;
inline EmergencyCommandQueue emergencyQueue;
inline TruckEventFSM truckEventFSM;

#endif