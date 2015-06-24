#if !defined TRISGAMEBOARD_INCLUDE_
#define TRISGAMEBOARD_INCLUDE_

#include "trisdefs.h"

// ***********************************************************************
// ***********************************************************************
typedef enum {
    TRIS_GBCELLSTAT_FREE,
    TRIS_GBCELLSTAT_P1,
    TRIS_GBCELLSTAT_P2

} TrisGameBoardCellStat_t;

class GameBoardStatus
{
    public:
        GameBoardStatus();
        ~GameBoardStatus() {};

        void SetCellStatus(int row, int col, TrisGameBoardCellStat_t cs);
        int DetectTris(int player_ndx);

    private:
        TrisGameBoardCellStat_t gbs[A3INO_NUM_ROWS][A3INO_NUM_COLS];

};


#endif     // #if !defined TRISGAMEBOARD_INCLUDE_

