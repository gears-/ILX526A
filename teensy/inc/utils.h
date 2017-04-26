#ifndef _UTILS_H_
#define _UTILS_H_

// Define all of the different regular expressions
#define REGEX_EXPOSURE "^e[ms][%d]+[%.]?[%d]*$"
#define REGEX_NUMBER "[%d]+[%.]?[%d]*$"

#define EXIT_ERROR -1

enum class CMD_PARSE_RET {
    EXPOSURE_MATCH = 1,
    NO_MATCH = 0 
};

enum class FACTOR {
    SECONDS = 1,
    MILLISECONDS = 1000
};

CMD_PARSE_RET cmd_parse(char *cmd_buffer);
uint32_t read_exposure(char *cmd_buffer);

#endif
