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

void wait_until_done(int connection, unsigned char id, int time) {
	usleep(time*1000000);
}

int open_claw(int connection) {
	move_to_location(connection,5,0x01,0xff);
	wait_until_done(connection,100, 0.5);
	return 0;
}

int close_claw(int connection){
	move_to_location(connection,5,0x01,0x40);
	wait_until_done(connection,15, 0.5);
	return 0;
}

int brickUp(int connection){
	move_to_location(connection,2,0x01,0x9a);
	wait_until_done(connection,15, 2);
	move_to_location(connection,1,0x02,0x00); 
	move_to_location(connection,3,0x01,0x8f);
	move_to_location(connection,4,0x00,0x99);

	return 0;
}

int position_x(int connection){
	move_to_location(connection,2,0x00,0xbe);
	move_to_location(connection,3,0x02,0x0a);
	move_to_location(connection,4,0x00,0xd7);
	wait_until_done(connection,15, 2);
	return 0;
}

int position_y(int connection){
	move_to_location(connection,2,0x00,0xf4);
	move_to_location(connection,3,0x01,0xe4);
	move_to_location(connection,4,0x00,0xd1);
	wait_until_done(connection,15, 2);
	return 0;
}

int position_z(int connection){
	move_to_location(connection,2,0x00,0xfa);
	move_to_location(connection,3,0x02,0x09);
	move_to_location(connection,4,0x00,0xaa);
	wait_until_done(connection,15, 2);
	return 0;
}


int tower_1(int connection){
	move_to_location(connection,1,0x01,0xa0); 
	wait_until_done(connection,15, 1);
	return 0;
}

int tower_2(int connection){
	move_to_location(connection,1,0x02,0x00); 
	wait_until_done(connection,15, 1);
	return 0;
}

int tower_3(int connection){
	move_to_location(connection,1,0x02,0x59); 
	wait_until_done(connection,15, 1);
	return 0;
}


int main(int argc, char* argv[]) {

	int connection = open_connection("/dev/ttyUSB0",B1000000);
	// initialisation
	brickUp(connection);
	open_claw(connection);

	// take block z from tower 1
	tower_1(connection);
	position_z(connection);
	close_claw(connection);
	brickUp(connection);
	
	//place previous block to block x of tower 3
	tower_3(connection);
	position_x(connection);
	open_claw(connection);
	brickUp(connection);

	tower_1(connection);
	position_y(connection);
	close_claw(connection);
	brickUp(connection);

	tower_2(connection);
	position_x(connection);
	open_claw(connection);
	brickUp(connection);

	tower_3(connection);
	position_x(connection);
	close_claw(connection);
	brickUp(connection);

	tower_2(connection);
	position_y(connection);
	open_claw(connection);
	brickUp(connection);

	tower_1(connection);
	position_x(connection);
	close_claw(connection);
	brickUp(connection);

	tower_3(connection);
	position_x(connection);
	open_claw(connection);
	brickUp(connection);

	tower_2(connection);
	position_y(connection);
	close_claw(connection);
	brickUp(connection);

	tower_1(connection);
	position_x(connection);
	open_claw(connection);
	brickUp(connection);

	tower_2(connection);
	position_x(connection);
	close_claw(connection);
	brickUp(connection);

	tower_3(connection);
	position_y(connection);
	open_claw(connection);
	brickUp(connection);

	tower_1(connection);
	position_x(connection);
	close_claw(connection);
	brickUp(connection);

	tower_3(connection);
	position_z(connection);
	open_claw(connection);
	brickUp(connection);

}



