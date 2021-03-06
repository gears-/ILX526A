/* ILX526A Teensy Interface v1.0: a C interface running on the Teensy v3.2 to 
 * obtain data from an ILX526A CCD that is digitized with an ADS803U ADC.
 *
 * Copyright (C) 2018 Pierre-Yves Taunay
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Contact info: https://github.com/pytaunay
 *
 * Source: https://github.com/pytaunay/ILX526A
 */
#include "Arduino.h"
#include <Regexp.h>
#include <type_traits>

#include "utils.h"

MatchState ms;

/*
 * Function: cmd_parse
 * Description: parses a command sent by the host and make sure it is valid
 * Returns different values based on the type of command is sent (e.g. "Exposure", "stop", etc.)
 *
 */
CMD_PARSE_RET cmd_parse(char *cmd_buffer) {
    ms.Target(cmd_buffer);

    // First match the exposure
    char result = ms.Match(REGEX_EXPOSURE); 
    if( result > 0 )
        return CMD_PARSE_RET::EXPOSURE_MATCH;

    // Static cast is necessary to get the correct type of CMD_PARSE_RETURNS enum vals
    return CMD_PARSE_RET::NO_MATCH;
}



/*
 * Function: read_exposure 
 * Description: extracts the exposure value from the command that was sent by the host
 * Validity checks have been done upstream
 *
 */
uint32_t read_exposure(char *cmd_buffer) {
    ms.Target(cmd_buffer);

    // Unit must be parsed before the actual value, as the Regexp methods modify cmd_buffer
    // Parse the unit
    uint8_t unit = cmd_buffer[1];
    Serial.printf("Command: %s\n",cmd_buffer);
    Serial.printf("Unit: %c\n",unit);

    // Find the actual value
    char result = ms.Match(REGEX_NUMBER);
    float val = ((String)ms.GetMatch(cmd_buffer)).toFloat();


    FACTOR fac;
    switch(unit) {
        case 's':
            fac = FACTOR::SECONDS;
            break;
        case 'm':
            fac = FACTOR::MILLISECONDS;
            // If the exposure value the host asked is too low, then set it to a default value of 5 ms
            if( val < 5. )
                val = 5.;

            break;
    }

    // Get the corresponding timer value
    return ((F_BUS / (uint32_t) fac) * val - 1);
}
