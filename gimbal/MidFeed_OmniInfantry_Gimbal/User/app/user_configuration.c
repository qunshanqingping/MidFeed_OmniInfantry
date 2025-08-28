//
// Created by honor on 25-8-28.
//
#include "user_configuration.h"

bool User_Debug_handle(void) {
#ifdef DEBUG_MODE
    return false;
#endif
}