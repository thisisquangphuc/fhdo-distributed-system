#include "following_truck.h"

// Constructor
FollowingTruck::FollowingTruck() : platoonClient() {
    this->truck_id = "TRUCK";
    this->retry_times = 0;

    this->truck_location["lat"] = 0.0;
    this->truck_location["lon"] = 0.0;

    this->truck_distance["front"] = 0.0;
    this->truck_distance["back"] = 0.0;

    this->truck_speed = 40.0;
    this->truck_status = "";
    this->brake_force = 0.0;
    this->error_code = "";

    this->msg_id = rand() % 1000 + 1000;

   this->truck_info["truck_id"] = this->truck_id;
    this->updateCurrentStatus();

    srand(time(0));
}

// Destructor
FollowingTruck::~FollowingTruck() {}

//
bool FollowingTruck::askToJoinPlatoon() {
    std::string error_message;
    std::string auth_token = env_get("SESSION_KEY", "PHUC01QUYEN02ISSAC03DAO04");

    spdlog::info("Authentication to join platoon system ... ");

    json auth_req;
    auth_req["cmd"] = "auth";
    auth_req["contents"]["auth_key"] = auth_token;
    auth_req["msg_id"] = this->msg_id++;
    auth_req["timestamp"] = time(NULL);

    std::cout << auth_req.dump() << std::endl;

    // this->port = env_get_int("PORT", 8080);
    // this->host_ip = env_get("HOST_IP", "127.0.0.1");

    // // Print port and host info 
    // spdlog::info("Port: {}", this->port);
    // spdlog::info("Host IP: {}", this->host_ip);

    // if (!this->platoonClient.startClient(this->port, this->host_ip, error_message)) {
    //     std::cerr << "Error conneting to server: " << error_message << std::endl;
    //     return false;
    // }

    // // platoonClient.setAuthToken("expectedToken123");
    // if (!this->platoonClient.sendMessage(auth_req.dump(), error_message)) {
    //     std::cerr << "Error conneting to server: " << error_message << std::endl;
    //     return false;
    // }

    // this->platoonClient.closeClientSocket();
    return true;
}

// 
bool FollowingTruck::joiningPlatoon() {
    std::string error_message;
    json done_message;
    json leading_rsp;

    spdlog::info("Waiting to receive truck info from Leading ... ");
    // leading_rsp = json::parse(this->platoonClient.receiveMessage()); 

    // if (leading_rsp["cmd"] != "auth_ok") {
    //     spdlog::info("Authentication failed.");
    //     return false;
    // }

    // Do calculation and join platoon
    spdlog::info("Joining from the end of platoon system.");  
    // this->truck_id = leading_rsp["contents"]["id"];
    this->truck_distance["front"] = TRUCK_SAFE_DISTANCE;
    this->truck_distance["back"] = 0.0;
    this->truck_status = "running";

    // this->updateCurrentStatus();

    // done_message = this->truck_info;
    // done_message["cmd"] = "join";
    // done_message["msg_id"] = this->msg_id++;
    // done_message["timestamp"] = time(NULL);

    std::cout << "Done joining " << done_message.dump(3) << std::endl;
    // if (!this->platoonClient.sendMessage(done_message.dump(), error_message)) {
    //     std::cerr << "Error conneting to server: " << error_message << std::endl;
    //     this->retry_times++;
    //     return false;
    // }

    this->retry_times = 0;
    return true;
}

//
bool FollowingTruck::leavingPlatoon() {
    std::string error_message;
    json server_rsp;
    json leave_req;
    
    leave_req["cmd"] = "leave";
    leave_req["truck_id"] = this->truck_id;
    leave_req["msg_id"] = this->msg_id++;
    leave_req["timestamp"] = time(NULL);

    spdlog::info("Asking to leave the platoon ...");
    std::cout << leave_req.dump() << std::endl;
    // if (!this->platoonClient.sendMessage(leave_req.dump(), error_message)) {
    //     std::cerr << "Error conneting to server: " << error_message << std::endl;
    //     this->retry_times++;
    //     return false;
    // }

    // spdlog::info("Waiting for approval from LEADING Truck ... ");
    // server_rsp = json::parse(this->platoonClient.receiveMessage());
    // spdlog::info("Response from LEADING truck : {:s}", server_rsp.dump());

    // Prepare and leave the platoon
    spdlog::info("Leaving platoon ...");  
    
    // this->platoonClient.closeClientSocket();
    this->retry_times = 0;
    return true;
}

//
bool FollowingTruck::sendCurrentStatus() {
    std::string error_message;

    this->updateCurrentStatus();

    spdlog::info("Send truck status to LEADING truck.");

    // if (!this->platoonClient.sendMessage(truck_info.dump(), error_message)) {
    //     std::cerr << "Error conneting to server: " << error_message << std::endl;
    //     this->retry_times++;
    //     return false;
    // }
    this->retry_times = 0;
    return true;
}

//
bool FollowingTruck::listenForLeading() {
    std::string error_message;
    std::string leading_rsp;
    bool emergency = true;

    spdlog::info("Listening for LEADING Truck ... ");
    // leading_rsp = json::parse(this->platoonClient.readMessage());
    // std::cout << "Message from LEADING Truck : " << leading_rsp.dump() << std::endl;

    // if (leading_rsp["cmd"] == "emergency") emergency = true;

    // return emergency;
    return ((rand() % 10) == 0) ? true : false ; // [FIXME] Check if emergency happens
}

//
void FollowingTruck::startBraking() {
    this->brake_force = (double)(rand() % 10 + 5) / 100;
    if (this->truck_speed == 0) this->truck_status = "emergency";
}

// 
void FollowingTruck::emergencyBrake() {
    this->brake_force *= (double)(rand() % 3 + 1); // increase force to accelarate stop
    if (this->brake_force >= MAX_BRAKE_FORCE) this->brake_force = MAX_BRAKE_FORCE;
    this->sendCurrentStatus();
}

// 
void FollowingTruck::updateCurrentStatus() {
    // Calculation and update new status    
    this->monitorDistance();
    this->speedControl();

    this->truck_info["contents"]["locations"] = this->truck_location;
    this->truck_info["contents"]["distance"] = this->truck_distance;
    this->truck_info["contents"]["speed"] = this->truck_speed;
    this->truck_info["contents"]["status"] = this->truck_status;
    this->truck_info["contents"]["brake_force"] = this->brake_force;
    this->truck_info["contents"]["error_code"] = this->error_code;

    spdlog::info("Update truck status");
    std::cout << this->truck_info.dump(3) << std::endl;
}

//
void FollowingTruck::monitorDistance() {
    if (this->truck_speed > 0) {
        double front_d = (double)TRUCK_SAFE_DISTANCE * (rand() % 7 + 5) / 10;  
        double back_d = (double)TRUCK_SAFE_DISTANCE * (rand() % 7 + 5) / 10;
    }

    this->truck_distance["front"] = front_d;
    this->truck_distance["back"] = back_d;
}

//
void FollowingTruck::speedControl() {
    // When having brake
    if (this->brake_force > 0) {
        // if truck is running
        if (this->truck_speed > 0) {
            this->truck_speed -= (double)this->truck_speed*this->brake_force;
        }
        if (this->truck_speed <= 0) this->truck_speed = 0;
    } else {
       this->truck_speed = (double)this->truck_speed * (rand() % 20 + 90 ) / 100;
    }

    if (this->truck_speed == 0) this->truck_status = "stop";
}

void FollowingTruck::resetRetryCounter() {
    this->retry_times = 0;
}

int FollowingTruck::getRetryTimes() {
    return this->retry_times;
}