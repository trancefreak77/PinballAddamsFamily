{{
             FFDS1        
  (Fast Full-Duplex Serial, 1 cog)

  version 0.9

  Jonathan Dummer (lonesock)

  FFDS1 provides a fast and stable serial interface
  using a single cog.

  Max baudrate = clkfreq / (86 * 2)

  Clock  | MaxBaud | Standard
  -------+---------+---------
  96 MHz | 558_139 | 500_000    <- 6MHz XTAL at 16x PLL
  80 MHz | 465_116 | 460_800    <- 5MHz XTAL at 16x PLL (most common)
  12 MHz |  69_767 |  57_600    <- approx RCFAST
  20 kHz |     116 | hah hah    <- approx RCSLOW

  Bit period is calculated to the nearest 2 clocks.
  So, the bit period should be within 1 clock, or
  12.5 ns at 80 MHz.
}}


CON
  ' Size of the RX buffer in bytes.
  ' No restrictions on size (well, Hub RAM limits [8^)
  ' Does not need to be a power of 2
  BUF_LEN = 256
  ' There is no TX buffer, FFDS1 sends directly from Hub RAM.

  ' Half period must be at least this, otherwise the cog will
  ' sleep for a whole counter cycle (2^32 / clkfreq seconds).
  MIN_HALF_PERIOD = 86 ' 83 fails, 84 seems safe (after a few hours of testing), but I'm paranoid

  ' not used directly, but handy to know
  XON  = 17 ' a.k.a. please resume serial
  XOFF = 19 ' a.k.a. please pause serial  

  
VAR
  ' for interfacing with the PASM cog
  long {++volatile} write_buf_ptr
  ' holder for transmitting a single character
  long {++volatile} send_temp
  ' half a bit period in system clocks
  long {++volatile} half_bit_period
  ' for incoming data
  word {++volatile} rx_head
  word {++volatile} rx_tail
  byte {++volatile} rx_buffer[BUF_LEN]
  ' for tracking the cog IDs
  byte cog     


{========== Main FFDS1 Functions ==========}


PUB Start( rx_pin, tx_pin, rate )
{{
  * Starts the FFDS1 PASM engine running in a new cog.
  > rx_pin : the pin [0..31] for incoming data.
  > tx_pin : the pin [0..31] for outgoing data.
             Must not equal rx_pin.  Use -1 to disable TX.
  > rate   : the initial baud rate
  < Returns TRUE (-1) if the cog started OK
  
  e.g. Start( 31, 30, 460_800 ) ' communicate with the PC over the programming link.
}}
  ' start by stopping, just in case we were already running
  stop        
  ' set up TX
  maskTX := ctra_val := 0
  if tx_pin => 0    
    maskTX := |< tx_pin
    ' use counter A to output bits (NCO single-ended)...if desired
    ctra_val := constant( %00100 << 26 ) | tx_pin   
  ' set up RX
  maskRX := ctrb_val := 0
  if rx_pin => 0
    maskRX := |< rx_pin
    ' use counter B (in LOGIC !A) to see if I need to use the 1st or 2nd sample for RX
    ctrb_val := constant( %10101 << 26 ) | rx_pin        
  ' timing
  SetBaud( rate )
  period_ptr := @half_bit_period       
  ' set up the buffers and pointers
  bytefill( @rx_buffer, 0, BUF_LEN )
  rx_head_ptr := @rx_buffer
  rx_end_ptr := @rx_buffer + BUF_LEN
  rx_head := 0
  rx_tail := 0
  update_head_ptr := @rx_head       
  ' start the new cog
  write_buf_ptr := 1    
  cog := 1 + cognew( @FDS_entry, @write_buf_ptr )
  if cog
    ' wait until the cog is officially running
    repeat
    while write_buf_ptr
    return true

    
PUB Stop
{{
  * Stops the FFDS1 PASM engine, if it is running.
}}
  if cog
    cogstop( cog~ - 1 )


PUB SetBaud( rate ) : got_rate
{{
  * Does a live update of the baud rate in the FFDS1 engine.
  > rate     : desired baud rate of the FFDS1 engine.
  < Returns TRUE if the baud rate was achieved

  e.g. got_desired_rate := SetBaud( 9600 )
}}
  got_rate := SetBaudClock( rate, clkfreq )

  
