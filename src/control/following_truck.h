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
        void updateCurrentStatus();
        std::string listenForLeading();
        std::string listenForBroadcast();
        bool emergencyBrake();
        int obstacleAvoidance();
        bool alertObstacleDetection();
//        void startBraking();
        void resetRetryCounter();
        std::string processCommands(std::string leading_rsp);

        // setters
        void setTruckID(std::string id) { this->truck_id = id; }
        void setTruckStatus(std::string status) { this->truck_status = status; }
        void setTruckFrontDistance(double distance) { this->truck_front_d = distance; }
        void setTruckBackDistance(double distance) { this->truck_back_d = distance; }
        void setTruckLeadDistance(double distance) { this->truck_lead_d = distance; }
        void setTruckLatLoc(double loc) { this->truck_lat_loc = loc; }
        void setTruckLonLoc(double loc) { this->truck_lon_loc = loc; }
        void setTruckSpeed(double speed) { this->truck_speed = speed; }
        void setRefSpeed(double speed) { this->ref_speed = speed; }
        void setBrakeForce(double brake_value) { this->brake_force = brake_value; }
        
        // getters
        TruckMessage getTruckMessage() { return this->truck_message; }
        std::string getTruckStatus() { return this->truck_status; }
        double getRefSpeed() { return this->ref_speed; }
        double getTruckSpeed() { return this->truck_speed; }
        double getTruckFrontDistance() { return this->truck_front_d; }
        double getTruckBackDistance() { return this->truck_back_d; }
        double getTruckLeadDistance() { return this->truck_lead_d; }
        double getTruckLatLoc() { return this->truck_lat_loc; }
        double getTruckLonLoc() { return this->truck_lon_loc; }
        double getBrakeForce() { return this->brake_force; }
        int getRetryTimes() { return this->retry_times; }


    private:


        int retry_times;
        int tcp_port;
        int udp_port;
        std::string host_ip;
        bool in_Emergercy;

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
        void monitorDistance();
        void monitorLocation();
        void speedControl();
};

#endif // FOLLOWING_TRUCK_H
