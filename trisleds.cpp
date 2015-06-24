#include "trisleds.h"

// -----------------------------------------------------------------
void InitLedInfo(TrisLedStatus_t  *p_ls)
{

  // leds effect mode
  p_ls->led_effect_mode = LED_EFFECT_NONE;

  // blinking info
  p_ls->blink_cnt = 0;
  p_ls->blink_cur_stat = -1;

  // rotation info
  p_ls->rot_cnt = 0;
  p_ls->rot_led_ndx = -1;
  
}



