 # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro    PUSH reg
    addi    $sp,$sp,-4
    sw    \reg,0($sp)
.endm

.macro    POP reg
    lw    \reg,0($sp)
    addi    $sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
.global delay
.global time2string
.global enable_interrupt

.text

enable_interrupt:
	ei
	jr $ra
	nop

hexasc:
	andi $v0, $a0, 0xf 	# loads the first 4 bits of the return arg
	li $t0, 9		# loads for the if statement
	ble $v0, $t0, return 	# if $v0 <= 9: return
	nop
	addi $v0, $v0, 7	# adds 7 if it's a letter, the offset
return:
	addi $v0, $v0, 0x30	# adds the offset for ASCII 0
	jr $ra 			# returns
	nop


delay:
	ble $a0, $zero, delayreturn # returns if ms <= 0
	nop
	addi $a0, $a0, -1 # ms = ms - 1
	li $t0, 0 	# i = 0
	li $t1, 3600    # used for the for loop
delayloop:
	ble $t1, $t0, delay # if (i >= 160) break the loop
	nop
	addi $t0, $t0, 1 # i = i + 1
	j delayloop # continue
	nop
delayreturn:
	jr $ra
	nop
	
time2string:
	andi $t0, $a1, 0xff	# Masks it to only use the last byte
	beqz $t0, time2ding	# if the byte is 0, eg if the seconds are 0 go to ding
    	nop

	PUSH $s0        		# Saves $s0, callee saved
	PUSH $s1         	# Saves $s1, callee saved
	PUSH $ra         	# Saves $ra, gets overwritten by jal hexasc

	move $s0, $a0        	# To use $a0 freely since it’s getting overwritten by hexasc
	move $s1, $a1 

	srl $a0, $a1, 12    	# Use the first nibble by moving it to the least   significant bits
	jal hexasc        	# Get the ASCII in $v0
	nop
	sb $v0, 0($s0)        	# Stores the return value at the appropriate   bytes
    
	srl $a0, $s1, 8        	# Use the second nibble
	jal hexasc        	# Get the ASCII
	nop
	sb $v0, 1($s0)        	# Stores the return value
    
	li $t0, 0x3A
	sb $t0, 2($s0)        	# Stores ":"

	srl $a0, $s1, 4        	# Use the third nibble
	jal hexasc        	# Get the ASCII
	nop
	sb $v0, 3($s0)        	# Stores the return value

	move $a0, $s1        	# Stores the fourth nibble, no need for srl
	jal hexasc        	# Get the ASCII
	nop

	sb $v0, 4($s0)        	# Stores the return value
	sb $zero, 5($s0)    	# Stores the null byte

	POP $ra        		# Restores $ra
	PUSH $s1       		# Restores $s1, callee saved
	POP $s0        		# Restores $s0
	jr $ra          	# Returns
	nop

time2ding:
	lui $t0, 0x474e 	# G N
	ori $t0, 0x4944 	# I D
	sw $t0, 0($a0)  	# Stores the word "GNID"

	sb $zero, 4($a0)        # Stores null
	sb $zero, 5($a0)        # Stores null
	jr $ra
	nop
