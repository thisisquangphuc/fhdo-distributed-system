/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 14 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */

#pragma once

#include <string>
#include <list>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Check whether json object contains the list of keys.
 *
 * @param j json object
 * @param keys key list. for example: {"a", "b", "c"}
 * @return true if all keys are present in the json object,
 * @return false otherwise
 */
inline bool json_contains(const json &j, std::list<std::string> keys)
{
    for (auto key : keys)
    {
        if (!j.contains(key))
            return false;
    }
    return true;
}
