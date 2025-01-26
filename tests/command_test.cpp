#include <gtest/gtest.h>
#include "../src/communication/platoon_server.h"  // Your project header
#include "../src/communication/comm_msg.h"  // Your project header

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


// Test 4: UUID Generation
TEST(TruckMessageTest, GenerateUUID) {
    std::string uuid1 = TruckMessage::generateUUID();
    std::string uuid2 = TruckMessage::generateUUID();
    EXPECT_NE(uuid1, uuid2); // UUIDs should be unique
    EXPECT_EQ(uuid1.length(), 36); // UUID length should be 36 characters
}