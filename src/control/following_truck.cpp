#include "following_truck.h"

// Constructor
FollowingTruck::FollowingTruck() : platoonClient() {
    this->truck_id = -1;
    this->retry_times = 0;
}

// Destructor
FollowingTruck::~FollowingTruck() {}

//
bool FollowingTruck::askToJoinPlatoon(std::string auth_token) {
    std::string error_message;

    this->port = env_get_int("PORT", 8080);
    this->host_ip = env_get("HOST_IP", "127.0.0.1");

    // Print port and host info 
    spdlog::info("Port: {}", this->port);
    spdlog::info("Host IP: {}", this->host_ip);

    if (!this->platoonClient.startClient(this->port, this->host_ip, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        return false;
    }

    // platoonClient.setAuthToken("expectedToken123");
    if (!this->platoonClient.sendMessage(auth_token, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        return false;
    }
    // this->platoonClient.closeClientSocket();
    return true;
}

// 
bool FollowingTruck::joiningPlatoon() {
    std::string error_message;
    std::string done_message;

    spdlog::info("Waiting to receive truck info from Leading ... ");
    truck_info = this->platoonClient.receiveMessage();
    spdlog::info("Truck info : {:s}", truck_info);

    // Do calculation and join platoon
    spdlog::info("Joining from the end of platoon system.");  
    done_message = "Done joining platoon" + truck_info;
    std::cout << done_message << std::endl;
    if (!this->platoonClient.sendMessage(done_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }
    this->retry_times = 0;
    return true;
}

//
bool FollowingTruck::leavingPlatoon() {
    std::string error_message;
    std::string leave_message;

    spdlog::info("Asking to leave the platoon ...");
    leave_message = "Leave platoon"; // [FIXME] Convert to JSON
    std::cout << leave_message << std::endl;
    if (!this->platoonClient.sendMessage(leave_message, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }

    spdlog::info("Waiting for approval from LEADING Truck ... ");
    leave_message = this->platoonClient.receiveMessage();
    spdlog::info("Response from LEADING truck : {:s}", leave_message);

    // Prepare and leave the platoon
    spdlog::info("Leaving platoon ...");  
    
    this->platoonClient.closeClientSocket();
    this->retry_times = 0;
    return true;
}

//
bool FollowingTruck::sendCurrentStatus() {
    std::string error_message;

    spdlog::info("Send truck status to LEADING truck.");
    if (!this->platoonClient.sendMessage(truck_info, error_message)) {
        std::cerr << "Error conneting to server: " << error_message << std::endl;
        this->retry_times++;
        return false;
    }
    this->retry_times = 0;
    return true;
}

//
bool FollowingTruck::listenForEmergency() {
    std::string error_message;
    std::string done_message;

    spdlog::info("Listening for LEADING Truck ... ");
    truck_info = this->platoonClient.readMessage();
    spdlog::info("Message from LEADING Truck : {:s}", truck_info);
    
    return true; // [FIXME] Check if emergency happens
}

void FollowingTruck::resetRetryCounter() {
    this->retry_times = 0;
}

int FollowingTruck::getRetryTimes() {
    return this->retry_times;
}