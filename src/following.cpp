#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <omp.h>
#include <pthread.h> 
#include "communication/platoon_client.h"
#include "control/following_truck.h"

unsigned int microsecond = 1000000;

typedef enum {
    IDLE,
    JOINING,
    LEAVING,
    NORMAL_OPERATION,
    SPEED_UP,
    SLOW_DOWN,
    EMERGENCY_BRAKE,
    CONNECTION_LOST,
} TruckState;

TruckState current_state = IDLE;
TruckState next_state = current_state;

TruckState get_current_state() {
    return current_state;
}

typedef enum {
    NONE,
    ASKTOJOIN,
    ASKTOLEAVE
} TruckRequest;

TruckRequest request;

void following_fsm();
void simulate_request();
void send_current_status();
bool check_emergency_brake();
void load_environment(std::string env_file);

FollowingTruck followingTruck;

int main()
{
    init_logger();
    
    //load evironment
    load_environment(std::filesystem::current_path().u8string() + "/.env");

    #pragma omp parallel sections 
    {
        #pragma omp section
        {
            following_fsm();
        }
        #pragma omp section
        {
            simulate_request();
        }
    }

    return 0;
}

// 
void following_fsm() {
    while (1) {
        // spdlog::info(current_state);
        switch (current_state) {
            case IDLE:
                if (request == ASKTOJOIN) {
                    if (followingTruck.askToJoinPlatoon("expectedToken123")) {
                        spdlog::info("Authenticate successfully.");
                        next_state = JOINING;
                        spdlog::info(next_state);
                    }
                } 
                break;

            case JOINING:
                spdlog::info("Joining platoon ...");
                if (followingTruck.joiningPlatoon()) { 
                    next_state = NORMAL_OPERATION;
                } else if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                    spdlog::info("Failed to join platoon.");
                    next_state = IDLE;
                } 
                break;

            case NORMAL_OPERATION:
                // #pragma omp parallel sections 
                // {
                //     #pragma omp section
                //     {
                //         send_current_status();
                //     }   
                //     #pragma omp section 
                //     {
                //         if (check_emergency_brake()) next_state = EMERGENCY_BRAKE;
                //     }
                //     #pragma omp section
                //     {
                        // while (true) {
                            // std::cout << "aaaaaaaaaaaaa " << request << std::endl;
                            if (request == ASKTOLEAVE) {
                                followingTruck.resetRetryCounter();
                                next_state = LEAVING;
                                // break;
                            }
                        // }
                //     }
                // }
                break;

            case SPEED_UP:
                break;

            case SLOW_DOWN:
                break;

            case EMERGENCY_BRAKE:
                break;

            case CONNECTION_LOST:
                break;

            case LEAVING:
                if (followingTruck.leavingPlatoon()) {
                    spdlog::info("Leaving platoon successfully.");
                    next_state = IDLE;
                } else if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                    spdlog::info("Cannot leave platoon. Please try again.");
                    followingTruck.resetRetryCounter();
                    next_state = NORMAL_OPERATION;
                }
                break;

            default:
                next_state = IDLE;
                break;
        }
        current_state = next_state;
    }
}

// 
void simulate_request() {
    int input;
    while (1) {
        std::cout << "Request to server (1-joinPlatoon, 2-leavePlatoon): "; 
        std::cin >> input;
        request = (TruckRequest)input;
        std::cout << "Request value: " << request << std::endl;
        sleep(1);
        request = NONE;
    }
}

// 
void send_current_status() {
    while (next_state == NORMAL_OPERATION) {
        if (!followingTruck.sendCurrentStatus()) {
            spdlog::warn("Failed to send current status to server.");
        }
        // if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
        //     spdlog::error("Connection lost with server.");
        // }
        // send 2 seconds
        sleep(2); 
    }
}

//
bool check_emergency_brake() {
    while (!followingTruck.listenForEmergency());
    return true;
}

//
void load_environment(std::string env_file) {

    std::string env_file_string = env_file;

    if (env_file_string.empty())
        env_file_string = SERVICE_ENV_FILE_DEFAULT;
    env_init(env_file_string.c_str());

    std::cout << "Project Name: " << env_get("PROJECT_NAME", "Unknown") << std::endl;
}
