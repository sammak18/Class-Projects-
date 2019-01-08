
# mp1.S - missile-command version
# Good luck, and have fun!

.data
	
    # Constants for accessing the fields of a struct missile, 
    # struct missile is defined in rtc.h and mp1.h

	NEXT 	  = 0
	X	  = 4
	Y	  = 8
	VX	  = 12
	VY	  = 16
	DEST_X	  = 20
	DEST_Y	  = 24
	EXPLODED  = 28
	C	  = 32

    # Character to draw for an explosion - '@'

	EXPLOSION = 64


# Data shared between this file and rtc.c helper functions

# This '.globl' directive makes these symbols visible externally
.globl mp1_missile_list, base_alive, mp1_score

mp1_missile_list:	.long 0x0	# Head of list pointer
base_alive:		.long 0x0	# Array of 3 bytes, plus a padding byte
mp1_score:		.long 0x0	# Player's current score


# Data private to this  file

base_pic:     	    .string "/^^^\\"	# Picture of a live base
dead_base_pic: 	    .string "xxxxx"	# Picture of a dead base
crosshairs_x:		.long 0x0	# X-position of the crosshairs
crosshairs_y:		.long 0x0	# Y-position of the crosshairs
previous_ptr:		.long 0x0
.text

# void mp1_poke(void)
# You can use this function to write to video memory.
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX

mp1_poke:
		movl    vmem_base_addr(,1),%edx
		movb    %cl, (%edx,%eax,1)
		ret

#   ----------------- Exported functions ---------------------

# void mp1_rtc_tasklet(unsigned long garbage)
# Performs three tasks:
#	(1) updates the list of missiles (implement this in update_missiles,
#           below, and call it from here).
#	(2) Redraw the bases - they may have been overwritten with missiles
#	(3) Redraw the crosshairs - it may have been overwritten with missiles
# Inputs   : none
# Outputs  : none
# Registers: Standard C calling convention

.globl mp1_rtc_tasklet
mp1_rtc_tasklet:

/*
; First, your tasklet should walk down the struct missile linked list. For each missile, you should check whether
; it is currently exploding. If not, then you should update the x and y fields as explained above in the “MP1 Data
; Structure” section (add vx and vy). There are then three cases based on the state and position of the missile.*/
	
	# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %eax
	pushl %ecx
	pushl %edx
	

	# call redraw_cities	
	# call redraw_crosshairs

	call update_missiles

	call redraw_cities	
	call redraw_crosshairs

	popl %edx
	popl %ecx
	popl %eax
	popl %edi
	popl %esi
	popl %ebx

	leave
	ret


