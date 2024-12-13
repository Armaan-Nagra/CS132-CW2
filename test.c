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
#define MAX_BLOCKS 4

// Number of current blocks in each pile/tower 
int tower1_plane = MAX_BLOCKS; 
int tower2_plane = 0;
int tower3_plane = 0;

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

int position(int connection, int plane){
    unsigned char plane_positions[][2] = {
        {0x00, 0xfa}, // Motor 2
        {0x02, 0x09}, // Motor 3
        {0x00, 0xaa}  // Motor 4
    };
    switch(plane)
    {
        case 1: 
            plane_positions[0][0] = 0x00;
            plane_positions[0][1] = 0xbe;
            plane_positions[1][0] = 0x02;
            plane_positions[1][1] = 0x0a;
            plane_positions[2][0] = 0x00;
            plane_positions[2][1] = 0xd7;
            break;
        case 2:
            plane_positions[0][0] = 0x00;
            plane_positions[0][1] = 0xf4;
            plane_positions[1][0] = 0x01;
            plane_positions[1][1] = 0xe4;
            plane_positions[2][0] = 0x00;
            plane_positions[2][1] = 0xd1;
            break;
        case 4:
            plane_positions[0][0] = 0x01;
            plane_positions[0][1] = 0x3b;
            plane_positions[1][0] = 0x01;
            plane_positions[1][1] = 0xb4;
            plane_positions[2][0] = 0x00;
            plane_positions[2][1] = 0xcc;
        default:
            break;
    }
    move_to_location(connection,2,plane_positions[0][0],plane_positions[0][1]);
    move_to_location(connection,3,plane_positions[1][0],plane_positions[1][1]);
    move_to_location(connection,4,plane_positions[2][0],plane_positions[2][1]);
    wait_until_done(connection,15, 4);
    return 0;
}


void move_to_tower(int connection, int tower_id) {
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
void move_block(int connection, int source, int destination, char source_position, char destination_position) {
    move_to_tower(connection, source);  // Go to source tower
    position(connection, source_position); // Lower to pick up the block
    close_claw(connection);            // Grab the block
    return_to_base(connection);              // Raise the block

    move_to_tower(connection, destination); // Go to destination tower
    position(connection, destination_position); // Lower to place the block
    open_claw(connection);                  // Release the block
    return_to_base(connection);                   // Raise the arm

    // Update the values of the number of blocks in
    // each tower/pile
    if (source == 1)
        tower1_plane --;
    else if (source == 2)
        tower2_plane --;
    else
        tower3_plane --;

    if (destination == 1)
        tower1_plane ++;
    else if (destination == 2)
        tower2_plane ++;
    else
        tower3_plane ++;
}

// Return the position of the top block on the argument pile/tower
int find_plane(int connection, int tower){
    if (tower == 1)
        return tower1_plane;
    else if (tower == 2)
        return tower2_plane;
    else
        return tower3_plane;
}

// Recursive Tower of Hanoi
void solve_hanoi(int connection, int n, int source, int auxiliary, int destination) {
    // Shift last block from pile 1 to pile 3
    if (n == 1) {
        // Find the position of the top block on the source pile/tower
        int source_plane = find_plane(connection, source);
               
        // Calculate the position the block should be placed on the destination pile/tower
        int destination_plane = find_plane(connection, destination) + 1;
               
        // Execute the current move
        move_block(connection, source, destination, source_plane, destination_plane);
        return;
    }
    // Shift n-1 blocks from pile 1 to pile 2, using pile 3
    solve_hanoi(connection, n - 1, source, destination, auxiliary);
           
    // Find the position of the top block on the source pile/tower
    int source_plane = find_plane(connection, source); 
           
    // Calculate the position the block should be placed on the destination pile/tower
    int destination_plane = find_plane(connection, destination) + 1;
           
    // Execute current move
    move_block(connection, source, destination, source_plane, destination_plane);
           
    // Shift n-1 blocks from pile 2 to pile 3, using pile 1
    solve_hanoi(connection, n - 1, auxiliary, source, destination);
}

// Main function
int main(int argc, char *argv[]) {
    int connection = open_connection("/dev/ttyUSB0", B1000000);
           
    // Initialize start up sequence
    return_to_base(connection);
    open_claw(connection);
           
    // Initiate recursive algorithm
    solve_hanoi(connection, MAX_BLOCKS, 1, 2, 3);

    return 0;
}   
