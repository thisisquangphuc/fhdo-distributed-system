#ifndef FOLLOWING_TRUCK_H
#define fOLLOWING_TRUCK_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include "communication/platoon_client.h"
#include "utils/env.h"
#include "utils/config.h"
#include "utils/logger.h"
using json = nlohmann::json;

class FollowingTruck {
    public:
        FollowingTruck();
        ~FollowingTruck();

        bool askToJoinPlatoon();
        bool joiningPlatoon();
        bool leavingPlatoon();
        bool sendCurrentStatus();
        bool listenForLeading();
        void emergencyBrake();
        void startBraking();
        int getRetryTimes();
        void resetRetryCounter();


    private:
        int port;
        int retry_times;
        std::string host_ip;

        std::string truck_id;
        json truck_info;
        json truck_location;
        json truck_distance;
        double truck_speed;
        std::string truck_status;
        double brake_force;
        std::string error_code;

        int msg_id;
        
        PlatoonClient platoonClient;

        void updateCurrentStatus();
        void monitorDistance();
        void speedControl();
};

#endif // FOLLOWING_TRUCK_H