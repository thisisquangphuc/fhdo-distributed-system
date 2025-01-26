#include <stdio.h>
#include <stdlib.h> 
#include <filesystem>
#include <pthread.h> 
#include "communication/platoon_client.h"
#include "control/following_truck.h"

unsigned int microsecond = 1000000;

//
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
TruckState prev_state = next_state;

TruckState get_current_state() {
    return current_state;
}

TruckState get_next_state() {
    return next_state;
}

//
typedef enum {
    NONE,
    ASK_TO_JOIN,
    ASK_TO_LEAVE,
    OBSTACLE_DETECT
} TruckRequest;

TruckRequest request;

//
typedef enum {
    NORMAL,
    BRAKING,
    SLOWING,
    SPEEDING,
    OSTACLE 
} LeadNoti;
LeadNoti leadingNoti;

void* following_fsm(void* arg);
void* simulate_request(void* arg);
void* send_current_status(void* arg);
void* check_obstacle(void* arg);
void* check_lead_message(void* arg);
void* check_broadcast_message(void* arg);
void* emergency_brake(void* arg);
void load_environment(std::string env_file);

FollowingTruck followingTruck;

int main()
{
    init_logger();
    
    //load evironment
    load_environment(std::filesystem::path(std::filesystem::current_path()).parent_path().u8string() + "/src/.env");

    pthread_t thread_fsm;
    pthread_t thread_req;

    pthread_create(&thread_fsm, NULL, &following_fsm, NULL);
    pthread_create(&thread_req, NULL, &simulate_request, NULL);

//    spdlog::debug("Wait for main control threads EXIT");
    pthread_join(thread_fsm, NULL); 
//    spdlog::debug("FSM thread EXIT");
    pthread_join(thread_req, NULL); 
//    spdlog::debug("REQ thread EXIT");
    
    return 0;
}

// 
void* following_fsm(void* arg) {
    pthread_detach(pthread_self()); 

    pthread_t thread_send_stats;
    pthread_t thread_check_obstacle;
    pthread_t thread_check_lead;
    pthread_t thread_check_broadcast;
    // pthread_t thread_req_lead;
    pthread_t thread_emg_brake;

    while (true) {
        switch (get_current_state()) {
            case IDLE: //{
                if (request == ASK_TO_JOIN) {
                    int period = env_get_int("LISTEN_LEADING_PERIOD", 1);
                    if (followingTruck.askToJoinPlatoon()) {
                        spdlog::info("Authenticate successfully.");
                        next_state = JOINING;
                    } else if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                        spdlog::info("Failed to authenticate truck.");
                    }
                    sleep(period);
                } 
                prev_state = IDLE; 
                break;
            //}
            case JOINING: //{
                spdlog::info("Joining platoon ...");
                if (followingTruck.joiningPlatoon()) { 

                    pthread_create(&thread_send_stats, NULL, &send_current_status, NULL);
                    pthread_create(&thread_check_obstacle, NULL, &check_obstacle, NULL);
                    pthread_create(&thread_check_lead, NULL, &check_lead_message, NULL);
                    pthread_create(&thread_check_broadcast, NULL, &check_broadcast_message, NULL);
                    // pthread_create(&thread_req_lead, NULL, &request_to_lead, NULL);

                    next_state = NORMAL_OPERATION;
                } else if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                    spdlog::info("Failed to join platoon.");
                    next_state = IDLE;
                } 
                break;
            //}
            case NORMAL_OPERATION: //{
                                   
                if (request == ASK_TO_LEAVE) {
                    spdlog::info("Send leaving request.");
                    next_state = LEAVING;
                } 

                if (leadingNoti == BRAKING) {
                    spdlog::info("Receive emergency brake from LEADING truck.");
//                    followingTruck.startBraking();
                    pthread_create(&thread_emg_brake, NULL, &emergency_brake, NULL);
                    next_state = EMERGENCY_BRAKE;
                }

                if (leadingNoti == SLOWING) {
                    spdlog::info("Receive slowing down from LEADING truck.");
                    next_state = SLOW_DOWN;
                }

                if (leadingNoti == SPEEDING) {
                    spdlog::info("Receive speeding up from LEADING truck.");
                    next_state = SPEED_UP;
                }

                prev_state = current_state;
                break;
            //}
            case SPEED_UP: //{
                if (followingTruck.getTruckStatus() != "speed_up") {
                    next_state = NORMAL_OPERATION;
                }
                prev_state = current_state;
                break;
            //}
            case SLOW_DOWN: //{
                if (followingTruck.getTruckStatus() != "slow_down") {
                    next_state = NORMAL_OPERATION;
                }
                prev_state = current_state;
                break;
            //}
            case EMERGENCY_BRAKE: //{
                if (leadingNoti != BRAKING) {
                    pthread_join(thread_emg_brake, NULL);
                    next_state = NORMAL_OPERATION;
                }
                prev_state = current_state;
                break;
            //}
            case CONNECTION_LOST: //{
                if (followingTruck.sendCurrentStatus()) {
                    followingTruck.resetRetryCounter();
                    next_state = prev_state;
                }
                break;  
            //}
            case LEAVING: //{
                spdlog::debug("Wait for threads EXIT");

                pthread_join(thread_send_stats, NULL); 
                pthread_join(thread_check_obstacle, NULL); 
                pthread_join(thread_check_lead, NULL); 
                pthread_join(thread_check_broadcast, NULL); 
                // pthread_join(thread_req_lead, NULL); 

                spdlog::debug("Threads EXIT");

                if (followingTruck.leavingPlatoon()) {
                    spdlog::info("Leaving platoon successfully.");
                    next_state = IDLE;
                } else if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                    spdlog::info("Cannot leave platoon. Please try again.");
                    followingTruck.resetRetryCounter();
                    next_state = NORMAL_OPERATION;
                }
                prev_state = current_state;
                break;
            //}
            default:
                next_state = IDLE;
                break;
        }
        current_state = next_state;
    }
    pthread_exit(NULL);
}

