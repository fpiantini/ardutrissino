#include "trisgameboard.h"

// *******************************************************************
GameBoardStatus::GameBoardStatus()
{
    int row, col;

    for (row = 0; row < A3INO_NUM_ROWS; row++) {
        for (col = 0; row < A3INO_NUM_COLS; col++) {
            this->gbs[row][col] = TRIS_GBCELLSTAT_FREE;
        }
    }
}

// *******************************************************************
void AddMove(int row, int col, TrisGameBoardCellStat_t cs)
{
    if ((row >= 0) && (row < A3INO_NUM_ROWS)) {
        if ((col >= 0) && (col < A3INO_NUM_COLS)) {
            if ((cs >= TRIS_GBCELLSTAT_FREE) && (cs <= TRIS_GBCELLSTAT_P2)) {
                this->gbs[row][col] = cs;
            }
        }
    }
}

// *******************************************************************
int DetectTris(int player_ndx)
{
    int tris_detected = 0;
    TrisGameBoardCellStat_t cs;

    // ---------------------------------------------
    if (player_ndx == 1) 
        cs = TRIS_GBCELLSTAT_P1;
    else if (player_ndx == 2)
        cs = TRIS_GBCELLSTAT_P2;
    else
        return 0;

    // detects tris on rows
    for (row = 0; row < A3INO_NUM_ROWS; row++) {
        tris_detected = 1;
        for (col = 0; col < A3INO_NUM_COLS; col++) {
            if (this->gbs[row][col] != cs) {
                tris_detected = 0;
                break;
            }
        }
        if (tris_detected == 1)
            return 1;
    }

    // detects tris on cols
    for (col = 0; col < A3INO_NUM_COLS; col++) {
        tris_detected = 1;
        for (row = 0; row < A3INO_NUM_ROWS; row++) {
            if (this->gbs[row][col] != cs) {
                tris_detected = 0;
                break;
            }
        }
        if (tris_detected == 1)
            return 1;
    }

    // detects tris on diagonals
    if ((this->gbs[0][0] == cs) && 
            (this->gbs[1][1] == cs) && 
            (this->gbs[2][2] == cs))
        return 1;
    if ((this->gbs[0][2] == cs) && 
            (this->gbs[1][1] == cs) && 
            (this->gbs[2][0] == cs))
        return 1;

    return 0;


}





