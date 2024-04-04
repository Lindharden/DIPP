#ifndef ERRORS_H
#define ERRORS_H

typedef enum ERROR_CODE
{
    SUCCESS = 0,
    
    PARSING_ERROR = 100,                            // General error in parsing of capture instructions
    PARSING_ERROR_CAMERA_TYPE_INVALID = 101,        // Invalid camera type or camera type implementation missing
    PARSING_ERROR_CAMERA_ID_MISSING = 102,          // Camera id is missing from capture instructions
    PARSING_ERROR_NUM_IMAGES_INVALID = 103,         // Number of images to capture is 0

    CAPTURE_ERROR = 200,                            // General error during image capture
    CAPTURE_ERROR_NO_CAMERAS = 201,                 // No cameras connected
    CAPTURE_ERROR_CAMERA_NOT_FOUND = 202,           // Camera with id CAMERA_ID not found

    MESSAGE_QUEUE_ERROR = 300,                      // General error in message queue
    MESSAGE_QUEUE_ERROR_MEMORY_SPACE_FAILURE = 301, // Unable to create shared memory space
    MESSAGE_QUEUE_ERROR_INSERT_DATA_FAILURE = 302   // Unable to insert data into memory space
} ERROR_CODE;

#endif