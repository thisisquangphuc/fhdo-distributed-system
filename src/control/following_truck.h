#ifndef FOLLOWING_TRUCK_H
#define FOLLOWING_TRUCK_H

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include "communication/platoon_client.h"
#include "communication/comm_msg.h"
#include "utils/env.h"
#include "utils/config.h"
#include "utils/logger.h"

class FollowingTruck {
    public:
        FollowingTruck();
        ~FollowingTruck();

        bool askToJoinPlatoon();
        bool joiningPlatoon();
        bool leavingPlatoon();
        bool sendCurrentStatus();
        std::string listenForLeading();
        std::string listenForBroadcast();
        void emergencyBrake();
//        void startBraking();
        int getRetryTimes();
        void resetRetryCounter();
        std::string getTruckStatus();


    private:
        int retry_times;
        int tcp_port;
        int udp_port;
        std::string host_ip;

        std::string truck_id;
        double truck_lat_loc, truck_lon_loc;
        double truck_front_d, truck_back_d, truck_lead_d;
        double truck_speed;
        std::string truck_status;
        double brake_force;
        std::string error_code;

        double ref_speed;

        TruckMessage truck_message;
        
        PlatoonClient platoonClient;

        void initTruck();
        void updateCurrentStatus();
        void monitorDistance();
        void monitorLocation();
        void speedControl();
};

#endif // FOLLOWING_TRUCK_H
