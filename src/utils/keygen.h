/*
 * Author : Phuc Le
 *
 * Created on Sun Jan 19 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

// Generate pre-shared key for given number of trucks 
// Keys should use info of time, session key in env,... and make sure keys are unique.
// Retuen a list of keys in Cpp

#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include "env.h"
#include "config.h"


inline std::vector<std::string> generate_pre_shared_key(int num_trucks) {
    std::vector<std::string> keys;
    const char* session_key = env_get("SESSION_KEY", DEFAULT_SESSION_KEY); // Retrieve session key from environment

    for (int i = 0; i < num_trucks; ++i) {
        std::ostringstream key_stream;
        std::time_t current_time = std::time(nullptr); // Get current time
        key_stream << session_key << "_" << current_time << "_" << i; // Create unique key
        
        // Hash the key string
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(key_stream.str().c_str()), key_stream.str().length(), hash);

        // Convert hash to hex string
        std::ostringstream hex_stream;
        for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
            hex_stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[j]);
        }
        keys.push_back(hex_stream.str()); // Add hashed key to the list
    }

    return keys; // Return the list of keys
}