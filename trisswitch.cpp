
#include "trisswitch.h"

// -----------------------------------------------------------------
void InitSwitchesInfo(TrisSwitchStatus_t *p_ss)
{
  int cnt;

  // switches status
  for (cnt = 0; cnt < A3INO_NUM_SWITCHES; cnt++) {
    p_ss->prev_stat[cnt] = -1;
    p_ss->curr_stat[cnt] = -1;
    p_ss->schanged[cnt] = false;
    p_ss->sw_event[cnt] = SW_EVENT_NONE;
  }
  p_ss->changed_flag = false;

}

