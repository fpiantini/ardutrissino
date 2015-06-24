#if !defined TRISSWITCH_INCLUDE_
#define TRISSWITCH_INCLUDE_

// ---------------------------------------------
#define A3INO_NUM_SWITCHES  2

#define SW_EVENT_NONE       0
#define SW_EVENT_PRESSED    1
#define SW_EVENT_RELEASED   2

typedef struct {

  // switches status
  int prev_stat[A3INO_NUM_SWITCHES];
  int curr_stat[A3INO_NUM_SWITCHES];

  // status changed flag
  int schanged[A3INO_NUM_SWITCHES];
  int sw_event[A3INO_NUM_SWITCHES];
  int changed_flag;

} TrisSwitchStatus_t;

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
void InitSwitchesInfo(TrisSwitchStatus_t *p_ss);

#endif // #if !defined TRISSWITCH_INCLUDE_



