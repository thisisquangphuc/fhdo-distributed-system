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

    this->port = env_get_int("PORT", 8080);
    this->host_ip = env_get("HOST_IP", "127.0.0.1");

    // Print port and host info 
    spdlog::debug("[{}]: Port: {}", __func__, this->port);
    spdlog::debug("[{}]: Host IP: {}", __func__, this->host_ip);

    if (!this->platoonClient.startClient(this->port, this->host_ip, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    // 
    spdlog::info("[{}]: Waiting to receive truck info from Leading ... ", __func__);
    json leading_rsp = json::parse(this->platoonClient.receiveMessage()); 

    // Receive authentication result 
    if (leading_rsp["cmd"] != "auth_ok") {
        spdlog::info("[{}]: Authentication failed.", __func__);
        this->retry_times++;
        return false;
    }
    this->truck_id = leading_rsp["contents"]["id"]; // [FIXME]
    this->truck_message.setTruckID(this->truck_id);
//    this->truck_speed = leading_rsp.getSpeed(); // [FIXME] Need to uncomment 
//    this->truck_lead_d = leading_rsp.getLeadDistance(); // [REVIEW]
    std::cout << this->truck_message.buildPayload() << std::endl;

    this->resetRetryCounter();
    return true;
}

// 
bool FollowingTruck::joiningPlatoon() {
    std::string send_message;
    std::string error_message;
    TruckMessage done_message = this->truck_message;

    // Join the platoon
    spdlog::info("[{}]: Joining from the end of platoon system.", __func__);  
    done_message.setTruckID(this->truck_id);
    done_message.setCommand("join");

    send_message = done_message.buildPayload();
    spdlog::info("[{}]: Done joining - {}", __func__, send_message);
    if (!this->platoonClient.sendMessage(send_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    // Do calculation
    spdlog::info("[{}]: {} - Calculating truck status.", __func__, this->truck_id);  
    this->truck_front_d = TRUCK_SAFE_DISTANCE;
    this->truck_back_d = 0.0;
    this->truck_status = "running";

    this->updateCurrentStatus();
    this->resetRetryCounter();
    return true;
}

//
bool FollowingTruck::leavingPlatoon() {
    std::string error_message;
    TruckMessage leave_req = this->truck_message;
    
    leave_req.setCommand("leave");

    spdlog::info("[{}]: Asking to leave the platoon ... - {}", __func__, this->truck_id, leave_req.serialize());
    if (!this->platoonClient.sendMessage(leave_req.buildPayload(), error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    spdlog::info("[{}]: {} - Waiting for approval from LEADING Truck ... ", __func__, this->truck_id);
    TruckMessage server_rsp(this->platoonClient.receiveMessage());
    spdlog::info("[{}]: {} - Response from LEADING truck - {}", __func__, this->truck_id, server_rsp.buildPayload());

    // Prepare and leave the platoon
    spdlog::info("[{}]: {} - Leaving platoon ...", __func__, this->truck_id);  
    
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
bool FollowingTruck::listenForLeading() {
    std::string leading_rsp;
    std::string error_message;
    bool emergency = false;

    spdlog::debug("[{}]: {} - Listening for LEADING Truck ... ", __func__, this->truck_id);
    leading_rsp = this->platoonClient.receiveMessage();
    
    if (!leading_rsp.empty()) {
        spdlog::debug("[{}]: Message from LEADING Truck: {} ", __func__, leading_rsp);
        
        TruckMessage rsp_mess(leading_rsp);
        if (rsp_mess.getCommand() == "emergency") { 
            this->truck_status = "emergency";
            this->brake_force = rsp_mess.getBrakeForce();
            emergency = true;
        }
    }

    return emergency;
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
    this->sendCurrentStatus();
}

// 
void FollowingTruck::updateCurrentStatus() {
    // Calculate and update new status    
    spdlog::info("[{}]: {} - Update truck status.", __func__, this->truck_id);

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
        if (this->truck_back_d != 0.0) // not the last truck in platoon
            this->truck_back_d = (double)TRUCK_SAFE_DISTANCE * (rand() % 10 + 95) / 100;
    }
}

//
void FollowingTruck::speedControl() {
    // When having brake
    if ((this->brake_force > 0) && (this->truck_speed > 0)) { // brake_force and truck is running 
        this->truck_speed -= (double)this->truck_speed*this->brake_force;  
        if (this->truck_speed <= 0) { 
            this->brake_force = 0.0;
            this->truck_speed = 0.0; 
        }
    } else if (this->truck_speed > 0) {
        // simulate speed between [0.95*Speed, 1.15*Speed]
        this->truck_speed = (double)this->truck_speed * (rand() % 10 + 95 ) / 100;
    }

    if (this->truck_speed == 0) this->truck_status = "stop";
}

//
void FollowingTruck::initTruck() {
    this->truck_id = "TRUCK";
    this->retry_times = 0;

    this->truck_lat_loc = 0.0;
    this->truck_lon_loc = 0.0;

    this->truck_front_d = 0.0;
    this->truck_back_d = 0.0;
    this->truck_lead_d = 0.0;

    this->truck_speed = 60.0;
    this->truck_status = "";
    this->brake_force = 0.0;
    this->error_code = "";

    this->truck_message.setTruckID(this->truck_id);
    this->updateCurrentStatus();
}

//
void FollowingTruck::resetRetryCounter() {
    this->retry_times = 0;
}

//
int FollowingTruck::getRetryTimes() {
    return this->retry_times;
}