PUB SetBaudClock( rate, sysclock ) : got_rate
{{
  * Does a live update of the baud rate in the FFDS1 engine.
  > rate     : desired baud rate of the FFDS1 engine.
  > sysclock : the system clock, use CLKFREQ, or provide your own (useful when using RCFAST)
  < Returns TRUE if the baud rate was achieved

  e.g. got_desired_rate := SetBaudClock( 9600, clkfreq )
  e.g. got_desired_rate := SetBaudClock( 9600, actual_sys_clock_freq )
}}
  ' how many clocks per 1/2 bit (pre-round to the nearest integer)
  got_rate := ((sysclock>>1) + (rate>>1)) / rate
  ' clamp the period to the allowable range
  half_bit_period := got_rate #> MIN_HALF_PERIOD
  ' return true if the requested period was >= the allowable limit
  got_rate =>= MIN_HALF_PERIOD 

  
PUB Str( string_ptr )
{{
  * Sends a string over the serial line.
  > string_ptr : a pointer to a 0-terminated string in Hub RAM

  e.g. Str( string( "I don't like Spin's string() command" ) )
}}
  if byte[string_ptr]
    write_buf_ptr := string_ptr | ((strsize( string_ptr ) - 1) << 16)
    repeat
    while write_buf_ptr

    
PUB Tx( char_val )
{{
  * Sends down a single character

  e.g. Tx( 13 ) ' send a CR
}}
  send_temp := char_val  
  write_buf_ptr := @send_temp
  repeat
  while write_buf_ptr

  
PUB TxBuf( buf_ptr, buffer_bytes )
{{
  * Sends a raw buffer over the serial line.
  > buf_ptr      : a pointer to a byte array in Hub RAM.
  > buffer_bytes : the number of bytes to send.

  e.g. TxBuf( @some_buffer, buffer_size_in_bytes )
  e.g. TxBuf( @some_string, strsize( @some_string ) )  
}}
  if buffer_bytes > 0
    write_buf_ptr := buf_ptr | ((buffer_bytes - 1) << 16)
    repeat
    while write_buf_ptr

    
PUB TxBufNoWait( buf_ptr, buffer_bytes )
{{
  * Sends a raw buffer over the serial line, but returns immediately (non-blocking).
  > buf_ptr      : a pointer to a byte array in Hub RAM.
  > buffer_bytes : the number of bytes to send.

  e.g. TxBufNoWait( @some_buffer, buffer_size_in_bytes )
  e.g. TxBufNoWait( @some_string, strsize( @some_string ) )  
}}
  if buffer_bytes > 0
    write_buf_ptr := buf_ptr | ((buffer_bytes - 1) << 16)

    
PUB WaitForTx
{{
  * Blocking wait until the transmit cog is finished with the Hub RAM.
  * Only useful after TxBufNoWait.
}}
  repeat
  while write_buf_ptr

  
PUB RxFlush
{{
  * Flushes receive buffer
}}
  rx_tail := rx_head

  
PUB RxCheck : rxbyte
{{
  * Checks if byte is waiting in the buffer, but doesn't wait.
  < returns -1 if no byte received, $00..$FF if byte

  e.g. if (c := RxCheck) < 0
}}
  rxbyte := -1
  if rx_tail <> rx_head
    rxbyte := rx_buffer[rx_tail]
    rx_buffer[rx_tail] := 0 ' OJO - for safety, should NOT be necessary
    rx_tail := (rx_tail + 1) // BUF_LEN

    
PUB RxTime(ms) : rxbyte | tout
{{
  * Waits for a byte to be received or a timeout to occur.
  > ms : the number of milliseconds to wait for an incoming byte
  < returns -1 if no byte received, $00..$FF if byte

  e.g. if (c := RxTime( 10 )) < 0
}}
  tout := clkfreq / 1000 * ms + cnt
  repeat
    rxbyte := RxCheck
  while (rxbyte < 0) and ((cnt - tout) < 0)

  
PUB Rx : rxbyte
{{
  * Receives a byte, and will wait forever to get one.
  < returns $00..$FF

  e.g. c := Rx
}}
  repeat
  while (rxbyte := rxcheck) < 0

  
