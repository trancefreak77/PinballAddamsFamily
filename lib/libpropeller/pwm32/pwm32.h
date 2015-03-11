
#ifndef libpropeller_pwm32_h__
#define libpropeller_pwm32_h__

#include <stdint.h>


#include <propeller.h>

#ifdef __GNUC__
#define INLINE__ static inline
#define Yield__() __asm__ volatile( "" ::: "memory" )
#define PostEffect__(X, Y) __extension__({ int tmp__ = (X); (X) = (Y); tmp__; })
#else
#define INLINE__ static
static int tmp__;
#define PostEffect__(X, Y) (tmp__ = (X), (X) = (Y), tmp__)
#define Yield__()
#endif

namespace libpropeller {

/** Run up to 32 channels of low speed PWM (up to ~10kHz@80MHz clock).
 * 
 * No external hardware beyond the PWM devices is needed. Works with H bridges,
 * LEDs, servos, and anything else that takes a repeating PWM signal.
 * 
 * Copyright (c) 2013 Kenneth Bedolla  (libpropeller@kennethbedolla.com)
 */
class PWM32 {
public:
    static const int Resolution = 8200; //8.2 uS @80MHz

    PWM32() {
        command = 0;
        argument_0 = 0;
        argument_1 = 0;
        argument_2 = 0;
        cog = 0;
    }

    /** Start a cog with the PWM runner.
     */
    void Start(void) {
        Stop();

        /*
        volatile void * asm_reference = NULL;
        __asm__ volatile ( "mov %[asm_reference], #PWM32_Entry \n\t"
                : [asm_reference] "+r" (asm_reference));
        cog = cognew(_load_start_pwm32_cog, &command) + 1;
         */


        cog = cognew((int) (&(*(int *) dat())), (int) (&command)) + 1;
    }

    /** Stop the cog, if running.
     */
    void Stop(void) {
        if (cog != 0) {

            cogstop(cog - 1);
            cog = 0;
        }
    }

    /** Output a PWM wave with a calculated duty.
     * 
     * @param pin        The I/O pin to use (0..31)
     * @param duty_cycle The duty cycle (0..100) to use. 0 is off, 100 is full on.
     * @param period     The period (in uS) of the cycle.
     */
    void Duty(int pin, int duty_cycle, int period) {
        if (period != 0) {
            if (duty_cycle == 0) {
                Dutymode(pin, 2);
                Statemode(pin, 1);
            }
            if (duty_cycle == 100) {
                Dutymode(pin, 1);
                Statemode(pin, 1);
            }
            if ((duty_cycle != 0) && (duty_cycle != 100)) {
                int Baseperiod = ((period * 1000) / Resolution);
                int Ton = ((duty_cycle * Baseperiod) / 100);
                int Toff = (Baseperiod - Ton);
                if (Ton == 0) {
                    Dutymode(pin, 1);
                }
                if (Toff == 0) {
                    Dutymode(pin, 2);
                }
                if ((Ton == 0) && (Toff == 0)) {
                    Statemode(pin, 0);
                }
                if ((Ton != 0) || (Toff != 0)) {
                    InternalPWM(pin, Ton, Toff);
                    Dutymode(pin, 0);
                }
            }
        } else {
            Statemode(pin, 0);
        }
    }

    /** Output a servo compatible pulse with a period of 20ms.
     * 
     * @param pin         The I/O pin to use (0..31)
     * @param pulse_width The width of the servo pulse in uS. Can be any value,
     *                          but most servos require a range (1000..2000)
     */
    void Servo(int pin, int pulse_width) {
        int Ton, Toff;
        if (pulse_width != 0) {
            Ton = ((pulse_width * 1000) / Resolution);
            Toff = (((20000 - pulse_width) * 1000) / Resolution);
            InternalPWM(pin, Ton, Toff);
        } else {
            Statemode(pin, 0);
        }
    }