// 
void* simulate_request(void* arg) {
    pthread_detach(pthread_self()); 

    std::string input;
    int period = env_get_int("SIMULATION_REQ_PERIOD", 1);
    while (true) {
        std::cout << "Request to server (0->Exit Program, 1->Join Platoon, 2->Leave Platoon): "; 
        std::cin >> input;
        if (input == "0") break;
        if (input == "1") request=ASK_TO_JOIN;
        else if (input == "2") request=ASK_TO_LEAVE;
        else if (input == "3") request=OBSTACLE_DETECT;
        else request = NONE;
        std::cout << "Request value: " << request << std::endl;
        sleep(period);
        request = NONE;
    }

    pthread_exit(NULL);
}

// 
void* send_current_status(void* arg) {
    pthread_detach(pthread_self()); 

    spdlog::debug("Send current status to LEADING");
    // wait to change state
    while (get_current_state() != NORMAL_OPERATION);
    //
    int period = env_get_int("SEND_STATUS_PERIOD", 2);
    while (true) {
        if (get_current_state() == IDLE) break;
        if ((get_current_state() != LEAVING) && (get_current_state() != CONNECTION_LOST)) {
            if (!followingTruck.sendCurrentStatus()) {
                spdlog::warn("Failed to send current status to server.");
            }
            if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                spdlog::warn("Connection lost with server.");
                next_state = CONNECTION_LOST;
            }
        }
        sleep(period); 
    }
    spdlog::debug("Exit send current status.");

    pthread_exit(NULL); 
}

//
void* check_obstacle(void* arg) {
    pthread_detach(pthread_self()); 

    spdlog::debug("Check Obstacle");
    // wait to change state
    while (get_current_state() != NORMAL_OPERATION);
    //
    int period = env_get_int("SENSOR_STATUS_PERIOD", 1);
    while (true) {
        if (get_current_state() == IDLE) break;
        if ((get_current_state() != LEAVING) && (get_current_state() != CONNECTION_LOST)) {
            if ((followingTruck.obstacleAvoidance() == 2) || (request == OBSTACLE_DETECT))  {
                while (!followingTruck.alertObstacleDetection()) { 
                    if (followingTruck.getRetryTimes() >= MAX_RESEND_MESS) {
                        spdlog::error("Connection lost with server.");
                        next_state = CONNECTION_LOST;
                        break;
                    }
                    sleep(period);
                }
            }
            sleep(period);
        }
    }
    spdlog::debug("Exit check obstacle.");

    pthread_exit(NULL); 
}

//
void* check_lead_message(void* arg) {
    pthread_detach(pthread_self()); 

    spdlog::debug("Check LEADING notification");
    // wait to change state
    while (get_current_state() != NORMAL_OPERATION);
    //
    int period = env_get_int("LISTEN_LEADING_PERIOD", 1);
    while (true) {
        if (get_current_state() == IDLE) break;
        std::string leading_mess = followingTruck.listenForLeading();

        if (leading_mess == "emergency")      leadingNoti = BRAKING;
        else if (leading_mess == "slow_down") leadingNoti = SLOWING;
        else if (leading_mess == "speed_up")  leadingNoti = SPEEDING;
        else                                  leadingNoti = NORMAL;

        sleep(period);
    } 
    spdlog::debug("Exit check notification.");

    pthread_exit(NULL); 
}

//
void* check_broadcast_message(void* arg) {
    pthread_detach(pthread_self()); 

    spdlog::debug("Check BROADCAST notification");
    // wait to change state
    while (get_current_state() != NORMAL_OPERATION);
    //
    int period = env_get_int("LISTEN_LEADING_PERIOD", 1);
    while (true) {
        if (get_current_state() == IDLE) break;
        std::string leading_mess = followingTruck.listenForBroadcast();

        if (leading_mess == "emergency")      leadingNoti = BRAKING;
        else if (leading_mess == "slow_down") leadingNoti = SLOWING;
        else if (leading_mess == "speed_up")  leadingNoti = SPEEDING;
        else                                  leadingNoti = NORMAL;

        sleep(period);
    } 
    spdlog::debug("Exit check notification.");

    pthread_exit(NULL); 
}

//
void* emergency_brake(void* arg) {
    pthread_detach(pthread_self()); 

    // wait to change state
    while (get_current_state() != EMERGENCY_BRAKE);
    //
    while (get_current_state() == EMERGENCY_BRAKE) { 
        spdlog::warn("Emergency Brake");
        followingTruck.emergencyBrake();
        sleep(1);
    }
    pthread_exit(NULL);
}

//
void load_environment(std::string env_file) {
    std::string env_file_string = env_file;

    if (env_file_string.empty())
        env_file_string = SERVICE_ENV_FILE_DEFAULT;
    env_init(env_file_string.c_str());

    std::cout << "Project Name: " << env_get("PROJECT_NAME", "Unknown") << std::endl;
}