PUB Dec(value) | i
{{
  * Sends a decimal number over the serial link.
  > value : the number to send.

  e.g. Dec( 10 ) ' sends "10"
}}
  if value < 0
    -value
    tx("-")
  i := 1_000_000_000
  repeat 10
    if value => i
      tx(value / i + "0")
      value //= i
      result~~
    elseif result or i == 1
      tx("0")
    i /= 10 

    
PUB Hex(value, digits)
{{
  * Sends a hexadecimal number over the serial link.
  > value  : the number to send.
  > digits : how many hex digits to send (typically 2 per byte)

  e.g. Hex( 10, 2 ) ' sends "0A"
}}
  value <<= (8 - digits) << 2
  repeat digits
    tx(lookupz((value <-= 4) & $F : "0".."9", "A".."F"))

    
PUB Bin(value, digits)
{{
  * Sends a binary number over the serial link.
  > value  : the number to send.
  > digits : how many binary digits to send (typically 8 per byte)

  e.g. Bin( 10, 4 ) ' sends "1010"
}}
  value <<= 32 - digits
  repeat digits
    tx((value <-= 1) & 1 + "0")


{========== Helper Functions ==========} 


PUB AToI( strptr ) : int | sign
{{
  * Converts a string to an integer
  > strptr : pointer to a string with a base-10 integer in it
  < Returns the integer, or 0 if no base-10 digits were found.

  e.g. i := AToI( string( "-42" ) ) ' returns -42 
}}
  sign := 1
  repeat
    case byte[strptr]
      "-":      sign := -1
      "0".."9": int := int*10 + byte[strptr] - "0"
      other:    quit
    strptr++
  int *= sign

  
PUB HToI( strptr ) : int | c
{{
  * Converts a hexadecimal string to an integer
  > strptr : pointer to a string with a hexadecimal number in it
  < Returns the integer, or 0 if no base-16 digits were found.

  e.g. i := HToI( string( "2A" ) )  ' returns 42 
}}
  repeat while c := lookdown( byte[strptr++] : "a".."f","0".."9","A".."F" )
    ' we got something new
    int <<= 4
    int |= (c + 9) & 15


{========== serial PASM code ==========}


DAT
org 0

{----- Setup -----}

FDS_entry     ' Counter A's job is to output data on the TX pin
              neg       phsa, #1
              mov       ctra, ctra_val
              mov       dira, maskTX            ' may be 0 if the TX pin is undefined

              ' Counter B's job is to count how many clocks the RX pin is low
              ' (specifically for aligning the read to the start bit).  Note:
              ' it increments by 2...I want to compare 1/4 the bit period, so
              ' instead of dividing the 1/2 bit period by 2, I just increment
              ' the logic counter by 2.
              mov       ctrb, ctrb_val
              mov       frqb, #2

              ' period info: load in 1/2 bit period
              rdlong    step_clocks, period_ptr

              ' let the caller know it's OK to proceed (store a 0)
              wrlong    outb, par
              
              ' start looping
              mov       timestamp, step_clocks
              add       timestamp, cnt

{----- Receive -----}
{
        Notes:
        * Any hub ops in RX must have a jmpret within 2 instructions: rdbyte nop jmpret
}

              ' resume after setting my start-bit-edge-detector to 0 
rx_cleanup    mov       phsb, #0
                                 
              ' loop (while calling TX co-routines) until we have a start bit
rx_main       jmpret    lockstep_ret, tx_jump
              mov       bits_in, phsb wz
        if_z  jmp       #rx_main
        
              ' We have a start bit!              
              ' Sync up with the correct place to sample the next bit:
              ' If RX was low for a short time (< 1/4 period), wait an
              ' extra 1/2 bit (places us before the last 1/4).
              jmpret    lockstep_ret, tx_jump                
              cmp       bits_in, step_clocks wc,wz
        if_b  jmpret    lockstep_ret, tx_jump

              ' OK, we have 8 data bits left to read
              mov       bits_in, #8

              ' read in 8 bits
