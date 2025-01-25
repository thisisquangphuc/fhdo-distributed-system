#include "following_truck.h"

// Constructor
FollowingTruck::FollowingTruck() : platoonClient() {
    initTruck();
    srand(time(0));
}

// Destructor
FollowingTruck::~FollowingTruck() {}


//
bool FollowingTruck::askToJoinPlatoon() {
    std::string send_message;
    std::string error_message;
    std::string auth_token = env_get("SESSION_KEY", "PHUC01QUYEN02ISSAC03DAO04");

    spdlog::info("[{}]: Authentication to join platoon system ... ", __func__);

    TruckMessage auth_req;
    
    auth_req.setCommand("auth");
    auth_req.setAuthenKey(auth_token);  

    send_message = auth_req.buildPayload();
    spdlog::info("[{}]: {}", __func__, send_message);               

    this->tcp_port = env_get_int("PORT", 8080);
    this->udp_port = env_get_int("UDP_PORT", 59059);
    this->host_ip = env_get("HOST_IP", "127.0.0.1");

    // Print port and host info 
    spdlog::debug("[{}]: TCP-Port: {}", __func__, this->tcp_port);
    spdlog::debug("[{}]: UDP-Port: {}", __func__, this->udp_port);
    spdlog::debug("[{}]: Host IP: {}", __func__, this->host_ip);

    if (!this->platoonClient.startClient(this->tcp_port, this->udp_port, this->host_ip, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

//    if (!this->platoonClient.initUDPConnection(this->udp_port, this->host_ip, error_message)) {
//        std::cerr << "Error conneting to server: " << error_message << std::endl;
//        this->retry_times++;
//        return false;
//    }

    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    // 
    json leading_rsp = json::parse(this->platoonClient.receiveMessage()); 
    spdlog::info("[{}]: Response from LEADING ... {}", __func__, leading_rsp.dump());
    // Receive authentication result 
    if (leading_rsp["cmd"] != "auth_ok") {
        spdlog::info("[{}]: Authentication failed.", __func__);
        this->retry_times++;
        return false;
    }
    this->truck_id = leading_rsp["contents"].value("id", "TRUCK_0000");
    this->truck_message.setTruckID(this->truck_id);
    std::cout << this->truck_message.buildPayload() << std::endl;

    this->resetRetryCounter();
    return true;
}

// 
bool FollowingTruck::joiningPlatoon() {
    std::string send_message;
    std::string error_message;
    TruckMessage join_message = this->truck_message;

    // Join the platoon
    join_message.setTruckID(this->truck_id);
    join_message.setCommand("join");

    send_message = join_message.buildPayload();
    spdlog::info("[{}]: Joining from the end of platoon system - {}", __func__, send_message);  
    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    // Receive truck info 
    spdlog::info("[{}]: Receive truck info from LEADING ... ", __func__);
    TruckMessage leading_rsp(this->platoonClient.receiveMessage()); 

    // Receive joining result 
    if (leading_rsp.getCommand() != "join_accepted") {
        spdlog::info("[{}]: Joining failed.", __func__);
        this->retry_times++;
        return false;
    }

    // Do calculation
    spdlog::info("[{}]: {} - Calculating truck status.", __func__, this->truck_id);  
    this->truck_front_d = TRUCK_SAFE_DISTANCE;
    this->truck_back_d = TRUCK_SAFE_DISTANCE;
//    this->truck_lead_d = leading_rsp.getLeadDistance(); // [REVIEW]
    this->truck_status = "runnnig";
    this->truck_speed = leading_rsp.getSpeed(); 

    this->updateCurrentStatus();

    // Done joining
    join_message = this->truck_message;
    join_message.setCommand("join_done");

    send_message = join_message.buildPayload();
    spdlog::info("[{}]: Done joining - {}", __func__, send_message);
    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    this->resetRetryCounter();
    return true;
}

//
bool FollowingTruck::leavingPlatoon() {
    std::string send_message;
    std::string error_message;
    TruckMessage leave_message = this->truck_message;
    
    // Leave request
    leave_message.setCommand("leave");

    send_message = leave_message.buildPayload();
    spdlog::info("[{}]: Asking to leave the platoon ... - {}", __func__, this->truck_id, send_message);
    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    // Approval from leading 
    spdlog::info("[{}]: {} - Waiting for approval from LEADING Truck ... ", __func__, this->truck_id);
    TruckMessage leading_rsp(this->platoonClient.receiveMessage());

    // Receive joining result 
    if (leading_rsp.getCommand() != "leave_start") {
        spdlog::info("[{}]: Leaving failed.", __func__);
        this->retry_times++;
        return false;
    }

    // Prepare and leave the platoon
    leave_message.setCommand("leave_done");

    send_message = leave_message.buildPayload();
    spdlog::info("[{}]: Leaving the platoon ... - {}", __func__, send_message);
    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }
    
    this->platoonClient.closeClientSocket();
    this->resetRetryCounter();
    return true;
}

//
bool FollowingTruck::sendCurrentStatus() {
    std::string send_message;
    std::string error_message;

    this->updateCurrentStatus();

    this->truck_message.setCommand("status");

    send_message = this->truck_message.buildPayload();
    spdlog::info("[{}]: {} - Send truck status to LEADING truck - {} ", 
            __func__, 
            this->truck_id, 
            send_message);

    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }
    this->resetRetryCounter();
    return true;
}

