/**
 * File that contains any status codes
 * to be used in the program
 */
#pragma once

typedef enum _StatusCode
{
    OK = 0,
    InvalidParameters = -1,
    CouldNotOpenFile = -2,
    FileLengthIsIncorrect = -3,
    FileFormatIsIncorrect = -4,
    FileFormatNotSupported = -5,
    CouldNotWriteToFile = -6,
} StatusCode;