    /** Output a series of square waves with a high time of on_time
     * microseconds, and a period of (on_time + low_time) microseconds.
     * 
     * @param pin       The I/O pin to use (0..31)
     * @param on_time   The duration of the high time, in uS.
     * @param off_time  The duration of the low time, in uS.
     */
    void PWM(int pin, int on_time, int off_time) {
        InternalPWM(pin, (on_time * 1000) / Resolution, (off_time * 1000) / Resolution);
    }

private:

    static const int Updatetontoff = 1;
    static const int Io_state = 2;
    static const int Dutyoverride = 3;
    static const int Syncphase = 4;
    static const int Duty_default = 0;
    static const int Duty_100 = 1;
    static const int Duty_0 = 2;
    static const int Disablepin = 0;
    static const int Enablepin = 1;

    int cog;

    //These four variables must remain in the same order.
    volatile int command, argument_0, argument_1, argument_2;

    void Statemode(int Pin, int State) {
        argument_0 = Pin;
        argument_1 = State;
        command = Io_state;
        while (!(command == 0)) {
            Yield__();
        }
    }

    void Dutymode(int Pin, int Mode) {
        argument_0 = Pin;
        argument_1 = Mode;
        command = Dutyoverride;
        while (!(command == 0)) {
            Yield__();
        }

    }


    // Phasesync is untested...

    void Phasesync(int Pin1, int Pin2, int Phase) {
        Phase = ((Phase * 1000) / Resolution);
        argument_0 = Pin1;
        argument_1 = Pin2;
        argument_2 = Phase;
        command = Syncphase;
        while (!(command == 0)) {
            Yield__();
        }
    }

    /**
     * 
     * @param Pin
     * @param Ontime in units of resolution (decimal, 8.2)
     * @param Offtime
     * @return 
     */
    void InternalPWM(int Pin, int Ontime, int Offtime) {
        if ((Ontime == 0) && (Offtime == 0)) {
            Statemode(Pin, 0);
        }
        if (Ontime == 0) {
            Dutymode(Pin, 2);
            Statemode(Pin, 1);
        }
        if (Offtime == 0) {
            Dutymode(Pin, 1);
            Statemode(Pin, 1);
        }
        if ((Ontime == 0) && (Offtime == 0)) {
            Statemode(Pin, 0);
        }
        if ((Ontime != 0) && (Offtime != 0)) {
            argument_0 = Pin;
            argument_1 = (Ontime - 1);
            argument_2 = (Offtime - 1);
            command = Updatetontoff;
            while (!(command == 0)) {
                Yield__();
            }
            Statemode(Pin, 1);
            Dutymode(Pin, 0);
        }
    }