//
std::string FollowingTruck::listenForLeading() {
    std::string leading_rsp;
    std::string error_message;

    spdlog::debug("[{}]: {} - Listening for LEADING Truck ... ", __func__, this->truck_id);
    leading_rsp = this->platoonClient.receiveMessage();
    
    if (!leading_rsp.empty()) {
        json leading_mess = json::parse(leading_rsp);
        spdlog::debug("[{}]: Message from LEADING Truck: {} ", __func__, leading_mess.dump());
        
        std::string leading_cmd = leading_mess["cmd"];

        if (leading_cmd == "emergency") {
            this->truck_status = "emergency";
            this->brake_force = (double)leading_mess["contents"].value("brake_force", this->brake_force);
            this->ref_speed = (double)leading_mess["contents"].value("speed", this->truck_speed);
        } else if (leading_cmd == "slow_down") {
            this->brake_force = (double)leading_mess["contents"].value("brake_force", this->brake_force);
            this->ref_speed = (double)leading_mess["contents"].value("speed", this->truck_speed);
        } else if (leading_cmd == "speed_up") {
            this->truck_speed += TRUCK_SPEED_UP_SPEED;
            this->ref_speed = (double)leading_mess["contents"].value("speed", this->truck_speed);
        } else {
            leading_cmd = "";
        }

        this->in_Emergercy = (leading_cmd == "emergency") ? true : false;
        return leading_cmd;
    }
    this->in_Emergercy = false;
    return "";
}

// 
std::string FollowingTruck::listenForBroadcast() {
    std::string leading_rsp;
    std::string error_message;

    spdlog::debug("[{}]: {} - Listening for Broadcast ... ", __func__, this->truck_id);
    leading_rsp = this->platoonClient.receiveUDPMessage();

    if (!leading_rsp.empty()) {
        json leading_mess = json::parse(leading_rsp);
        spdlog::debug("[{}]: Broadcast Message: {} ", __func__, leading_mess.dump());
        
        std::string leading_cmd = leading_mess["cmd"];

        if (leading_cmd == "emergency") {
            this->truck_status = "emergency";
            this->brake_force = (double)leading_mess["contents"].value("brake_force", this->brake_force);
            this->ref_speed = (double)leading_mess["contents"].value("speed", this->truck_speed);
        } else if (leading_cmd == "slow_down") {
            this->brake_force = (double)leading_mess["contents"].value("brake_force", this->brake_force);
            this->ref_speed = (double)leading_mess["contents"].value("speed", this->truck_speed);
        } else if (leading_cmd == "speed_up") {
            this->truck_speed += TRUCK_SPEED_UP_SPEED;
            this->ref_speed = (double)leading_mess["contents"].value("speed", this->truck_speed);
        } else {
            leading_cmd = "";
        }
        this->in_Emergercy = (leading_cmd == "emergency") ? true : false;
        return leading_cmd;
    }
    this->in_Emergercy = false;
    return "";
}

//
//void FollowingTruck::startBraking() {
//    this->brake_force = (double)(rand() % 10 + 5) / 100;
//    if (this->truck_speed == 0) this->truck_status = "";
//}

