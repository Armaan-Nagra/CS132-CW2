#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void move_to_location(int connection, unsigned char id,
			unsigned char loc_h, unsigned char loc_l) {

	unsigned char cs = ~ ( id + 0x07 + 0x03 + 0x1e + loc_l + loc_h +
				0x30 + 0x00);

	unsigned char arr[] = { 0xff, 0xff, id, 0x07, 0x03, 0x1e, loc_l,
                                       loc_h, 0x30, 0x00, cs };

	int buff_len = 100;
	unsigned char buff[buff_len];

	int bytes_read = write_to_connection(connection,arr,11,buff,buff_len);

}

// One-second delay until the robot arms next move
void wait_until_done(int connection, unsigned char id, int time) {
	usleep(time);
}

// Open the claw of the robot via Motor 5
int open_claw(int connection) {
	wait_until_done(connection,100, 5000000);
	move_to_location(connection,5,0x01,0xff);
	return 0;
}

// Close the claw of the robot via Motor 5
int close_claw(int connection){
	wait_until_done(connection,15, 3000000);
	move_to_location(connection,5,0x01,0x4a);
	return 0;
}

int main(int argc, char* argv[]) {

	int connection = open_connection("/dev/ttyUSB0",B1000000);

	// Open the claw to initiate picking a block up
	open_claw(connection);

	// Pick up block Z from pile/tower 1
	move_to_location(connection,1,0x01,0x9b); 
	move_to_location(connection,2,0x01,0x27);
	move_to_location(connection,3,0x01,0x74);
	move_to_location(connection,4,0x01,0x11);

	// Close the claw to grab the block
	close_claw(connection);

	// Wait for the robot arm to pick up the 
	// block before going back up
	wait_until_done(connection,15,2000000);
	
	// Bring the robot arm back up to its
	// initial position (Return to base)
	move_to_location(connection,2,0x01,0xf4);
	wait_until_done(connection,15, 2000000);
	move_to_location(connection,1,0x01,0xf0); 
	move_to_location(connection,3,0x02,0x1e);
	move_to_location(connection,4,0x01,0xfd);

	wait_until_done(connection,15, 2000000);

	// Placing block Z on pile/tower 3
	move_to_location(connection,1,0x02,0x59); 
	move_to_location(connection,2,0x00,0xdc);
	move_to_location(connection,3,0x01,0xed);
	move_to_location(connection,4,0x00,0xeb);


	open_claw(connection);

	wait_until_done(connection,15, 2000000);

	//going back up (bricked function)
	move_to_location(connection,2,0x01,0xf4);
	wait_until_done(connection,15, 2000000);
	move_to_location(connection,1,0x01,0xf0); 
	move_to_location(connection,3,0x02,0x1e);
	move_to_location(connection,4,0x01,0xfd);

	//picking up tower 1 block y
	move_to_location(connection,2,0x01,0x2e);
	wait_until_done(connection,15, 2000000);
	move_to_location(connection,1,0x01,0x9d); 
	move_to_location(connection,3,0x01,0x5c);
	move_to_location(connection,4,0x01,0x32);

	close_claw(connection);

	wait_until_done(connection,15, 2000000);
	//going back up (bricked function)
	move_to_location(connection,2,0x01,0xf4);
	wait_until_done(connection,15, 2000000);
	move_to_location(connection,1,0x01,0xf0); 
	move_to_location(connection,3,0x02,0x1e);
	move_to_location(connection,4,0x01,0xfd);

	wait_until_done(connection,15,2000000);
	//place block y in tower 2
	move_to_location(connection,2,0x01,0x1c);
	wait_until_done(connection,15, 2000000);
	move_to_location(connection,1,0x01,0xfd); 
	move_to_location(connection,3,0x01,0x56);
	move_to_location(connection,4,0x01,0x2f);



	return 0;

}



