# Parallel SRAM/EEPROM chips test with Arduino

This program is designated for testing the parallel SRAM/EEPROM chips with Arduino Mega (2560) using the serial console as a command line interface.
It's capable of testing the chips having volume up to 512k.

## How to perform the test: ##

1. Download the *'SRAM_test.ino'* file (or clone this repo).
2. Open the *'SRAM_test.ino'* file with Arduino IDE.
3. Connect your chip pins to the Arduino ports, changing the variables ***addr***, ***io***, ***cs***, ***we***, ***oe*** to the actual pins if necessary.
4. Update the ***cap*** variable, indicating the default capacity of your chip in words (i.e. 32k chip has 32767 words). This value can be changed later via the command line interface.
5. Connect your Arduino board to the PC, select your Arduino board model and COM port.
6. Open the serial console *(Tools -> Serial monitor)*, select the baud rate of '***115200 baud***'.
7. Flash the program into the Arduino controller.

When the controller is booted up, you should get the "Ready." message in the serial console. Now you can type the commands to execute the tests.

## Command line interface: ##

You can type in the following commands to perform the operations or change settings.

### start \<word> ###

Begins the automatic sequential write-read test, starting from the specified <word> address (decimal word number, i.e. 0 - 32766 for 32k chip).
The data that is written to memory cells are being randomly generated.

*word* - decimal word number.

Example command:

```
start 256 
```

Example output:

```
Errors: 0, tWP: 1, Word: 256, Addr: 0000000000100000000, Data_dec: 167, Data_bin: 10100111, read binary: 10100111 - OK
Errors: 0, tWP: 1, Word: 257, Addr: 0000000000100000001, Data_dec: 241, Data_bin: 11110001, read binary: 11110001 - OK
Errors: 0, tWP: 1, Word: 258, Addr: 0000000000100000010, Data_dec: 217, Data_bin: 11011001, read binary: 11011001 - OK
Errors: 0, tWP: 1, Word: 259, Addr: 0000000000100000011, Data_dec: 42, Data_bin: 00101010, read binary: 00101010 - OK
Errors: 0, tWP: 1, Word: 260, Addr: 0000000000100000100, Data_dec: 130, Data_bin: 10000010, read binary: 10000010 - OK
Errors: 0, tWP: 1, Word: 261, Addr: 0000000000100000101, Data_dec: 200, Data_bin: 11001000, read binary: 11001000 - OK
...
```


### stop ###

This command can be issued to stop the automatic test launched by the *start* command.


### resume  ###

This command will resume the automatic test, interrupted by the *stop* command, starting from the address where the previous test was interrupted.


### test \<word> ###

Performs the write-read test of a single word. The data is randomly generated.

*word* - decimal word number.


Example command:

```
test 777
```

Example output:

```
Word: 777, Addr: 0000000001100001001, Data_dec: 229, Data_bin: 11100101, read binary: 11100101 - OK
```


### write \<word> \<data> ###

Write the data into the specific word. 

*word* - decimal word number.
*data* - decimal byte value (0 - 255).

Example command:

```
write 123 255
```

Output:

```
Word: 123, Addr: 0000000000001111011, Data_dec: 255, Data_bin: 11111111
```


### read \<word> ###

Reads the single word.

*word* - decimal word number.

Example command:

```
read 123
```

Example output:

```
Word: 123, Addr: 0000000000001111011, read binary: 11111111
```


### overlap ###

Performs the memory address overlap test, in attempt to detect the fake/improper capacity chips.

Example command:

```
overlap
```

Example output:

```
Writing...
Word: 1, Addr: 0000000000000000001, Data_dec: 225, Data_bin: 11100001
Word: 2, Addr: 0000000000000000010, Data_dec: 6, Data_bin: 00000110
Word: 4, Addr: 0000000000000000100, Data_dec: 152, Data_bin: 10011000
Word: 8, Addr: 0000000000000001000, Data_dec: 109, Data_bin: 01101101
Word: 16, Addr: 0000000000000010000, Data_dec: 97, Data_bin: 01100001
Word: 32, Addr: 0000000000000100000, Data_dec: 255, Data_bin: 11111111
Word: 64, Addr: 0000000000001000000, Data_dec: 52, Data_bin: 00110100
Word: 128, Addr: 0000000000010000000, Data_dec: 161, Data_bin: 10100001
Word: 256, Addr: 0000000000100000000, Data_dec: 30, Data_bin: 00011110
Word: 512, Addr: 0000000001000000000, Data_dec: 25, Data_bin: 00011001
Word: 1024, Addr: 0000000010000000000, Data_dec: 253, Data_bin: 11111101
Word: 2048, Addr: 0000000100000000000, Data_dec: 54, Data_bin: 00110110
Word: 4096, Addr: 0000001000000000000, Data_dec: 80, Data_bin: 01010000
Word: 8192, Addr: 0000010000000000000, Data_dec: 233, Data_bin: 11101001
Word: 16384, Addr: 0000100000000000000, Data_dec: 183, Data_bin: 10110111

Reading...
Word: 1, Addr: 0000000000000000001, read binary: 11100001 - Match
Word: 2, Addr: 0000000000000000010, read binary: 00000110 - Match
Word: 4, Addr: 0000000000000000100, read binary: 10011000 - Match
Word: 8, Addr: 0000000000000001000, read binary: 01101101 - Match
Word: 16, Addr: 0000000000000010000, read binary: 01100001 - Match
Word: 32, Addr: 0000000000000100000, read binary: 11111111 - Match
Word: 64, Addr: 0000000000001000000, read binary: 00110100 - Match
Word: 128, Addr: 0000000000010000000, read binary: 10100001 - Match
Word: 256, Addr: 0000000000100000000, read binary: 00011110 - Match
Word: 512, Addr: 0000000001000000000, read binary: 00011001 - Match
Word: 1024, Addr: 0000000010000000000, read binary: 11111101 - Match
Word: 2048, Addr: 0000000100000000000, read binary: 00110110 - Match
Word: 4096, Addr: 0000001000000000000, read binary: 01010000 - Match
Word: 8192, Addr: 0000010000000000000, read binary: 11101001 - Match
Word: 16384, Addr: 0000100000000000000, read binary: 10110111 - Match
```


### errors ###

Prints the list of error cells detected duding the previous automatic test.


### recheck ###

Performs the automatic re-check of the error cells, detected during the previous automatic test.


### set \<parameter> \<value> ###

Adjust the specific parameter settings. The values are stored in memory, and are reset to defaults upon Arduino restart/reset.

**Parameters:**

***cap*** - sets the chip capacity in words. Provide the new chip capacity with the \<value> parameter. The default capacity is set by the *cap* variable.

Example command:

```
set cap 65535
```

Output:

```
Capacity was set to: 65535 words.
```

***twp*** - sets the minimum write pulse width (tWP) - the write sequence delay in milliseconds. Please correspond to the chip's datasheet to get the correct tWP value. The default tWP value is set by the *twp* variable.

Example command for setting tWP = 20ms:

```
set twp 20
```

Output:

```
tWP was set to: 20
```

Setting the *twp* value to "-1" will enable the potentiometer reading for dynamic adjustment of the tWP value.
You can connect the output pin of 1k Ohm potentiometer to the Arduino analog input port (A0 - A15), and indicate the potentiometer pin in the *twp_pin* code definition (or use predefined A7 input port).

Example:

```
set twp -1
```

Output:

```
tWP was set to potentiometer input
```