:get_bit      jmpret    lockstep_ret, tx_jump
              test      maskRX, ina wc
              rcr       data_in, #1 
              jmpret    lockstep_ret, tx_jump              
              djnz      bits_in, #:get_bit
              
              ' align the data
              shr       data_in, #32-8 

              ' read and verify the stop bit
              jmpret    lockstep_ret, tx_jump
              test      maskRX, ina wc
        if_nc jmp       #rx_cleanup

              ' save the byte, increment the pointer
              add       rx_ptr, rx_head_ptr                
              wrbyte    data_in, rx_ptr
              add       rx_ptr, #1

              jmpret    lockstep_ret, tx_jump

              ' wrap the pointer and re-adjust back to 0-based index
              cmpsub    rx_ptr, rx_end_ptr wc
        if_nc sub       rx_ptr, rx_head_ptr

              ' update the head pointer                      
              wrword    rx_ptr, update_head_ptr
                                               
              tjz       phsb, #rx_main wr

              
{----- Sync -----}                   
              
tx_jump       ' This is the jmpret register that handles switching to the right TX co-routine
              jmp       #tx_main

lockstep      ' This routine cycles through parts of the TX routine, and ends always locked in step
              waitcnt   timestamp, step_clocks
              ' send out another TX bit (if we have any) right after the lockstep
              ror       phsa, write_bit
              ' return back to the RX code, also always a fixed time after the lockstep 
lockstep_ret  jmp       #0-0               


{----- Transmit -----

        Notes:
        * Any hub ops in TX must have a jmpret within 3 instructions: rdbyte nop nop jmpret
        * There is no guarantee that a TX co-routine is in sync
        * So bits are clocked out in "lockstep".
}

tx_main       ' check for outgoing data
              rdlong    bytes_out, par              
              jmpret    tx_jump, #lockstep
              ' I need some free time to adjust the clock rate
              rdlong    step_clocks, period_ptr           
              jmpret    tx_jump, #lockstep  
              ' loop if we have no data to send out
              tjz       bytes_out, #tx_main    

              ' Bottom 16 bits hold the buffer address
              ' (no need to mask, prop does it for us)
              mov       write_ptr, bytes_out
              
              ' Top 16 bits hold the buffer length - 1
              shr       bytes_out, #16
              add       bytes_out, #1
               
              jmpret    tx_jump, #lockstep                                          

:tx_byte      ' set up for sending out a byte
              rdbyte    tmp, write_ptr                                         
              add       write_ptr, #1

              ' force the stop bit
              or        tmp, #%1_00000000

              jmpret    tx_jump, #lockstep

              ' sign extend the 1 into all upper bits              
              shl       tmp, #32-9         
              sar       tmp, #32-10
              mov       phsa, tmp
              
              ' 10 bits (start + 8 data + stop) makes 20 half-bits
              mov       half_bits_out, #20

              ' wait 1/2 a bit, toggle the write_bit bit
:tx_bit       jmpret    tx_jump, #lockstep
              xor       write_bit, #1
              djnz      half_bits_out, #:tx_bit

              jmpret    tx_jump, #lockstep

              ' go back for more?
              djnz      bytes_out, #:tx_byte
              wrlong    bytes_out, par
              jmpret    tx_jump, #lockstep
              jmp       #tx_main + 1
              
        
{----- ASM parameters -----}


period_ptr              long 0
maskRX                  long 0
maskTX                  long 0
rx_head_ptr             long 0
rx_end_ptr              long 0
rx_ptr                  long 0
ctra_val                long 0
ctrb_val                long 0 
write_bit               long 0
update_head_ptr         long 0


{----- ASM local variables -----}


tmp           res 1
step_clocks   res 1
bits_in       res 1
half_bits_out res 1
bytes_out     res 1
data_in       res 1
timestamp     res 1
write_ptr     res 1


' make sure we fit inside the 512-16 instruction / variable limit
fit 497


{========== License ==========}


DAT
{{

+---------------------------------------------------------------------------------------+
|                               TERMS OF USE: MIT License                               |
|                                                                                       |
| Permission is hereby granted, free of charge, to any person obtaining a copy of this  |
| software and associated documentation files (the "Software"), to deal in the Software |
| without restriction, including without limitation the rights to use, copy, modify,    |
| merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    |
| permit persons to whom the Software is furnished to do so, subject to the following   |
| conditions:                                                                           |
|                                                                                       |
| The above copyright notice and this permission notice shall be included in all copies |
| or substantial portions of the Software.                                              |
|                                                                                       |
| THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   |
| INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         |
| PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    |
| HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  |
| CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  |
| OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         |
+---------------------------------------------------------------------------------------+
}}
