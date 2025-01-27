#include <gtest/gtest.h>
#include <iostream>
#include <cstring>
#include <chrono>

#include "../src/communication/platoon_client.h"
#include "../src/communication/platoon_server.h"
#include "../src/control/following_truck.h"
#include "../src/utils/config.h"

// 
TEST(FollowingTruckTest, GetCommandFromLeading) {
    FollowingTruck followingTruck;

    json cmd = {
        {"truck_id", "LEADING_001"},
        {"cmd", "emergency"},
        {"contents", {
            {"speed", 5.0},
            {"brake_force", 0.8}
        }},
        {"msg_id", "TEST_MSG_001"},
        {"timestamp", "1737670780"}
    };


    EXPECT_EQ(followingTruck.processCommands(cmd.dump()), "emergency");
    EXPECT_EQ(followingTruck.getTruckStatus(), "emergency");
    EXPECT_DOUBLE_EQ(followingTruck.getBrakeForce(), 0.8);
    EXPECT_DOUBLE_EQ(followingTruck.getRefSpeed(), 5.0);
}


//
TEST(FollowingTruckTest, TruckInit) {
    FollowingTruck followingTruck;

    EXPECT_DOUBLE_EQ(followingTruck.getBrakeForce(), 0.0);
    EXPECT_DOUBLE_EQ(followingTruck.getRefSpeed(), 0.0);
    EXPECT_GE(followingTruck.getTruckSpeed(), 60.0*0.98);
    EXPECT_LE(followingTruck.getTruckSpeed(), 60.0*1.12);
    EXPECT_DOUBLE_EQ(followingTruck.getTruckLeadDistance(), 0.0);
}

//
//TEST(FollowingTruckTest, UpdateTruckMessage) {
//    FollowingTruck followingTruck;
//
//    followingTruck.setTruckID("TRUCK_0001");
//    followingTruck.setTruckSpeed(0.0);
//    followingTruck.setTruckFrontDistance(2.0);
//    followingTruck.setTruckBackDistance(2.1);
//    followingTruck.setTruckLeadDistance(2.2);
//    followingTruck.setTruckLatLoc(51.5918);
//    followingTruck.setTruckLonLoc(7.3421);
//
//    followingTruck.updateCurrentStatus();
//
//    TruckMessage truck_message = followingTruck.getTruckMessage();
//    spdlog::info("{}", truck_message.buildPayload());
//    
//
//    EXPECT_DOUBLE_EQ(truck_message.getFrontDistance(), 2.0);
////    EXPECT_DOUBLE_EQ(truck_message.getBackDistance(), 2.1);
////    EXPECT_DOUBLE_EQ(truck_message.getLeadDistance(), 2.2);
////    EXPECT_DOUBLE_EQ(truck_message.getLatitude(), 51.5918);
////    EXPECT_DOUBLE_EQ(truck_message.getLongitude(), 7.3421);
//}

//
TEST(FollowingTruckTest, TruckSpeedControlWhenBraking) {
    FollowingTruck followingTruck;
  
    followingTruck.setTruckSpeed(20.0);	
    followingTruck.setRefSpeed(10.0);	
    followingTruck.setBrakeForce(0.8);

    followingTruck.updateCurrentStatus();
    followingTruck.updateCurrentStatus();

    EXPECT_DOUBLE_EQ(followingTruck.getBrakeForce(), 0.0);
    EXPECT_DOUBLE_EQ(followingTruck.getRefSpeed(), 0.0);
    EXPECT_GE(followingTruck.getTruckSpeed(), 10.0*0.98);
    EXPECT_LE(followingTruck.getTruckSpeed(), 10.0*1.12);
}



//
//
TEST(CommandDefectTest, CommandParseInvalid) {
    FollowingTruck followingTruck;

    json cmd = {
      {"cmd", "á"},
      {"contents", {
          {"speed", "hi"}
      }}
    };

    EXPECT_THROW(followingTruck.processCommands(cmd), nlohmann::json::exception);
}

//
//
TEST(FOllowingTruckTest, SendRequestWhenNoConnection) {
    FollowingTruck followingTruck;

    EXPECT_FALSE(followingTruck.askToJoinPlatoon());
    EXPECT_FALSE(followingTruck.joiningPlatoon());
    EXPECT_FALSE(followingTruck.leavingPlatoon());
    EXPECT_EQ(followingTruck.getRetryTimes(), 3);

    followingTruck.resetRetryCounter();
    EXPECT_EQ(followingTruck.getRetryTimes(), 0);
}



//
TEST(PlatoonConnectionTest, CreateSocket) {
    int tcp_port = 8080;
    int udp_port = 59059;
    std::string host_ip = "127.0.0.1";
    std::string error_message = "";

    PlatoonServer platoonServer(8080);
    platoonServer.setIP(host_ip);
    EXPECT_TRUE(platoonServer.startServer(error_message));


    PlatoonClient platoonClient;

    EXPECT_TRUE(platoonClient.startClient(tcp_port, udp_port, host_ip, error_message));
    EXPECT_EQ(error_message, "\n");
}

int main() {
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
