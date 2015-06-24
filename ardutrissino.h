#if !defined ARDUTRISSINO_INCLUDE
#define ARDUTRISSINO_INCLUDE

// Hardware related --- --- --- --- --- --- --- --- ---  ---
#define A3INO_NUM_LEDS (A3INO_NUM_ROWS + A3INO_NUM_COLS)

#define ROW_DRIVE_ADIUST_TIME_MS  20
#define ROW_UNDRIVE_ADIUST_TIME_MS  ROW_DRIVE_ADIUST_TIME_MS

#include "trisengine.h"
#include "trisswitch.h"
#include "trisleds.h"
#include "trisfield.h"

// ---------------------------------------------
typedef struct {

  // outputs used to drive rows
  int row_out_lines[A3INO_NUM_ROWS];

  // input used as hall sensor inputs
  int col_in_lines[A3INO_NUM_COLS];

  // inputs used to read switch status
  int switch_lines[A3INO_NUM_SWITCHES];

  // outputs used to drive row leds
  int row_led_lines[A3INO_NUM_ROWS];

  // outputs used to drive column leds
  int col_led_lines[A3INO_NUM_COLS];


} TrisHardwareInfo_t;

// ---------------------------------------------
typedef struct {

    // HARDWARE information
    TrisHardwareInfo_t  hw_info;

    // BATTLE FIELD status
    TrisFieldStatus_t   field_stat;

    // SWITCHES status
    TrisSwitchStatus_t  sw_stat;

    // LEDS status
    TrisLedStatus_t     led_stat;

    // ENGINE status
    TrisEngine          teng;

} TrisBoardStatus_t;



#endif   // #if !defined HALL_DEFS_INCLUDE

// *************************************************************
// *************************************************************
// Development soundtrack:
//   - Avishai Cohen, miscellaneous...
// *************************************************************
// *************************************************************


