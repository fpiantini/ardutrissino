#if !defined TRISLEDS_INCLUDE_
#define TRISLEDS_INCLUDE_

// ---------------------------------------------
#define LED_EFFECT_NONE     0
#define LED_EFFECT_BLINK    1
#define LED_EFFECT_ROTATE   2
#define LED_EFFECT_ALLON    3
#define LED_EFFECT_ALLOFF   4

typedef struct {

  // leds effect mode
  int led_effect_mode;

  // blinking info
  int blink_cnt;
  int blink_cur_stat;

  // rotation info
  int rot_cnt;
  int rot_led_ndx;
  
} TrisLedStatus_t;

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
void InitLedInfo(TrisLedStatus_t  *p_ls);

#endif // #if !defined TRISLEDS_INCLUDE_