// 
void FollowingTruck::emergencyBrake() {
//    this->brake_force *= (double)(rand() % 3 + 1); // [FIXME] increase force to accelarate stop
    if (this->brake_force >= MAX_BRAKE_FORCE) this->brake_force = MAX_BRAKE_FORCE;
    this->in_Emergercy = true;
    this->sendCurrentStatus();
}

// 
void FollowingTruck::updateCurrentStatus() {
    // Calculate and update new status    
    spdlog::debug("[{}]: {} - Update truck status.", __func__, this->truck_id);

    this->monitorDistance();
    this->speedControl();

    this->truck_message.setLocation(this->truck_lat_loc, this->truck_lon_loc);
    this->truck_message.setDistances(this->truck_front_d, this->truck_back_d, this->truck_lead_d);
    this->truck_message.setSpeed(this->truck_speed);
    this->truck_message.setStatus(this->truck_status);
    this->truck_message.setBrakeForce(this->brake_force);
    this->truck_message.setErrorCode(this->error_code);
}

//
void FollowingTruck::monitorDistance() {
    if (this->truck_speed > 0) {
        // simulate distance between [0.95*Distance, 1.15*Distance]
        this->truck_front_d = (double)TRUCK_SAFE_DISTANCE * (rand() % 10 + 95) / 100;  
//        if (this->truck_back_d != 0.0) // not the last truck in platoon
        this->truck_back_d = (double)TRUCK_SAFE_DISTANCE * (rand() % 10 + 95) / 100;
    }
}

//
void FollowingTruck::monitorLocation() {
    if (this->truck_speed > 0) {    
        // simulate location 
        this->truck_lat_loc = (rand() % 2) ? 
            (this->truck_lat_loc + ((rand() % 20)/ 1000)) : 
            (this->truck_lat_loc - ((rand() % 20) / 1000));

        this->truck_lon_loc = (rand() % 2) ? 
            (this->truck_lon_loc + ((rand() % 20)/ 1000)) : 
            (this->truck_lon_loc - ((rand() % 20) / 1000));
    }}

//
void FollowingTruck::speedControl() {
    // When having brake
    if ((this->brake_force > 0) && (this->truck_speed > 0)) { // brake_force and truck is running 
        this->truck_speed -= (double)TRUCK_BRAKE_SPEED*this->brake_force;  
        //
        if (this->truck_speed <= 0) { 
            this->brake_force = 0.0;
            this->truck_speed = 0.0; 

            this->ref_speed = 0.0;
        }
        // maintain speed to sync with platoon system  
        if (this->truck_speed < (ref_speed*0.95)) {
            this->truck_speed = (double)this->ref_speed * (rand() % 10 + 95 ) / 100;
            this->brake_force = 0.0;

            this->ref_speed = 0.0;
            if (!this->in_Emergercy) this->truck_status = "running";
        }
    } else if (this->truck_speed > 0) { // while running
        // 
        if (this->getTruckStatus() == "speed_up") {
            this->truck_speed += TRUCK_SPEED_UP_SPEED * (rand() % 10 + 95 ) / 100;
            // maintain speed to sync with platoon system  
            if (this->truck_speed > (ref_speed*1.15)) {
                this->truck_speed = (double)this->ref_speed * (rand() % 10 + 95 ) / 100;
                this->truck_status = "running";
                
                this->ref_speed = 0.0;
            }
        } else { // normal
          // simulate speed between [0.95*Speed, 1.15*Speed]
          this->truck_speed = (double)this->truck_speed * (rand() % 10 + 95 ) / 100;
        }
    }

    if (this->truck_speed == 0) this->truck_status = "stopped";
}

//
void FollowingTruck::initTruck() {
    this->truck_id = "TRUCK";
    this->retry_times = 0;
    this->in_Emergercy = false;

    this->truck_lat_loc = 51.4941;
    this->truck_lon_loc = 7.4204;

    this->truck_front_d = 0.0;
    this->truck_back_d = 0.0;
    this->truck_lead_d = 0.0;

    this->truck_speed = 60.0;
    this->truck_status = "";
    this->brake_force = 0.0;
    this->error_code = "";

    this->ref_speed = 0.0;

    this->truck_message.setTruckID(this->truck_id);

    this->updateCurrentStatus();
}

//
std::string FollowingTruck::getTruckStatus() {
    return this->truck_status;
}

//
void FollowingTruck::resetRetryCounter() {
    this->retry_times = 0;
}

//
int FollowingTruck::getRetryTimes() {
    return this->retry_times;
}
