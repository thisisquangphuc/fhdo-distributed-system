#ifndef TRUCKMESSAGE_H
#define TRUCKMESSAGE_H

#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <uuid/uuid.h>
#include <chrono>
#include <ctime>

using json = nlohmann::json;

class TruckMessage {
    private:
        nlohmann::json payload;
        nlohmann::json outgoingPayload;
        std::string truck_id;

        // Returns default payload structure
    nlohmann::json defaultPayload() const {
        return {
            {"truck_id", ""},
            {"cmd", ""},
            {"contents", {
                {"location", {{"lat", 0.0}, {"lon", 0.0}}},
                {"distance", {{"front", 0}, {"back", 0}, {"lead", 0}}},
                {"speed", 0},
                {"status", "normal"},
                {"brake_force", 0.0},
                {"error_code", ""}
            }},
            {"msg_id", generateUUID()},
            {"timestamp", generateTimestamp()}
        };
    }

    public:
        // Constructor
        TruckMessage(std::string payload) {
            // parse 
            if (!parsePayload(payload)) {
                throw std::invalid_argument("Invalid payload");
            }
            truck_id = getTruckID();
        }

        TruckMessage() {
        }

        // Parse JSON string
        bool parsePayload(const std::string& jsonString) {
            try {
                payload = nlohmann::json::parse(jsonString);
                return true;
            } catch (const nlohmann::json::exception& e) {
                std::cerr << "JSON Parse Error: " << e.what() << std::endl;
                return false;
            }
        }

        // Serialize JSON
        std::string serialize(bool pretty=false) const {
            if (pretty) {
                return payload.dump(4);
            } else {
                return payload.dump();
            }
        }

        // Getters
        std::string getTruckID() const { return payload.value("truck_id", ""); }
        std::string getCommand() const { return payload.value("cmd", ""); }
        double getLatitude() const { return payload["contents"]["location"].value("lat", 0.0); }
        double getLongitude() const { return payload["contents"]["location"].value("lon", 0.0); }
        double getFrontDistance() const { return payload["contents"]["distance"].value("front", 0.0); }
        double getBackDistance() const { return payload["contents"]["distance"].value("back", 0.0); }
        double getLeadDistance() const { return payload["contents"]["distance"].value("lead", 0.0); }
        double getSpeed() const { return payload["contents"].value("speed", 0.0); }
        std::string getStatus() const { return payload["contents"].value("status", "normal"); }
        double getBrakeForce() const { return payload["contents"].value("brake_force", 0.0); }
        std::string getErrorCode() const { return payload["contents"].value("error_code", ""); }
        std::string getMessageID() const { return payload.value("msg_id", ""); }
        std::string getTimestamp() const { return payload.value("timestamp", ""); }
        std::string getAuthenKey() const { return payload["contents"].value("auth_key", "");}
        // Setters
        void setTruckID(const std::string& id) { outgoingPayload["truck_id"] = id;  }
        void setCommand(const std::string& cmd) { outgoingPayload["cmd"] = cmd; }
        void setLocation(double lat, double lon) {
            outgoingPayload["contents"]["location"]["lat"] = lat;
            outgoingPayload["contents"]["location"]["lon"] = lon;
        }
        void setDistances(double front, double back, double lead) {
            outgoingPayload["contents"]["distance"]["front"] = front;
            outgoingPayload["contents"]["distance"]["back"] = back;
            outgoingPayload["contents"]["distance"]["lead"] = lead;
        }
        void setSpeed(double speed) { outgoingPayload["contents"]["speed"] = speed; }
        void setStatus(const std::string& status) { outgoingPayload["contents"]["status"] = status; }
        void setBrakeForce(double brakeForce) { outgoingPayload["contents"]["brake_force"] = brakeForce; }
        void setErrorCode(const std::string& errorCode) { outgoingPayload["contents"]["error_code"] = errorCode; }
        void setMessageID(const std::string& msgID) { outgoingPayload["msg_id"] = msgID; }
        void setTimestamp(const std::string& timestamp) { outgoingPayload["timestamp"] = timestamp; }
        void setAuthenKey(const std::string& authKey) { outgoingPayload["contents"]["auth_key"] = authKey; }
        // void setTruckID(const std::string& id) { this->truck_id = id; }
        void setContents(const nlohmann::json& contents) { outgoingPayload["contents"] = contents; }

        // Build payload from outgoing payload with defaults for unset fields
        std::string buildPayload() {
            nlohmann::json fullPayload = defaultPayload(); // Start with defaults
            fullPayload.merge_patch(outgoingPayload);      // Merge with outgoing data
            return fullPayload.dump();
        }

        // Build payload from outgoing payload with contents in json
        std::string buildPayload(const nlohmann::json& contents) {
            nlohmann::json fullPayload = defaultPayload();
            outgoingPayload["truck_id"] = "LEADING_001";
            outgoingPayload["contents"] = contents;
            fullPayload.merge_patch(outgoingPayload);
            return fullPayload.dump();
        }

        // Generate UUID
        static std::string generateUUID() {
            uuid_t uuid;
            char uuidStr[37];
            uuid_generate(uuid);
            uuid_unparse(uuid, uuidStr);
            return std::string(uuidStr);
        }

        // Generate Timestamp in epoch time 
        static std::time_t generateTimestamp() {
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            return now_c;
        }

        static std::string generateDateTime() {
            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            char buffer[100];
            strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&now_c));
            return std::string(buffer);
        }

        // Debug Print
        void printPayload() const {
            std::cout << payload.dump(4) << std::endl;
        }

        // Send payload over a TCP connection
        void sendPayload(int socket) const {
            std::string data = serialize();
            // send(socket, data.c_str(), data.size(), 0);
        }

};

#endif // TRUCKMESSAGE_H
