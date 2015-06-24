#if !defined TRISENGINE_INCLUDE_
#define TRISENGINE_INCLUDE_

//#include "trispawn.h" inutile, incluso da trisfield.h
#include "trisswitch.h"
#include "trisfield.h"

// Tris Engine statuses
#define TE_STAT_INIT  0
#define TE_STAT_WAIT_1ST_MOVE  1
#define TE_STAT_DO_MOVE  2
#define TE_STAT_WAIT_PAWN  3
#define TE_STAT_WAIT_MOVE  4
#define TE_STAT_GAME_WON   5
#define TE_STAT_GAME_LOST  6
#define TE_STAT_UNKNONW     7

typedef enum {

    TRIS_GAMETYPE_NONE,
    TRIS_GAMETYPE_FIRSTANGLE,
    TRIS_GAMETYPE_FIRST_SIDECENTER,
    TRIS_GAMETYPE_FIRST_CENTER,

    TRIS_GAMETYPE_UNKNOWN


} TrisGameType;

typedef enum {
    TRIS_GAMERESULT_NONE,
    TRIS_GAMERESULT_WON,
    TRIS_GAMERESULT_LOST,

    TRIS_GAMERESULT_UNKNOWN

} TrisGameResult_t;


// ***********************************************************************
// ***********************************************************************
class TrisEngine
{

    public:
        TrisEngine();

        void ResetGame();
        int SetStatus(int new_estat);

        int GetStatus() const
        { return this->te_stat; };

        TrisGameResult_t    GetGameResult() const
        { return this->game_result; };
        void SetGameResult(TrisGameResult_t res)
        { this->game_result = res; };

        int CheckGameWon() const
        { 
            // FIXME --- Questa funzione andrebbe riscritta
            // in modo che calcoli anche la variabile game_result
            // Attualmente per come e' scritto il codice la
            // variabile non cambia mai
            if (this->game_result == TRIS_GAMERESULT_WON) 
                return 1;
            return 0;
        };

        int Compute(int field_changed,
                const TrisFieldStatus_t *p_fs,
                int switch_changed,
                const TrisSwitchStatus_t *p_ss);
        int CheckStatusChange(int field_changed,
                const TrisFieldStatus_t *p_fs,
                int switch_changed,
                const TrisSwitchStatus_t *p_ss);
        int CheckSinglePawnPlacement(const TrisFieldStatus_t *p_fs, 
                int curr_stat);
        int CheckExpectedPawnPlacement(const TrisFieldStatus_t *p_fs, 
                int curr_stat);

        static int IsValidStat(int stat)
        {
            if ((stat >= TE_STAT_INIT) && (stat < TE_STAT_UNKNONW))
                return 1;
            return 0;
        };


    private:

        // ----------------------------------------------------------------
        int DoMove();
        int AddPawnMove(const TrisFieldCoords_t *p_pcoords, int curr_stat);

        // ----------------------------------------------------------------
        int                 te_stat;
        TrisGameResult_t    game_result;

        // pending move data (for wait pawn state)
        TrisFieldCoords_t   pmove_coords;

        TrisMovesSequence_t mseq;

};

#endif    // #if !defined TRISENGINE_INCLUDE_

