// this file is named c++ only because makefile is not
// able to link program correctly when extension is c
#include "trisengine.h"

// -----------------------------------------------------------------
// -----------------------------------------------------------------
//     PUBLIC METHODS
// -----------------------------------------------------------------
// -----------------------------------------------------------------
TrisEngine::TrisEngine()
{
    // initialize engine status
    this->SetStatus(TE_STAT_INIT);

    // initialize moves sequence
    MoveSequenceInit(&(this->mseq));
}


// -----------------------------------------------------------------
void TrisEngine::ResetGame()
{
    this->pmove_coords.row = -1;
    this->pmove_coords.col = -1;
    this->SetGameResult(TRIS_GAMERESULT_NONE);
}


// -----------------------------------------------------------------
int TrisEngine::SetStatus(int new_estat)
{
    // if new status is equal to current, exits
    if (new_estat == this->te_stat)
        return 0;

    // if new status is not valid, exits
    if (TrisEngine::IsValidStat(new_estat) == 0)
        return 0;

    this->te_stat = new_estat;
    if (new_estat == TE_STAT_INIT)
        this->ResetGame();

    return 1;
}


// -----------------------------------------------------------------
int TrisEngine::Compute(int field_changed,
                const TrisFieldStatus_t *p_fs,
                int switch_changed,
                const TrisSwitchStatus_t *p_ss)
{
    int new_stat;
    int stat_changed = false;
    int stat_changed_after_move = false;

    // TO BE COMPLETED
    // if nothing changed in field and switches
    // do nothing, just manage leds flashing if necessary 
    if ((field_changed == true) || (switch_changed == true)) {

        // checks for status change
        new_stat = this->CheckStatusChange(field_changed, p_fs,
                switch_changed, p_ss);

        stat_changed = this->SetStatus(new_stat);
        if (new_stat == TE_STAT_DO_MOVE) {
            // do the move!
            new_stat = this->DoMove();
            stat_changed_after_move = this->SetStatus(new_stat);
        }
    }
    return (stat_changed | stat_changed_after_move);
}

// -----------------------------------------------------------------
int TrisEngine::CheckStatusChange(int field_changed,
                const TrisFieldStatus_t *p_fs,
                int switch_changed,
                const TrisSwitchStatus_t *p_ss)
{
    int new_stat;

    // ---------------------------------------------------------
    new_stat = this->te_stat;

    switch (this->te_stat) {

        // - - - - - - - - - - - - - - - - - - - - - - - - -
        case TE_STAT_INIT:
            // if in init state, and board is not clear,
            // ignore any action
            if (p_fs->board_clear == true) {
                // if SW1 is pressed go in "wait first move" status
                // if SW2 is pressed go in "do move" status
                if (p_ss->sw_event[0] == SW_EVENT_PRESSED)
                   new_stat = TE_STAT_WAIT_1ST_MOVE;
                else if (p_ss->sw_event[1] == SW_EVENT_PRESSED) {
                   new_stat = TE_STAT_DO_MOVE;
                }
            }
            break;

        // - - - - - - - - - - - - - - - - - - - - - - - - -
        case TE_STAT_WAIT_1ST_MOVE:
            // if SW2 pressed, check if board is
            // clear and go in do move. If board is
            // not clear returns to init...
            if (p_ss->sw_event[1] == SW_EVENT_PRESSED) {
                if (p_fs->board_clear == true)
                    new_stat = TE_STAT_DO_MOVE;
                else
                    new_stat = TE_STAT_INIT;
                break;
            }

            if (field_changed == true) {
                // Checks for single pawn placement
                new_stat = this->CheckSinglePawnPlacement(
                                    p_fs, new_stat);
            }
            break;

        // - - - - - - - - - - - - - - - - - - - - - - - - -
        case TE_STAT_DO_MOVE:
            // impossible... DO_MOVE is just a transient status
            break;

        // - - - - - - - - - - - - - - - - - - - - - - - - -
        case TE_STAT_WAIT_PAWN:
            if (field_changed == true) {
                // Checks for correct pawn placement
                new_stat = this->CheckExpectedPawnPlacement(
                                    p_fs, new_stat);
            }
            break;

        // - - - - - - - - - - - - - - - - - - - - - - - - -
        case TE_STAT_WAIT_MOVE:
            if (field_changed == true) {
                new_stat = this->CheckSinglePawnPlacement(
                                    p_fs, new_stat);
            }
            break;

        // - - - - - - - - - - - - - - - - - - - - - - - - -
        default:
            // invalid state, do nothing...
            break;

    }

    return new_stat;

}

// -----------------------------------------------------------------
int TrisEngine::CheckSinglePawnPlacement(const TrisFieldStatus_t *p_fs, 
                int curr_stat)
{
    int new_stat;

    // ------------------------
    new_stat = curr_stat;

    // if a single pawn is placed on field, get
    // the move, pass it to the engine and go in
    // do move status. If illegal move is
    // detected returns to init
    if (p_fs->fld_events.num_events == 1) {
        // pass the move to the engine and
        // goes in do move stautus
        if (p_fs->fld_events.tp_evs[0].ev_code == FEVENT_PAWN_ADDED) {
            new_stat = this->AddPawnMove(
                         &(p_fs->fld_events.tp_evs[0].pcoords), curr_stat);
        }
        else {
            // Pawn removed???
            new_stat = TE_STAT_INIT;
        }
    }
    else if (p_fs->fld_events.num_events > 1)
        new_stat = TE_STAT_INIT;

    return new_stat;
}

// -----------------------------------------------------------------
int TrisEngine::CheckExpectedPawnPlacement(const TrisFieldStatus_t *p_fs, 
                int curr_stat)
{
    int new_stat;
    int game_won;

    // ------------------------
    new_stat = TE_STAT_INIT;

    // Check if a single pawn was placed on field.
    // Checks if the pawn is placed in the requested position.
    // Checks if the game is won
    if (p_fs->fld_events.num_events == 1) {
        if (p_fs->fld_events.tp_evs[0].ev_code == FEVENT_PAWN_ADDED) {
            if ((p_fs->fld_events.tp_evs[0].pcoords.row == this->pmove_coords.row) &&
                 (p_fs->fld_events.tp_evs[0].pcoords.col == this->pmove_coords.col)) {
                game_won = this->CheckGameWon();
                if (game_won != 0)
                    new_stat = TE_STAT_GAME_WON;
                else
                    new_stat = TE_STAT_WAIT_MOVE;
            }
        }
    }
    return new_stat;
}


// -----------------------------------------------------------------
// -----------------------------------------------------------------
//     PRIVATE METHODS
// -----------------------------------------------------------------
// -----------------------------------------------------------------

// -----------------------------------------------------------------
int TrisEngine::DoMove()
{ 
    // FIXME --- TODO
    return TE_STAT_WAIT_MOVE;
}

// -----------------------------------------------------------------
int TrisEngine::AddPawnMove(const TrisFieldCoords_t *p_pcoords, int curr_stat)
{ 
    // FIXME --- TODO
    MoveSequenceAddMove(&(this->mseq), p_pcoords);
    return TE_STAT_DO_MOVE;
}


