/*
 * struct to pass data to the toggle cog driver
 */

#ifndef MAILBOX_STRUCT
  #define MAILBOX_STRUCT 1
  #define IO_DRIVER_STACK_SIZE 16

  typedef struct cog_mailbox_st {
    uint8_t inputPort[64];                  // Inputs from 0 to 63.
    uint8_t outputPort[64];                 // Outputs from 0 to 63.
    uint8_t lampState[64];                  // Light definition from light 0 to light 63.
    uint32_t loopTicks;                     // How long did the loop took.
  } ioDriverMailbox_t;
#endif

