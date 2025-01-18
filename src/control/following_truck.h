#ifndef FOLLOWING_TRUCK_H
#define fOLLOWING_TRUCK_H

#include "communication/platoon_client.h"
#include <iostream>
#include <string>
#include <cstring>
#include "utils/env.h"
#include "utils/config.h"
#include "utils/logger.h"

class FollowingTruck {
    public:
        FollowingTruck();
        ~FollowingTruck();

        bool askToJoinPlatoon(std::string auth_token);
        bool joiningPlatoon();
        bool leavingPlatoon();
        bool sendCurrentStatus();
        bool listenForEmergency();
        int getRetryTimes();
        void resetRetryCounter();


    private:
        int truck_id;
        int port;
        int retry_times;
        std::string host_ip;
        std::string truck_info;
        PlatoonClient platoonClient;
};

#endif // FOLLOWING_TRUCK_H