#include "trispawn.h"

// *******************************************************************************************
// *******************************************************************************************
// --------------------------------------------------------
int TBPos_IsAngle(const TrisFieldCoords_t *p_pos)
{
    int ret = 0;

    if (((p_pos->row == 1) || (p_pos->row == 3)) &&
        ((p_pos->col == 1) || (p_pos->col == 3)))
        ret = 1;

    return ret;
}


// --------------------------------------------------------
int TBPos_IsSideCenter(const TrisFieldCoords_t *p_pos)
{
    int dum;
    int ret = 0;

    // ----------------------------
    dum = p_pos->row + p_pos->col;
    if ((dum == 3) || (dum == 5))
        ret = 1;

    return ret;
}

// --------------------------------------------------------
int TBPos_IsCenter(const TrisFieldCoords_t *p_pos)
{
    int ret = 0;

    if ((p_pos->row == 2) &&
        (p_pos->col == 2))
        ret = 1;

    return ret;
}

// --------------------------------------------------------
int TBPos_GetType(const TrisFieldCoords_t *p_pos)
{
    if (TBPos_IsAngle(p_pos) == 1)
        return PAWN_POS_ANGLE;
    else if (TBPos_IsSideCenter(p_pos) == 1)
        return PAWN_POS_SIDECENTER;
    else if (TBPos_IsCenter(p_pos) == 1)
        return PAWN_POS_CENTER;

    return PAWN_POS_NONE;
}


// --------------------------------------------------------
void TBPos_OppositeAngle(const TrisFieldCoords_t *p_pos, 
                                    TrisFieldCoords_t *p_opp)
{
    p_opp->row = -1;
    p_opp->col = -1;

    if (TBPos_IsAngle(p_pos) == 1) {
        p_opp->row = 4 - p_pos->row;
        p_opp->col = 4 - p_pos->col;
    }
}

// --------------------------------------------------------
void TBPos_OppositeSideCenter(const TrisFieldCoords_t *p_pos,
                                    TrisFieldCoords_t *p_opp)
{

    p_opp->row = -1;
    p_opp->col = -1;

    if (TBPos_IsSideCenter(p_pos) == 1) {
        p_opp->row = 4 - p_pos->row;
        p_opp->col = 4 - p_pos->col;
    }

}

// *******************************************************************************************
// *******************************************************************************************
// -----------------------------------------------------------------
void MoveSequenceInit(TrisMovesSequence_t *p_ms)
{
    p_ms->num_moves = 0;
}

// -----------------------------------------------------------------
int MoveSequenceAddMove(TrisMovesSequence_t *p_ms, const TrisFieldCoords_t *p_pos)
{
    if (p_ms->num_moves >= A3INO_MAX_NUM_MOVES)
        return -1;

    p_ms->moves[p_ms->num_moves] = *p_pos;
    ++(p_ms->num_moves);
    
    return 0;
}

// *******************************************************************************************
// *******************************************************************************************
// -----------------------------------------------------------------
void EventSetReset(TrisPawnEventSet_t *p_pes)
{
    p_pes->num_events = 0;
}

// -----------------------------------------------------------------
int  AddEventToSet(TrisPawnEventSet_t *p_pes, const TrisPawnEvent_t *p_ev)
{
    if (p_pes->num_events >= A3INO_MAX_NUM_EVENTS)
        return -1;

    p_pes->tp_evs[p_pes->num_events] = *p_ev;
    ++(p_pes->num_events);

    return 0;
}





