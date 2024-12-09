#include "dynamixel.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Macros for servo IDs
#define CLAW_ID 5
#define ARM_BASE_ID 1
#define ARM_JOINT_1_ID 2
#define ARM_JOINT_2_ID 3
#define ARM_JOINT_3_ID 4
#define MAX_BLOCKS 3

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
   wait_until_done(connection,100, 3);
   return 0;
}

int close_claw(int connection){
   move_to_location(connection,5,0x01,0x40);
   wait_until_done(connection,15, 3);
   return 0;
}

int return_to_base(int connection){
   move_to_location(connection,2,0x01,0x9a);
   wait_until_done(connection,15, 2);
   move_to_location(connection,1,0x02,0x00); 
   move_to_location(connection,3,0x01,0x8f);
   move_to_location(connection,4,0x00,0x99);
   return 0;
}

int move_to_height(int connection, char plane){
    unsigned char plane_positions[][2] = {
        {0x00, 0xfa}, // Motor 2
        {0x02, 0x09}, // Motor 3
        {0x00, 0xaa}  // Motor 4
    };
    switch(plane)
    {
        case 'x': 
            plane_positions[0][0] = 0x00;
            plane_positions[0][1] = 0xbe;
            plane_positions[1][0] = 0x02;
            plane_positions[1][1] = 0x0a;
            plane_positions[2][0] = 0x00;
            plane_positions[2][1] = 0xd7;
            break;
        case 'y':
            plane_positions[0][0] = 0x00;
            plane_positions[0][1] = 0xf4;
            plane_positions[1][0] = 0x01;
            plane_positions[1][1] = 0xe4;
            plane_positions[2][0] = 0x00;
            plane_positions[2][1] = 0xd1;
            break;
        default:
            break;
    }
    move_to_location(connection,2,plane_positions[0][0],plane_positions[0][1]);
    move_to_location(connection,3,plane_positions[1][0],plane_positions[1][1]);
    move_to_location(connection,4,plane_positions[2][0],plane_positions[2][1]);
    wait_until_done(connection,15, 4);
    return 0;
}


void move_to_pile(int connection, int tower_id) {
   unsigned char tower_positions[][2] = {
       {0x01, 0xa0}, // Tower 1
       {0x02, 0x00}, // Tower 2
       {0x02, 0x59}  // Tower 3
   };
   if (tower_id >= 1 && tower_id <= 3) {
       move_to_location(connection, ARM_BASE_ID, tower_positions[tower_id - 1][0], tower_positions[tower_id - 1][1]);
       wait_until_done(connection, ARM_BASE_ID, 3);
   }
}

// Move block from one tower to another
void move_block(int connection, int source_pile, int destination_pile, char source_height, char destination_height) {
    move_to_pile(connection, source_pile);  // Go to source tower
    move_to_height(connection, source_height); // Lower to pick up the block
    close_claw(connection); // Grab the block
    return_to_base(connection); // Raise the block

    move_to_pile(connection, destination_pile); // Go to destination tower
    move_to_height(connection, destination_height); // Lower to place the block
    open_claw(connection); // Release the block
    return_to_base(connection); // Raise the arm
}

int main(int argc, char* argv[]) {
    int connection = open_connection("/dev/ttyUSB0",B1000000);

    return_to_base(connection);
    open_claw(connection);

    move_block(connection, 1, 3, 'z', 'x');
    move_block(connection, 1, 2, 'y', 'x');
    move_block(connection, 3, 2, 'x', 'y');
    move_block(connection, 1, 3, 'x', 'x');
    move_block(connection, 2, 1, 'y', 'x');
    move_block(connection, 2, 3, 'x', 'y');
    move_block(connection, 1, 3, 'x', 'z');
}
