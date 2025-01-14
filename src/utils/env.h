/*
 * Author : Phuc Le
 *
 * Created on Tue Jan 14 2025
 *
 * Copyright (c) 2025 Acadamic Purpose. All rights reserved.
 */


#pragma once

#ifndef UTILS_ENV_H
#define UTILS_ENV_H

#include <stdlib.h>
#include "libs/dotenv/dotenv.h"
#include "utils/utils.h"

#ifdef __cplusplus
extern "C"
{
#endif

    inline void env_init(const char *path)
    {
        env_load(path, false);
    }

    inline const char *env_get(const char *name, const char *def)
    {
        char *val = getenv(name);
        if (val == NULL || val[0] == '\0')
            return def;
        return val;
    }

    inline int env_get_int(const char *name, int def)
    {
        char *val = getenv(name);
        if (val == NULL || val[0] == '\0')
            return def;
        return atoi(val);
    }

    inline bool env_get_bool(const char *name)
    {
        char *val = getenv(name);
        if (val == NULL || val[0] == '\0')
            return false;

        return stob(val);
    }

#ifdef __cplusplus
}
#endif

#endif
