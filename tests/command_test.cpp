#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/communication/platoon_server.h"
#include "../src/control/trucks_manager.h"
#include "../src/control/event_manager.h"
#include "../src/control/monitor.h"
#include "../src/app.h"
#include "../src/communication/comm_msg.h"

/******************************************************************************************/
/* Validation tests */
/******************************************************************************************/

// Test 1: Parse Payload
TEST(TruckMessageTest, ParsePayloadAndGetValues) {
    // Input JSON payload
    std::string testPayload = R"({
        "truck_id": "TRUCK_123",
        "cmd": "START",
        "contents": {
            "location": {"lat": 37.7749, "lon": -122.4194},
            "distance": {"front": 100.0, "back": 50.0, "lead": 30.0},
            "speed": 80.5,
            "status": "moving",
            "brake_force": 0.0,
            "error_code": "E_NONE"
        },
        "msg_id": "TEST_MSG_001",
        "timestamp": "1737670780"
    })";

    // Create a TruckMessage object
    TruckMessage message(testPayload);

    // Verify the parsed values
    EXPECT_EQ(message.getTruckID(), "TRUCK_123");
    EXPECT_EQ(message.getCommand(), "START");
    EXPECT_DOUBLE_EQ(message.getLatitude(), 37.7749);
    EXPECT_DOUBLE_EQ(message.getLongitude(), -122.4194);
    EXPECT_DOUBLE_EQ(message.getFrontDistance(), 100.0);
    EXPECT_DOUBLE_EQ(message.getBackDistance(), 50.0);
    EXPECT_DOUBLE_EQ(message.getLeadDistance(), 30.0);
    EXPECT_DOUBLE_EQ(message.getSpeed(), 80.5);
    EXPECT_EQ(message.getStatus(), "moving");
    EXPECT_DOUBLE_EQ(message.getBrakeForce(), 0.0);
    EXPECT_EQ(message.getErrorCode(), "E_NONE");
    EXPECT_EQ(message.getMessageID(), "TEST_MSG_001");
    EXPECT_EQ(message.getTimestamp(), "1737670780");
}

// Test 2: Generate UUID
TEST(TruckMessageTest, GenerateUUID) {
    std::string uuid1 = TruckMessage::generateUUID();
    std::string uuid2 = TruckMessage::generateUUID();
    EXPECT_NE(uuid1, uuid2); // UUIDs should be unique
    EXPECT_EQ(uuid1.length(), 36); // UUID length should be 36 characters
}

// Test 3: Generate Truck ID
TEST(PlatoonServerTest, GenerateTruckID) {
    PlatoonServer server(8080);

    std::string id1 = server.generateTruckID();
    std::string id2 = server.generateTruckID();

    EXPECT_FALSE(id1.empty()) << "Truck ID should not be empty.";
    EXPECT_FALSE(id2.empty()) << "Truck ID should not be empty.";
    EXPECT_NE(id1, id2) << "Truck IDs should be unique.";

    EXPECT_EQ(id1.substr(0, 4), "TR00") << "Truck ID should start with 'TR00'.";
    EXPECT_EQ(id2.substr(0, 4), "TR00") << "Truck ID should start with 'TR00'.";
}

// Test 4:

// Test 5:

/******************************************************************************************/
/* Defect tests */
/******************************************************************************************/

// Test 1: Invalid Authentication Key
TEST(AuthenticationDefectTest, InvalidKey) {
    PlatoonServer server(8080);
    std::string invalidKey = "INVALID_KEY_123";
    std::string expectedToken = "VALID_AUTH_TOKEN";
    
    // Assert that authentication fails with an invalid key
    EXPECT_FALSE(server.authenticateTruck(invalidKey, expectedToken)) 
        << "Truck should not authenticate with an invalid key.";
}

// Test 2: Duplicate Truck ID
TEST(TruckManagerDefectTest, DuplicateTruckID) {
    TruckManager& manager = TruckManager::getInstance();
    std::string truckID = "TRUCK_001";
    int firstSocket = 1001;
    int secondSocket = 1002;

    // Add a truck with the ID
    manager.addTruck(truckID, firstSocket);

    // Attempt to add another truck with the same ID
    manager.addTruck(truckID, secondSocket);

    // Check that the TruckManager did not overwrite the original truck
    int assignedSocket = manager.getSocketId(truckID);
    EXPECT_EQ(assignedSocket, firstSocket) 
        << "TruckManager should not overwrite existing truck with duplicate ID.";
}

// Test 3: Invalid Command in Truck Event FSM
TEST(TruckEventFSMTest, HandleInvalidCommandInHandleEvent) {
    TruckEventFSM stateMachine;
    try {
        stateMachine.setState("idle");
    } catch (const std::invalid_argument& e) {
        EXPECT_EQ(std::string(e.what()), "Invalid command name: idle");
    }
}

// Test 4:

// Test 5:

/******************************************************************************************/
/* Implement a component/block in a test driven development (TDD) approach */
/******************************************************************************************/
/* Testing the AppStateMachine - State Transition to Normal Operation
*
* 1. Test the transition from the INIT -> NORMAL
* 2. Test the transition from the NORMAL -> EMERGENCY
* 3. Test the transition from the EMERGENCY -> NORMAL
*   
* Revise: Implement/revise condition for transition NORMAL -> EMERGENCY
*
* Approach:
* - Create test case
* - Run test case
* - Verify the result
* - Refactor code
* - Repeat
*
/******************************************************************************************/


TEST(AppStateMachineTest, SwitchToNormalOperation) {
    AppStateMachine fsm;
    TruckEventFSM truckEventFSM;
    // Initially, the state should be INIT
    EXPECT_EQ(fsm.getCurrentState(), AppStateMachine::State::INIT);

    // switch to normal operation
    fsm.switchToNormal();
    EXPECT_EQ(fsm.getCurrentState(), AppStateMachine::State::NORMAL_OPERATION);

    // Found an emergency
    truckEventFSM.setEmergencyEnabled(true);

    // if found emergency, switch to EMERGENCY
    if (truckEventFSM.isEmergencyEnabled()) {
        fsm.switchToEmergency();
        EXPECT_EQ(fsm.getCurrentState(), AppStateMachine::State::EMERGENCY);
    } else {
        EXPECT_EQ(fsm.getCurrentState(), AppStateMachine::State::NORMAL_OPERATION);    
    }
}