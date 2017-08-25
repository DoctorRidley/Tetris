/*
 * tetris.c.c
 *
 * Created: 6/6/2017 4:41:53 AM
 * Author : Benjamin Koshy
 */ 

#include <avr/io.h>
#include <bit.h>
#include <timer.h>
#include <io.h>
#include <scheduler.h>
#include <controller.h>

const unsigned char initial_block[2] = {0x3F, 0x18};
unsigned char moving_block[2]; //Row, Column
unsigned char matrix[1][1] = {0}; // Keeps track of position of blocks,
	                              // Sets bit to 1 iff a block is
								  // stopped permanently
unsigned char row;
unsigned char column;
unsigned long int score;
								  
enum Display_Square{Square_1, Square_2};
int Display_Square_SM(int state);

enum Falling_States{Initial_Fall, Fall_1, Stop};
int Fall_SM(int state);

enum Moving_States{Initial_Move, Start_Move, Down_Move, Down_Move_Wait, Left_Move, Left_Move_Wait, Right_Move, Right_Move_Wait};
int Move_SM(int state);

enum Pause_States{Promoted};
int Pause_SM(int state);

int main(void) {
	DDRA = 0xFE; PORTA = 0x01;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	const unsigned char display_period = 10;
	const unsigned long falling_period = 1000;
	const unsigned char moving_period = 10;
	
	/*-----SM for Displaying Block-----*/
	task task1;
	task1.state = -1;
	task1.period = display_period;
	task1.elapsedTime = display_period;
	task1.TickFct = &Display_Square_SM;
	
	/*-----SM for Falling Block-----*/
	task task2;
	task2.state = -1;
	task2.period = falling_period;
	task2.elapsedTime = falling_period;
	task2.TickFct = &Fall_SM;
	
	/*-----SM for Moving Block-----*/
	task task3;

	task3.state = -1;
	task3.period = moving_period; //may be a placeholder for now
	task3.elapsedTime = moving_period;
	task3.TickFct = &Move_SM;
	
	LCD_init();
	unsigned char i = 0;
	row = 0xFF;
	column = 0x00;
	task tasks[3] = {task1, task2, task3};
	TimerSet(10);
	TimerOn();
    while (1) {
		for (i = 0; i < 3; ++i) {
			if ( tasks[i].elapsedTime >= tasks[i].period ) {
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += 10;
		}
		while (!TimerFlag);
		TimerFlag = 0;
	}
}

int Display_Square_SM(int state) {
	switch (state) {
		case Square_1:
			state = Square_2;
			break;
		case Square_2:
			state = Square_1;
			break;
		default:
			state = Square_1;
			break;
	}
	switch (state){
		case Square_1:
			PORTB = moving_block[0];
			PORTD = moving_block[1];
			break;
		case Square_2:
			PORTB = row;
			PORTD = column;
			break;
		default:
			PORTB = moving_block[0];
			PORTD = moving_block[1];
			break;
	}
	return state;
}

int Fall_SM(int state) {
	Get_Input();
	switch (state) {
		case Initial_Fall:
			state = Fall_1;
			break;
		case Fall_1:
			//if block/border below, goto Stop
			if (moving_block[0] >= 0xFC) {
				//block is at bottom of matrix
				state = Stop;	
			}
			// else if (moving_block[0] <= row && moving_block[1] >= column) {
			// or something like that
			// 	state = Stop;
			// }
			
			break;
		case Stop:
			state = Initial_Fall;
			// check if a new block can be generated
			// state = Initial_Fall;
			// generate a new block
			break;
		default:
			state = Initial_Fall;
			break;
	}
	
	switch (state) {
		case Initial_Fall:
			moving_block[0] = initial_block[0]; // Row
			moving_block[1] = initial_block[1]; // Column
			break;
		case Fall_1:
 			if (moving_block[0] < 0xFC) {
				unsigned char invert_row = ~moving_block[0];
 				moving_block[0] = ~(invert_row >> 1);
 			}
			break;
		case Stop:
			row &= moving_block[0];
			column |= moving_block[1];
			break;
	    default:
			row = 0xFF;
			column = 0x00;
			break;
	}
	return state;
}

int Move_SM(int state) {
	Get_Input();
	switch(state) {
		case Initial_Move:
			state = Start_Move;
			break;
		case Start_Move:
			if (buttons == Down) {
				state = Down_Move;
			}
			else if (buttons == Left) {
				state = Left_Move;
			}
			else if (buttons == Right) {
				state = Right_Move;
			}
			break;
		case Down_Move:
			if (!buttons) {
				state = Start_Move;
			}
			else if (buttons == Down) {
				state = Down_Move_Wait;
			}
			break;
		case Down_Move_Wait:
			if (!buttons) {
				state = Start_Move;
			}
			break;
		case Left_Move:
			if (!buttons) {
				state = Start_Move;
			}
			else if (buttons == Left) {
				state = Left_Move_Wait;
			}
			break;
		case Left_Move_Wait:
			if (!buttons) {
				state = Start_Move;
			}
			break;
		case Right_Move:
			if (!buttons) {
				state = Start_Move;
			}
			else if (buttons == Right) {
				state = Right_Move_Wait;
			}
			break;
		case Right_Move_Wait:
			if (!buttons) {
				state = Start_Move;
			}
			break;
		default:
			state = Initial_Move;
			break;
	}
	switch(state) {
		case Initial_Move:
			break;
		case Start_Move:
			break;
		case Down_Move:
 			if (moving_block[0] < 0xFC) {
	 			unsigned char invert_row = ~moving_block[0];
	 			moving_block[0] = ~(invert_row >> 1);
			 }
			 break;
		case Down_Move_Wait:
			break;
		case Left_Move:
            if (moving_block[1] < 0x80) {
				moving_block[1] = moving_block[1] << 1;
			}
			break;
		case Left_Move_Wait:
			break;
		case Right_Move:
			if (moving_block[1] > 0x03) {
				moving_block[1] = moving_block[1] >> 1;
			}
			break;
		case Right_Move_Wait:
			break;
		default:
			break;
 	}
	return state;
}