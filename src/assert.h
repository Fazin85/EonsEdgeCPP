#pragma once

#include <stdexcept>
#include <string>

#ifdef _DEBUG
#define EON_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                throw std::runtime_error("Assertion failed: " #condition " - " message); \
            } \
        } while(0)
#else
#define EON_ASSERT(condition, message) ((void)0)
#endif