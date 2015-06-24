#if !defined TRISFIELD_INCLUDE_
#define TRISFIELD_INCLUDE_

#include "trisdefs.h"
#include "trispawn.h"

#define A3INO_NUM_ROWS 3
#define A3INO_NUM_COLS 3

// ---------------------------------------------
typedef struct {

  // previous board status
  int prev_bstat[A3INO_NUM_ROWS][A3INO_NUM_COLS];

  // board status
  int bstat[A3INO_NUM_ROWS][A3INO_NUM_COLS];

  // last events information
  TrisPawnEventSet_t    fld_events;

  // high-level informations
  int board_clear;


} TrisFieldStatus_t;

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
void InitFieldInfo(TrisFieldStatus_t *p_fs);
void ManageFieldChange(TrisFieldStatus_t *p_fs);

#endif // #if !defined TRISFIELD_INCLUDE_





