#include "trisfield.h"

// -----------------------------------------------------------------
void InitFieldInfo(TrisFieldStatus_t *p_fs)
{
  int row, col;

  for (row = 0; row < A3INO_NUM_ROWS; row++) {
    for (col = 0; col < A3INO_NUM_ROWS; col++) {
      p_fs->prev_bstat[row][col] = -1;
      p_fs->bstat[row][col] = -1;
    }
  }

  p_fs->board_clear = -1;
}

// -----------------------------------------------------------------
void ManageFieldChange(TrisFieldStatus_t *p_fs)
{
    int row, col;
    TrisPawnEvent_t ev_to_add;

    // -----------------------------------------------

    EventSetReset(&(p_fs->fld_events));
    // extracts events and save them in events array
    for (row = 0; row < A3INO_NUM_ROWS; row++) {
        for (col = 0; col < A3INO_NUM_COLS; row++) {
            if (p_fs->bstat[row][col] != p_fs->prev_bstat[row][col]) {
                ev_to_add.pcoords.row = row;
                ev_to_add.pcoords.col = col;
                if (p_fs->bstat[row][col] == 0)
                    ev_to_add.ev_code = FEVENT_PAWN_ADDED;
                else
                    ev_to_add.ev_code = FEVENT_PAWN_REMOVED;

                AddEventToSet(&(p_fs->fld_events), &(ev_to_add));

            }
        }
    }
}