redraw_cities:



	movl base_alive,%edi
	# shift left by 15 then check if number is -ve
	shll $15,%edi 
	cmpl $0,%edi
	jl draw_base_one_alive

	# draw base one as dead

	# draw first x
	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (24 * 80 +18 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $18,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (24 * 80 +18 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $19,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax
	
	# (20 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $20,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (21 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $21,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (22 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $22,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax


check_base_two:
	shll $8,%edi
	cmpl $0,%edi
	jl draw_base_two_alive

	# draw base two as dead
	# (38 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $38,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (39 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $39,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax
	
	# (40 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $40,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (41 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $41,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (42 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $42,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax

	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

check_base_three:
	shll $8,%edi
	cmpl $0,%edi
	jl draw_base_three_alive

	# draw base three as dead
	# (58 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $58,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (59 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $59,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax
	
	# (60 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $60,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (61 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $61,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (62 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $62,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax

	# move  char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax

return_from_draw_bases:
	ret

draw_base_one_alive:

	# (18 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $18,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $47,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (19 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $19,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax
	
	# (20 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $20,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (21 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $21,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (22 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $22,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $92,%cl

	call mp1_poke

	popl %edx
	popl %eax
	

	jmp check_base_two

draw_base_two_alive:
	
	movl $24,%ecx
	imull $80,%ecx
	addl $38,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $47,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (39 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $39,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax
	
	# (40 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $40,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (41 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $41,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (42 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $42,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax

	# move  char to cl
	movb $92,%cl

	call mp1_poke

	popl %edx
	popl %eax
	jmp check_base_three

draw_base_three_alive:
	# (58 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $58,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $47,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (59 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $59,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax
	
	# (60 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $60,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (61 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $61,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move  char to cl
	movb $94,%cl

	call mp1_poke

	popl %edx
	popl %eax

	# (62 * 80 +24 ) * 2
	movl $24,%ecx
	imull $80,%ecx
	addl $62,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax

	# move  char to cl
	movb $92,%cl

	call mp1_poke

	popl %edx
	popl %eax
	jmp return_from_draw_bases
	

	

redraw_crosshairs:
	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (crosshairs_y * 80 +crosshairs_x  ) * 2
	movl crosshairs_y,%ecx
	imull $80,%ecx
	addl crosshairs_x,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move char to cl
	movb $120,%cl

	call mp1_poke

	popl %edx
	popl %eax
	ret

update_missiles:
	# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	pushl %ebx
	pushl %esi
	pushl %edi
	

	movl mp1_missile_list,%ebx # ebx contains head of linked list

linked_list_loop:

	# check if current pointer points to NULL, we are done if it does
	cmpl $0,%ebx
	je done_traversing

/*
	; if the missile has moved off of the screen (that is, its screen x coordinate
	; is outside of the range 0-79 or its y coordinate is out of the range 0-24), 
	; then the missile should be erased from the screen, 
	; removed from the linked list, and its struct missile freed with mp1 free
*/
/*
	# check if x < 0
	movl 4(%ebx),%edx
	shrl $16,%edx
	cmpl $0,%edx
	jl remove_missile

	# check if x > 79
	movl 4(%ebx),%edx
	shrl $16,%edx
	cmpl $79,%edx
	jg remove_missile

	# check if y < 0
	movl 8(%ebx),%edx
	shrl $16,%edx
	cmpl $0,%edx
	jl remove_missile

	# check if y > 24
	movl 8(%ebx),%edx
	shrl $16,%edx
	cmpl $79,%edx
	jg remove_missile*/
/*
	; Second, if the missile has reached its destination or is currently exploding, you must call missile explode with a
	; pointer to the missile’s struct missile as an argument. The missile explode function (provided to you) checks
	; whether this missile’s exposion causes any other missiles or any of your cities to explode. If so, it returns a non-zero
	; value. Otherwise, it returns zero. After calling missile explode, you must decrement the exploded field for this
	; missile. If it reaches zero, then the missile is finished exploding and must be erased from the screen, removed from
	; the list, and freed with mp1 free. Otherwise, it should be drawn to the screen with the EXPLOSION character.
*/	

	# check if it is currently exploding
	cmpl $0,28(%ebx) 
	jne missile_exploding

	# check if missile has reached its destination
	# x == dest_x AND y == dest_y
	movl 4(%ebx),%edx
	shrl $16,%edx
	cmpl %edx,20(%ebx)
	je y_reached_dest
	jmp missile_is_okay_till_now
y_reached_dest:	
	movl 8(%ebx),%edx
	shrl $16,%edx
	cmpl %edx,24(%ebx)
	je missile_exploding

missile_is_okay_till_now:
/*
; Finally, if the missile is simply moving toward its destination, is not exploding, and is still on 
; the screen, you should check whether its screen position has changed. If so, you should erase it 
; from its old position and re-draw it in its new position.
; Note that in every case the missile should be re-drawn—it could have been over-written by another 
; missile or the crosshairs moving through the same text-video location. 
*/
	# overwrite old location

	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (4(%ebx) * 80 +8(%ebx) ) * 2
	movl 8(%ebx),%ecx
	shrl $16,%ecx
	imull $80,%ecx
	movl 4(%ebx),%edx
	shrl $16,%edx
	addl %edx,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move space char to cl
	movb $32,%cl

	call mp1_poke

	popl %edx
	popl %eax
	 

	# increment x and y

	# add vx to x
	movl 12(%ebx),%ecx	# eax contains vx
	addl %ecx, 4(%ebx)   # x = x+vx

	# add vy to y
	movl 16(%ebx),%ecx	# eax contains vy
	addl %ecx, 8(%ebx)   # y = y+vy

	# write character in new location

	# check if x < 0
	movl 4(%ebx),%edx
	shrl $16,%edx
	cmpl $0,%edx
	jl remove_missile

	# check if x > 79
	movl 4(%ebx),%edx
	shrl $16,%edx
	cmpl $79,%edx
	jg remove_missile

	# check if y < 0
	movl 8(%ebx),%edx
	shrl $16,%edx
	cmpl $0,%edx
	jl remove_missile

	# check if y > 24
	movl 8(%ebx),%edx
	shrl $16,%edx
	cmpl $79,%edx
	jg remove_missile

	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (4(%ebx) * 80 +8(%ebx) ) * 2
	movl 8(%ebx),%ecx
	shrl $16,%ecx
	imull $80,%ecx
	movl 4(%ebx),%edx
	shrl $16,%edx
	addl %edx,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	xor %ecx,%ecx
	
	# move  char to cl
	movb 32(%ebx),%cl

	call mp1_poke

	popl %edx
	popl %eax
	jmp keep_moving_forward 


erase_missile: 

	# overwrite old location

	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (8(%ebx) * 80 +4(%ebx) ) * 2
	pushl %ecx 
	xorl %ecx,%ecx
	movl 8(%ebx),%ecx
	shrl $16,%ecx
	imull $80,%ecx
	movl 4(%ebx),%edx
	shrl $16,%edx
	addl %edx,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	xorl %eax,%eax
	movl %ecx,%eax
	
	# move space char to cl
	xorl %ecx,%ecx
	movb $32,%cl

	call mp1_poke

	popl %edx
	popl %eax
	popl %ecx
	 

remove_missile:
	# check if missile to be removed is NULL we are done
	# cmpl 0,%ebx
	# je done_traversing
	# check if missile to be removed is the head  then go to special case
	cmpl mp1_missile_list,%ebx
	je head_to_be_removed
	# check if the missile to be removed is the tail then go to special case
	cmpl $0,NEXT(%ebx)
	je tail_to_be_removed

	# check else make prev pointer point to next and free the current
	movl NEXT(%ebx),%edx
	movl %edx,NEXT(%eax)
		
	# prepare for function call
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters
	pushl %ebx
	call mp1_free

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax

	# put next pointer of prev pointer in ebx so we move forward iteratively
	movl %eax,%ebx	 
	jmp keep_moving_forward

tail_to_be_removed:
	movl $0,NEXT(%eax) #make previous point to null 

	# prepare for function call
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters
	pushl %ebx
	call mp1_free

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax
	jmp keep_moving_forward

head_to_be_removed:
	movl NEXT(%ebx),%edx
	movl %edx,mp1_missile_list # head pointer set to new head pointer
	
	# prepare for function call
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters
	pushl %ebx
	call mp1_free

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax


keep_moving_forward:
	movl %ebx,%eax 	# eax contains previous pointer 
	movl NEXT(%ebx),%ebx
	jmp linked_list_loop

done_traversing:
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

missile_exploding:
	# call missile_explode
/*
	; The missile explode function (provided to you) checks
	; whether this missile’s explosion causes any other missiles or any of your cities to explode. 
	; If so, it returns a non-zero value. Otherwise, it returns zero. After calling missile explode,
	; you must decrement the exploded field for this missile. If it reaches zero, then the missile 
	; is finished exploding and must be erased from the screen, removed from the list, and freed 
	; with mp1 free. Otherwise, it should be drawn to the screen with the EXPLOSION character.
*/
	# int missile_explode(struct missile *m)

	# prepare for function call
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters
	pushl %ebx
	call missile_explode

	# eax contains return value, shifting to esi since eax contains previous pointer
	movl %eax, %esi

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax # restoring prev pointer
		 
	# decrement exploding
	subl $1,28(%ebx)
	cmpl $0,28(%ebx)
	jle erase_missile	 
	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (4(%ebx) * 80 +8(%ebx) ) * 2
	
	movl 8(%ebx),%ecx
	shrl $16,%ecx
	imull $80,%ecx
	movl 4(%ebx),%edx
	shrl $16,%edx
	addl %edx,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	xor %ecx,%ecx
	
	# move explosion char to cl
	movb $64,%cl

	call mp1_poke

	popl %edx
	popl %eax

	jmp keep_moving_forward


	# check if it reaches zero
	/*
	; If it reaches zero, then the missile 
	; is finished exploding and must be erased from the screen, removed from the list, and freed 
	; with mp1 free. Otherwise, it should be drawn to the screen with the EXPLOSION character.
	*/
	# (spaceholder) else draw it to the screen with the explosion character


	# esi contains ret val
	cmpl 0,%esi
	je skip_notification
	# notify user things have changed
	#prepare for call, save stacks
	pushl %eax
	pushl %edx
	pushl %ecx

	call mp1_notify_user
	#restore values 
	popl %ecx
	popl %edx
	popl %eax # restoring prev pointer

skip_notification:

	jmp keep_moving_forward







# int mp1_ioctl(unsigned long arg, unsigned int cmd)
# The dispatch function for the MP1 ioctls - should use the cmd argument
# and a jumptable to execute one of the specific ioctls implemented below.
# Inputs   : unsigned long arg - parameter to the mp1_ioctl_....
#          : unsigned int cmd  - specifies which mp1_ioctl_... function 
#          :                     to execute
# Outputs  : Returns an integer - depends on which ioctl() function is called
# Registers: Standard C calling convention


.globl mp1_ioctl
mp1_ioctl:
/*	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
*/
	movl 8(%esp), %ecx
	cmpl $4, %ecx
	jg default
	cmpl $0,%ecx
	jl default
	jmp *jumptable(,%ecx,4)
/*
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret*/

jumptable:
.long mp1_ioctl_startgame, mp1_ioctl_addmissile, mp1_ioctl_movexhairs,mp1_ioctl_getstatus,mp1_ioctl_endgame
# ----------------- Functions private to this file -------------------

/*
; int mp1 ioctl startgame (unsigned long ignore)
; This function is called when the game is about to start. The parameter passed in arg is meaningless and should
; be ignored. This function should initialize all of the variables used by the driver—all of the variables declared in
; mp1.S—and the crosshairs should be set to the middle of the screen: (40, 12).
*/
mp1_ioctl_startgame:
	# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	pushl %ebx
	pushl %esi
	pushl %edi

	# initializing variables
	movl $0,mp1_missile_list
	movl $0x00010101,base_alive
	movl $0,mp1_score


	# crosshair set to the middle of the screen
	addl $40,crosshairs_x # X-position of the crosshairs
	addl $12,crosshairs_y # Y-position of the crosshairs


	popl %edi
	popl %esi
	popl %ebx

	leave
	ret
/*
; int mp1 ioctl addmissile (struct missile* user missile);
; This ioctl must add a new missile to the game. The parameter is a pointer to a struct missile in user space. This
; function needs to copy the user’s missile into a dynamically allocated buffer in kernel space. If either the dynamic
; memory allocation (see “Allocating and Freeing Memory” below) or the data copy (see “Moving data to/from the
; kernel”) fails, this function should return -1. If it does fail, it should be sure to free any memory it has allocated before
; returning. If it succeeds, it should add the new missile into the linked list and return 0.
*/
mp1_ioctl_addmissile:
	# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	movl 8(%esp), %ecx # ecx contains user missile

	# callee saved registers
	pushl %ebx
	pushl %esi
	pushl %edi

	# copy the user missile into a buffer in kernel space by calling C function 

	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags

	pushl $33 # parameter for number of bytes to be allocated in malloc 

	call mp1_malloc

	cmpl $0, %eax
	je add_missile_failed

	movl %eax,%ebx # ebx contains return value from malloc now
	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax
	
	# prepare to call copy form user 
	# unsigned long mp1 copy from user (void *to, const void *from, unsigned long n);
	
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters

	pushl $33
	pushl %ecx # ptr to user missile 
	pushl %ebx # return value from malloc and starting address. 
	call mp1_copy_from_user
	cmpl $0,%eax
	jne add_missile_failed

	movl %eax,%esi # esi contains return value from copy2user now
	addl $12,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax

	# we are here since the function did not fail
	# add the new missile into the linked list
	# ebx points to node in linked list to be added 
	movl mp1_missile_list,%edx
	movl %edx,NEXT(%ebx)
	movl %ebx,mp1_missile_list

	# return zero 
	movl $0, %eax
	# restore callee-saved registers ebx,esi,edi
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

add_missile_failed:
	movl $-1,%eax
	# free any memory allocated
	# ebx contains pointer to memory
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters

	pushl %ebx

	call mp1_free

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax

	# restore callee-saved registers ebx,esi,edi
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret





/*
; int mp1 ioctl movexhairs (unsigned long xhair delta packed);
; This function moves the crosshairs. The parameter is a 32-bit integer containing two signed 16-bit integers packed into
; its low and high words. The low 16 bits contain the amount by which the x component of the crosshair position should
; change, and the high 16 bits contain the amount by which the y component should change. This function should not
; allow the crosshairs to be moved off of the screen—that is, it should ensure that the x component of its position stays
; within the range 0-79, and its y component stays within the range 0-24. If the position of the crosshairs does change,
; this function should redraw it at its new location. It should never fail, and always return 0.
*/
mp1_ioctl_movexhairs:
	
	# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	movl 8(%esp), %ecx # ecx contains x_hair_delta_packed
	# callee saved registers
	pushl %ebx
	pushl %esi
	pushl %edi
	
	# preserve ecx
	pushl %ecx
	# remove old crosshair
	# calculate offset in ecx then transfer to eax (while preserving its value)
	# (crosshairs_x * 80 +crosshairs_y  ) * 2
	movl crosshairs_y,%ecx
	imull $80,%ecx
	addl crosshairs_x,%ecx
	imull $2,%ecx

	pushl %eax 
	pushl %edx
	movl %ecx,%eax
	
	# move char to cl
	movb $32,%cl

	call mp1_poke

	popl %edx
	popl %eax
	popl %ecx


	addw crosshairs_x,%cx
	# check if cx is between 0-79, if it is, put its contents in crosshairs_x
	
	cmpw $0, %cx # check if x < 0
	jl x_below_zero
	cmpw $79, %cx # check if x > 79
	jg x_above_sevnine

	# x is between 0-79 if we are here, move it to variable
	movw %cx,crosshairs_x


check_y: 

	shrl $16, %ecx # logical shift 16 bits
 	addw crosshairs_y,%cx	

	cmpw $0, %cx # check if y < 0
	jl y_below_zero
	cmpw $24, %cx # check if y > 24
	jg y_above_twenfour

	# y is between 0-24 if we are here, move it to variable
	movw %cx,crosshairs_y


moved_x_hairs:
	# push return value 0 in eax
	movl $0, %eax
	# restore callee-saved registers ebx,esi,edi
	popl %edi
	popl %esi
	popl %ebx
	call redraw_crosshairs
	leave
	ret



x_below_zero:
	movl $0, crosshairs_x
	jmp check_y

x_above_sevnine:
	movl $79, crosshairs_x
	jmp check_y

y_below_zero:
	movl $0, crosshairs_y
	jmp moved_x_hairs

y_above_twenfour:
	movl $24, crosshairs_y
	jmp moved_x_hairs



/*

; int mp1 ioctl getstatus (unsigned long* user status)
; This function allows the user to retrieve the current score and the status of the three cities. The argument is a pointer to
; a 32-bit integer in user space. This function should copy the current score into the low 16-bits of that integer, and the
; status of the three cities into bits 16, 17, and 18. The missile explode function maintains the user’s current score
; in the mp1 score variable declared in mp1.S. If a city is currently alive, the corresponding bit should be a 1 if it has
; been destroyed, the bit should be 0. The missile explode function maintains this information in the base alive
; array, as described above. This function should return 0 if the copy to user space succeeds, and -1 if it fails.
*/
mp1_ioctl_getstatus:

	# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	movl 8(%esp), %ecx # ecx contains user_status ptr

	# callee saved registers
	pushl %ebx
	pushl %esi
	pushl %edi

	xorl %eax,%eax # clear eax
	# put status of cities in eax's 2nd, 1st and 0th bit
	# base_alive when all cities are alive: 0x00010101
	movl base_alive,%ebx
	# shift left by 15 then check if number is -ve
	shll $15,%ebx 
	cmpl $0,%ebx
	jl base_one_alive

	shll $8,%ebx
	cmpl $0,%ebx
	jl base_two_alive

	shll $8,%ebx
	cmpl $0,%ebx
	jl base_three_alive

	shll $16,%eax
	movw mp1_score,%ax # eax contains the score and the base status now

	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags

	pushl $4 # parameter for number of bytes to be allocated in malloc 
	call mp1_malloc
	cmpl $0, %eax
	jne get_status_failed


	addl $4,%esp # pop parameters and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax

	# eax now contains address of the status

	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags

	# parameters for copy to user
	pushl $32 # number of bits to be copied
	pushl %eax # copy from (ptr to user status )
	pushl %ecx # copy to (user_status)
	call mp1_copy_to_user
	cmpl $0,%eax
	jne get_status_failed

	addl $12,%esp # pop parameters and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax

	movl $0,%eax
	# restore callee-saved registers ebx,esi,edi
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

get_status_failed:
	movl $-1,%eax
	leave 
	ret

base_one_alive:
	# modify 0th bit for eax to be 1
	addl $1, %eax
	ret
base_two_alive:
	# modify 1st bit for eax to be 1
	addl $2, %eax
	ret
base_three_alive:
	# modify 2nd bit for eax to be 1
	addl $4, %eax
	ret
	
/*
; int mp1 ioctl endgame (unsigned long ignore)
; Called when the game is over, this function must perform all the cleanup work. It should free all the memory being
; used by the linked list and then return success. When freeing the list, be careful to avoid accessing any memory that
; has already been freed*/
mp1_ioctl_endgame:
 
# setting up stack
	pushl %ebp # save old frame pointer
	movl %esp,%ebp # ebp (new frame ptr) now points to old ebp
	# callee-save registers ebx,esi,edi
	pushl %ebx
	pushl %esi
	pushl %edi

	# get head of the linked list in ecx
	movl mp1_missile_list, %ecx
	cmpl $0,%ecx
	je successful_free
	cmpl $0,NEXT(%ecx)
	je only_one_node
	# put next pointer from head in eax
	movl NEXT(%ecx),%eax

freeingloop:
	# check if head == NULL. If it is, you are done. Jump to successful_free
	cmpl $0,%ecx
	je successful_free
	# prepare for function call
	# push parameters and save caller saved registers
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters
	pushl %ecx
	call mp1_free

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax

	cmpl $0, %eax 
	je successful_free
	# ecx now contains new head, eax contains next pointer, repeat freeing until reaching null 
	movl %eax,%ecx 
	movl NEXT(%ecx),%eax

	jmp freeingloop


only_one_node:
	pushl %eax
	pushl %edx
	pushl %ecx
	# pushl %eflags
		
	# parameters
	pushl %ecx
	call mp1_free

	addl $4,%esp # pop parameter and restore stack values
	# popl %eflags
	popl %ecx
	popl %edx
	popl %eax
	jmp successful_free


successful_free:
	# update head of linked list 
	movl $0, mp1_missile_list
	# put return value 0 in eax
	movl $0, %eax
	# restore callee-saved registers ebx,esi,edi
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret


default:
	movl $-1, %eax
	ret