    static volatile uint8_t * dat() {
        static volatile uint8_t data[] = {
            0xf0, 0xe1, 0xbd, 0xa0, 0xf0, 0xd8, 0xbd, 0xa0, 0x04, 0xe0, 0xfd, 0x80, 0xf0, 0xda, 0xbd, 0xa0,
            0x04, 0xe0, 0xfd, 0x80, 0xf0, 0xdc, 0xbd, 0xa0, 0x04, 0xe0, 0xfd, 0x80, 0xf0, 0xde, 0xbd, 0xa0,
            0x01, 0x28, 0xfe, 0x85, 0xf4, 0xe8, 0xb2, 0x6e, 0x34, 0x29, 0xa2, 0xa0, 0x54, 0x29, 0x92, 0xa0,
            0x01, 0x2a, 0xfe, 0x85, 0xf5, 0xea, 0xb2, 0x6e, 0x35, 0x2b, 0xa2, 0xa0, 0x55, 0x2b, 0x92, 0xa0,
            0x01, 0x2c, 0xfe, 0x85, 0xf6, 0xec, 0xb2, 0x6e, 0x36, 0x2d, 0xa2, 0xa0, 0x56, 0x2d, 0x92, 0xa0,
            0x01, 0x2e, 0xfe, 0x85, 0xf7, 0xee, 0xb2, 0x6e, 0x37, 0x2f, 0xa2, 0xa0, 0x57, 0x2f, 0x92, 0xa0,
            0x01, 0x30, 0xfe, 0x85, 0xf8, 0xf0, 0xb2, 0x6e, 0x38, 0x31, 0xa2, 0xa0, 0x58, 0x31, 0x92, 0xa0,
            0x01, 0x32, 0xfe, 0x85, 0xf9, 0xf2, 0xb2, 0x6e, 0x39, 0x33, 0xa2, 0xa0, 0x59, 0x33, 0x92, 0xa0,
            0x01, 0x34, 0xfe, 0x85, 0xfa, 0xf4, 0xb2, 0x6e, 0x3a, 0x35, 0xa2, 0xa0, 0x5a, 0x35, 0x92, 0xa0,
            0x01, 0x36, 0xfe, 0x85, 0xfb, 0xf6, 0xb2, 0x6e, 0x3b, 0x37, 0xa2, 0xa0, 0x5b, 0x37, 0x92, 0xa0,
            0x01, 0x38, 0xfe, 0x85, 0xfc, 0xf8, 0xb2, 0x6e, 0x3c, 0x39, 0xa2, 0xa0, 0x5c, 0x39, 0x92, 0xa0,
            0x01, 0x3a, 0xfe, 0x85, 0xfd, 0xfa, 0xb2, 0x6e, 0x3d, 0x3b, 0xa2, 0xa0, 0x5d, 0x3b, 0x92, 0xa0,
            0x01, 0x3c, 0xfe, 0x85, 0xfe, 0xfc, 0xb2, 0x6e, 0x3e, 0x3d, 0xa2, 0xa0, 0x5e, 0x3d, 0x92, 0xa0,
            0x01, 0x3e, 0xfe, 0x85, 0xff, 0xfe, 0xb2, 0x6e, 0x3f, 0x3f, 0xa2, 0xa0, 0x5f, 0x3f, 0x92, 0xa0,
            0x01, 0x40, 0xfe, 0x85, 0x00, 0x01, 0xb3, 0x6e, 0x40, 0x41, 0xa2, 0xa0, 0x60, 0x41, 0x92, 0xa0,
            0x01, 0x42, 0xfe, 0x85, 0x01, 0x03, 0xb3, 0x6e, 0x41, 0x43, 0xa2, 0xa0, 0x61, 0x43, 0x92, 0xa0,
            0x01, 0x44, 0xfe, 0x85, 0x02, 0x05, 0xb3, 0x6e, 0x42, 0x45, 0xa2, 0xa0, 0x62, 0x45, 0x92, 0xa0,
            0x01, 0x46, 0xfe, 0x85, 0x03, 0x07, 0xb3, 0x6e, 0x43, 0x47, 0xa2, 0xa0, 0x63, 0x47, 0x92, 0xa0,
            0x01, 0x48, 0xfe, 0x85, 0x04, 0x09, 0xb3, 0x6e, 0x44, 0x49, 0xa2, 0xa0, 0x64, 0x49, 0x92, 0xa0,
            0x01, 0x4a, 0xfe, 0x85, 0x05, 0x0b, 0xb3, 0x6e, 0x45, 0x4b, 0xa2, 0xa0, 0x65, 0x4b, 0x92, 0xa0,
            0x01, 0x4c, 0xfe, 0x85, 0x06, 0x0d, 0xb3, 0x6e, 0x46, 0x4d, 0xa2, 0xa0, 0x66, 0x4d, 0x92, 0xa0,
            0x01, 0x4e, 0xfe, 0x85, 0x07, 0x0f, 0xb3, 0x6e, 0x47, 0x4f, 0xa2, 0xa0, 0x67, 0x4f, 0x92, 0xa0,
            0x01, 0x50, 0xfe, 0x85, 0x08, 0x11, 0xb3, 0x6e, 0x48, 0x51, 0xa2, 0xa0, 0x68, 0x51, 0x92, 0xa0,
            0x01, 0x52, 0xfe, 0x85, 0x09, 0x13, 0xb3, 0x6e, 0x49, 0x53, 0xa2, 0xa0, 0x69, 0x53, 0x92, 0xa0,
            0x01, 0x54, 0xfe, 0x85, 0x0a, 0x15, 0xb3, 0x6e, 0x4a, 0x55, 0xa2, 0xa0, 0x6a, 0x55, 0x92, 0xa0,
            0x01, 0x56, 0xfe, 0x85, 0x0b, 0x17, 0xb3, 0x6e, 0x4b, 0x57, 0xa2, 0xa0, 0x6b, 0x57, 0x92, 0xa0,
            0x01, 0x58, 0xfe, 0x85, 0x0c, 0x19, 0xb3, 0x6e, 0x4c, 0x59, 0xa2, 0xa0, 0x6c, 0x59, 0x92, 0xa0,
            0x01, 0x5a, 0xfe, 0x85, 0x0d, 0x1b, 0xb3, 0x6e, 0x4d, 0x5b, 0xa2, 0xa0, 0x6d, 0x5b, 0x92, 0xa0,
            0x01, 0x5c, 0xfe, 0x85, 0x0e, 0x1d, 0xb3, 0x6e, 0x4e, 0x5d, 0xa2, 0xa0, 0x6e, 0x5d, 0x92, 0xa0,
            0x01, 0x5e, 0xfe, 0x85, 0x0f, 0x1f, 0xb3, 0x6e, 0x4f, 0x5f, 0xa2, 0xa0, 0x6f, 0x5f, 0x92, 0xa0,
            0x01, 0x60, 0xfe, 0x85, 0x10, 0x21, 0xb3, 0x6e, 0x50, 0x61, 0xa2, 0xa0, 0x70, 0x61, 0x92, 0xa0,
            0x01, 0x62, 0xfe, 0x85, 0x11, 0x23, 0xb3, 0x6e, 0x51, 0x63, 0xa2, 0xa0, 0x71, 0x63, 0x92, 0xa0,
            0x01, 0x64, 0xfe, 0x85, 0x12, 0x25, 0xb3, 0x6e, 0x52, 0x65, 0xa2, 0xa0, 0x72, 0x65, 0x92, 0xa0,
            0x01, 0x66, 0xfe, 0x85, 0x13, 0x27, 0xb3, 0x6e, 0x53, 0x67, 0xa2, 0xa0, 0x73, 0x67, 0x92, 0xa0,
            0x74, 0xe1, 0xbd, 0xa0, 0x75, 0xe1, 0xbd, 0x68, 0x76, 0xe1, 0xbd, 0x68, 0x77, 0xe1, 0xbd, 0x68,
            0x78, 0xe1, 0xbd, 0x68, 0x79, 0xe1, 0xbd, 0x68, 0x7a, 0xe1, 0xbd, 0x68, 0x7b, 0xe1, 0xbd, 0x68,
            0x7c, 0xe1, 0xbd, 0x68, 0x7d, 0xe1, 0xbd, 0x68, 0x7e, 0xe1, 0xbd, 0x68, 0x7f, 0xe1, 0xbd, 0x68,
            0x80, 0xe1, 0xbd, 0x68, 0x81, 0xe1, 0xbd, 0x68, 0x82, 0xe1, 0xbd, 0x68, 0x83, 0xe1, 0xbd, 0x68,
            0x84, 0xe1, 0xbd, 0x68, 0x85, 0xe1, 0xbd, 0x68, 0x86, 0xe1, 0xbd, 0x68, 0x87, 0xe1, 0xbd, 0x68,
            0x88, 0xe1, 0xbd, 0x68, 0x89, 0xe1, 0xbd, 0x68, 0x8a, 0xe1, 0xbd, 0x68, 0x8b, 0xe1, 0xbd, 0x68,
            0x8c, 0xe1, 0xbd, 0x68, 0x8d, 0xe1, 0xbd, 0x68, 0x8e, 0xe1, 0xbd, 0x68, 0x8f, 0xe1, 0xbd, 0x68,
            0x90, 0xe1, 0xbd, 0x68, 0x91, 0xe1, 0xbd, 0x68, 0x92, 0xe1, 0xbd, 0x68, 0x93, 0xe1, 0xbd, 0x68,
            0xf0, 0xe8, 0xbf, 0xa0, 0xec, 0xe0, 0xbd, 0x0a, 0x08, 0x00, 0x68, 0x5c, 0x01, 0xe0, 0x7d, 0x6e,
            0xb5, 0x00, 0x68, 0x5c, 0x02, 0xe0, 0x7d, 0x6e, 0xc0, 0x00, 0x68, 0x5c, 0x03, 0xe0, 0x7d, 0x6e,
            0xca, 0x00, 0x68, 0x5c, 0x04, 0xe0, 0x7d, 0x6e, 0xd9, 0x00, 0x68, 0x5c, 0xec, 0xd4, 0x3d, 0x08,
            0x08, 0x00, 0x7c, 0x5c, 0xed, 0xe2, 0xbd, 0x08, 0xee, 0xe4, 0xbd, 0x08, 0x34, 0xe3, 0xfd, 0x80,
            0xf1, 0x7c, 0xbd, 0x54, 0xef, 0xe6, 0xbd, 0x08, 0x20, 0xe2, 0xfd, 0x80, 0xf1, 0x7a, 0xbd, 0x54,
            0xec, 0xd4, 0x3d, 0x08, 0xf2, 0x00, 0xbc, 0xa0, 0xf3, 0x00, 0xbc, 0xa0, 0x08, 0x00, 0x7c, 0x5c,
            0xed, 0xe2, 0xbd, 0x08, 0xee, 0xe4, 0xbd, 0x08, 0x01, 0xe0, 0xfd, 0xa0, 0xf1, 0xe0, 0xbd, 0x2c,
            0xec, 0xd4, 0x3d, 0x08, 0x01, 0xe4, 0x7d, 0x62, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xec, 0x97, 0x68,
            0xf0, 0xec, 0xab, 0x64, 0x08, 0x00, 0x7c, 0x5c, 0xed, 0xe2, 0xbd, 0x08, 0xee, 0xe4, 0xbd, 0x08,
            0xf1, 0xe0, 0xbd, 0xa0, 0x02, 0xe0, 0xfd, 0x2c, 0x09, 0xe0, 0xfd, 0x80, 0xf0, 0xac, 0xbd, 0x54,
            0x00, 0x00, 0x00, 0x00, 0xe9, 0xe0, 0xbd, 0xa0, 0x01, 0xe4, 0x7d, 0x6e, 0xe7, 0xe0, 0xa9, 0xa0,
            0x02, 0xe4, 0x7d, 0x6e, 0xe8, 0xe0, 0xa9, 0xa0, 0xf0, 0x00, 0xbc, 0x58, 0xec, 0xd4, 0x3d, 0x08,
            0x08, 0x00, 0x7c, 0x5c, 0xed, 0xe0, 0xbd, 0x08, 0x14, 0xe1, 0xfd, 0x80, 0xf0, 0xba, 0xbd, 0x50,
            0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xbd, 0xa0, 0xef, 0xd6, 0xbd, 0x08, 0xeb, 0xe0, 0xbd, 0x80,
            0xee, 0xd6, 0xbd, 0x08, 0x14, 0xd7, 0xfd, 0x80, 0xeb, 0xc8, 0xbd, 0x54, 0x00, 0x00, 0x00, 0x00,
            0xf0, 0x00, 0xbc, 0xa0, 0xec, 0xd4, 0x3d, 0x08, 0x08, 0x00, 0x7c, 0x5c, 0xd1, 0x00, 0x00, 0x00,
            0xc9, 0x00, 0x00, 0x00, 0xdd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
            0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
            0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
            0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
            0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08, 0x00,
            0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00,
            0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x08,
            0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };
        return data;
    }

};

}

#endif // libpropeller_pwm32_h__
