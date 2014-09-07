/* Copyright (C) 2014 Krzysztof Stachowiak */

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "error.h"

static bool err_state_arr[ERR_MODULES_COUNT];
static char *err_msg_arr[ERR_MODULES_COUNT];

void err_reset(void)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
        err_state_arr[i] = false;
        err_msg_arr[i] = NULL;
    }
}

void err_set(enum ErrModule module, char *message)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
        if (err_state_arr[i]) {
			LOG_ERROR("Error in the error system. Too embarrassed to continue.\n");
            exit(2);
        }
    }

    err_state_arr[module] = true;
    err_msg_arr[module] = message;
}

bool err_state(void)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
        if (err_state_arr[i]) {
            return true;
        }
    }
    return false;
}

char *err_msg(void)
{
    int i;
    for (i = 0; i < ERR_MODULES_COUNT; ++i) {
        if (err_state_arr[i]) {
            return err_msg_arr[i];
        }
    }

    return NULL;
}

