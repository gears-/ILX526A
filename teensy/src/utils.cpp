#include "Arduino.h"

/*
 * Function: read_exposure 
 * Description: makes sure the exposure command is well-formed, and reads it
 *
 */
int8_t read_exposure(uint8_t *cmd_buffer) {
    char ch = 0;
    uint8_t idx = 1;
    uint8_t fac = 0;
    ch = cmd_buffer[idx];

    // 'm' or 's' comes after the exposure command
    // If it is not the case, then exit and notice the host the command failed
    switch(ch) {
        case 's':
            fac = FACTOR_SECONDS;
            break;
        case 'm':
            fac = FACTOR_MILLISECONDS;
            break;
        default:
            return EXIT_ERROR;
            break;
    } 
}

