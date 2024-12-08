#include "dynamixel.h"
#include <stdio.h>
#include <unistd.h>

// Macros for servo IDs
#define CLAW_ID 5
#define ARM_BASE_ID 1
#define ARM_JOINT_1_ID 2
#define ARM_JOINT_2_ID 3
#define ARM_JOINT_3_ID 4

// Constants for timing
#define DEFAULT_WAIT 5
#define CLAW_WAIT 3
#define MAX_BLOCKS 3

// Track the number of blocks at each tower
int tower_heights[4] = {0, MAX_BLOCKS, 0, 0}; // Tower 0 unused; Towers 1, 2, 3

// Position functions for block heights
int position_x(int connection) {
    move_to_location(connection, 2, 0x00, 0xbe);
    move_to_location(connection, 3, 0x02, 0x0a);
    move_to_location(connection, 4, 0x00, 0xd7);
    wait_until_done(connection, 15, 4);
    return 0;
}

int position_y(int connection) {
    move_to_location(connection, 2, 0x00, 0xf4);
    move_to_location(connection, 3, 0x01, 0xe4);
    move_to_location(connection, 4, 0x00, 0xd1);
    wait_until_done(connection, 15, 4);
    return 0;
}

int position_z(int connection) {
    move_to_location(connection, 2, 0x00, 0xfa);
    move_to_location(connection, 3, 0x02, 0x09);
    move_to_location(connection, 4, 0x00, 0xaa);
    wait_until_done(connection, 15, 4);
    return 0;
}

// Adjust robot to specific block height
void adjust_to_block_height(int connection, int block_height) {
    if (block_height == 1) {
        position_x(connection); // Bottom block
    } else if (block_height == 2) {
        position_y(connection); // Middle block
    } else if (block_height == 3) {
        position_z(connection); // Top block
    } else {
        fprintf(stderr, "Error: Invalid block height %d\n", block_height);
    }
}

// Claw operations
void open_claw(int connection) {
    move_to_location(connection, CLAW_ID, 0x01, 0xff);
    wait_until_done(connection, CLAW_ID, CLAW_WAIT);
}

void close_claw(int connection) {
    move_to_location(connection, CLAW_ID, 0x01, 0x40);
    wait_until_done(connection, CLAW_ID, CLAW_WAIT);
}

void move_to_tower(int connection, int tower_id) {
    unsigned char tower_positions[][2] = {
        {0x00, 0x00}, // Tower 0 unused
        {0x01, 0xa0}, // Tower 1
        {0x02, 0x00}, // Tower 2
        {0x02, 0x59}  // Tower 3
    };
    if (tower_id >= 1 && tower_id <= 3) {
        move_to_location(connection, ARM_BASE_ID, tower_positions[tower_id][0], tower_positions[tower_id][1]);
        wait_until_done(connection, ARM_BASE_ID, DEFAULT_WAIT);
    }
}

// Raise the arm to a safe position
void brick_up(int connection) {
    move_to_location(connection, ARM_JOINT_1_ID, 0x01, 0x9a);
    wait_until_done(connection, ARM_JOINT_1_ID, 2);
}

// Move block from one tower to another
void move_block(int connection, int source, int destination) {
    move_to_tower(connection, source);
    adjust_to_block_height(connection, tower_heights[source]); // Adjust to top block
    close_claw(connection); // Pick up the block
    // tower_heights[source]--; // Update source height
    brick_up(connection);

    move_to_tower(connection, destination);
    adjust_to_block_height(connection, tower_heights[destination] + 1); // Adjust to next empty slot
    open_claw(connection); // Place the block
    // tower_heights[destination]++; // Update destination height
    brick_up(connection);
}

// Recursive Tower of Hanoi solver
void solve_hanoi(int connection, int n, int source, int auxiliary, int destination) {
    if (n == 1) {
        move_block(connection, source, destination);
        return;
    }

    solve_hanoi(connection, n - 1, source, destination, auxiliary);
    move_block(connection, source, destination);
    solve_hanoi(connection, n - 1, auxiliary, source, destination);
}

// Main function
int main(int argc, char *argv[]) {
    int connection = open_connection("/dev/ttyUSB0", B1000000);
    if (connection < 0) {
        fprintf(stderr, "Error: Unable to open connection\n");
        return -1;
    }

    solve_hanoi(connection, MAX_BLOCKS, 1, 2, 3);

    close_connection(connection);
    return 0;
}
