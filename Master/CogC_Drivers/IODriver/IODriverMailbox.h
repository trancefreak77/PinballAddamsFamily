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
    uint8_t kickerToInputPortMapping[10];   // Kicker number to input port mapping.
    uint8_t kickerToOutputPortMapping[10];  // Kicker number to output port mapping.
    bool isKickerCoilActive[10];            // Is the coil of a kicker currently actuating.
    uint32_t kickerActivationCnt[10];       // Clock count when a kicker coil was activated.
  } ioDriverMailbox_t;
#endif

