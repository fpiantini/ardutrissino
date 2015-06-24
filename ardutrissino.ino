/*
 TRISBOARD program, by F. Piantini http://www.fpiantini.it/
 -----------------------------------------------------------
 Manages a 3x3 trisboard with piece recognition based on hall
 effect sensors
 -----------------------------------------------------------
 Current version: 0.0.3 (20140714)
 -----------------------------------------------------------
 HISTORY

 20140714 - Version 0.0.3: finalized first working version
 with an over-simplified losing engine
 
 20140323 - Version 0.0.2: upgrade to reflect definitive
 version of the TrisBoard Hardware. The new hardware has two
 switch to command software special statuses and six leds
 on rows and column to indicate software moves.
 Use of the digital I/Os:
  - 0...2: used as output to drive rows
  - 3...5: used as inputs to read columns status
  - 6...7: used as inputs to read switches
  - 8...13: used as outputs to drive leds
 Removed the use of Arduino board led on pin 13 to show pawn
 presence (conflicts with drive of external led)

 20140208 - Version 0.0.1: the first version just recognize
 pawn using hall effect sensors placed under the board.
 Arduino led on pin 13 is turned on when at least a pawn
 is detected, and complete board status is sent to serial
 console when a variation is detected
 
*/

#include "ardutrissino.h"

// initialization functions
static void InitTrisBoardStat(TrisBoardStatus_t *p_tbs, const TrisHardwareInfo_t *p_hw);

// Hardware related functions
static void HW_Init(TrisHardwareInfo_t *p_hw);
static int  HW_ReadColumnState(const TrisHardwareInfo_t *p_hw, int col);
static void HW_InitiateRowRead(const TrisHardwareInfo_t *p_hw, int row);
static void HW_TerminateRowRead(const TrisHardwareInfo_t *p_hw, int row);
static int  HW_ReadSwitch(const TrisHardwareInfo_t *p_hw, int sn);
static void HW_TurnLedByNdx(const TrisHardwareInfo_t *p_ls, int led_ndx, int level);


// Battle field management functions
static int  ReadFieldStatus(TrisFieldStatus_t *p_fs, const TrisHardwareInfo_t *p_hw);
static int  ReadRow(TrisFieldStatus_t *p_fs, const TrisHardwareInfo_t *p_hw, int row);

// Switches managements functions
static int ReadAllSwitches(TrisSwitchStatus_t *p_ss, const TrisHardwareInfo_t *p_hw);
static void ManageSwitchChange(const TrisSwitchStatus_t *p_ss);

// Engine management functions

// Leds managements functions
static void SetLedEffectByEngStatus(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw, int eng_status);
static void SetLedEffect(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw, int led_effect);
static void ManageLedsEffect(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw);
static void BlinkLeds(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw);
static void RotateLeds(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw);
static int ComputeNextLedInRotation(int led_ndx);

// Print to serial functions
static void PrintBanner();
static void printFieldStatus(const TrisFieldStatus_t *p_fs);
static void printSwitchesStatus(const TrisSwitchStatus_t *p_ss);

// **********************************************************
// VARIABLES --------------------------------
TrisHardwareInfo_t  thw;
TrisBoardStatus_t tbs;
// **********************************************************

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//    INITIALIZATION FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// -----------------------------------------------------------------
void setup()
{

    // init hardware
    HW_Init(&thw);

    // initialize board status
    InitTrisBoardStat(&tbs, &thw);

    // Print program banner to serial port
    PrintBanner();

}


// -----------------------------------------------------------------
static void InitTrisBoardStat(TrisBoardStatus_t *p_tbs, const TrisHardwareInfo_t *p_hw)
{
    // init battle field status
    InitFieldInfo(&(p_tbs->field_stat));

    // inits switches information
    InitSwitchesInfo(&(p_tbs->sw_stat));

    // inits leds information
    InitLedInfo(&(p_tbs->led_stat));

    // inits engine information
    SetLedEffectByEngStatus(&(p_tbs->led_stat), p_hw, p_tbs->teng.GetStatus());

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//    MAIN LOOP FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

// -----------------------------------------------------------------
//    MAIN LOOP
// -----------------------------------------------------------------
void loop() 
{
    int fstat_changed = false;
    int sstat_changed = false;
    int estat_changed = false;

    // update the state of the battle field
    fstat_changed = ReadFieldStatus(&(tbs.field_stat), &thw);
    if (fstat_changed == true) {
        printFieldStatus(&(tbs.field_stat));
        ManageFieldChange(&(tbs.field_stat));
    }

    // update the state of the switches
    sstat_changed = ReadAllSwitches(&(tbs.sw_stat), &thw);
    if (sstat_changed == true) {
        ManageSwitchChange(&(tbs.sw_stat));
    }

    // performs engine operations
    estat_changed = tbs.teng.Compute(fstat_changed, &(tbs.field_stat),
                                     sstat_changed, &(tbs.sw_stat));

    // updates leds
    if (estat_changed == true) {
        SetLedEffectByEngStatus(&(tbs.led_stat), &thw, tbs.teng.GetStatus());
    }

    ManageLedsEffect(&(tbs.led_stat), &thw);

}

// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *
//     HARDWARE MANAGEMENT FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *

// -----------------------------------------------------------------
static void HW_Init(TrisHardwareInfo_t *p_hw)
{
  int row, col, cnt;
  
  p_hw->row_out_lines[0] = 0;
  p_hw->row_out_lines[1] = 1;
  p_hw->row_out_lines[2] = 2;
  for (row = 0; row < A3INO_NUM_ROWS; row++) {
    pinMode(p_hw->row_out_lines[row], OUTPUT);
    digitalWrite(p_hw->row_out_lines[row], LOW); 
  }

  p_hw->col_in_lines[0] = 3;
  p_hw->col_in_lines[1] = 4;
  p_hw->col_in_lines[2] = 5;
  for (col = 0; col < A3INO_NUM_COLS; col++) {
    pinMode(p_hw->col_in_lines[col], INPUT);
  }

  // switches lines
  p_hw->switch_lines[0] = 6;
  p_hw->switch_lines[1] = 7;
  for (cnt = 0; cnt < A3INO_NUM_SWITCHES; cnt++) {
    pinMode(p_hw->switch_lines[cnt], INPUT);
  }

  // rows leds
  p_hw->row_led_lines[0] =13;
  p_hw->row_led_lines[1] = 12;
  p_hw->row_led_lines[2] = 11;
  for (row = 0; row < A3INO_NUM_ROWS; row++) {
    pinMode(p_hw->row_led_lines[row], OUTPUT);
    digitalWrite(p_hw->row_led_lines[row], LOW); 
  }
  
  // columns leds
  p_hw->col_led_lines[0] = 8;
  p_hw->col_led_lines[1] = 9;
  p_hw->col_led_lines[2] = 10;
  for (col = 0; col < A3INO_NUM_COLS; col++) {
    pinMode(p_hw->col_led_lines[col], OUTPUT);
    digitalWrite(p_hw->col_led_lines[col], LOW); 
  }
}

// -----------------------------------------------------------------
static int  HW_ReadColumnState(const TrisHardwareInfo_t *p_hw, int col) 
{
  return digitalRead(p_hw->col_in_lines[col]);  
}

// -----------------------------------------------------------------
static void HW_InitiateRowRead(const TrisHardwareInfo_t *p_hw, int row)
{
    digitalWrite(p_hw->row_out_lines[row], HIGH);
    delay(ROW_DRIVE_ADIUST_TIME_MS);
}

// -----------------------------------------------------------------
static void HW_TerminateRowRead(const TrisHardwareInfo_t *p_hw, int row)
{
    digitalWrite(p_hw->row_out_lines[row], LOW);
    delay(ROW_UNDRIVE_ADIUST_TIME_MS);
}

// -----------------------------------------------------------------
static int  HW_ReadSwitch(const TrisHardwareInfo_t *p_hw, int sn)
{
  return digitalRead(p_hw->switch_lines[sn]);
}

// --------------------------------------------------------
static void HW_TurnLedByNdx(const TrisHardwareInfo_t *p_hw, int led_ndx, int level)
{
  switch (led_ndx) {
    case 0:
      digitalWrite(p_hw->row_led_lines[0], level);
      break;
    case 1:
      digitalWrite(p_hw->row_led_lines[1], level);
      break;
    case 2:
      digitalWrite(p_hw->row_led_lines[2], level);
      break;
    case 3:
      digitalWrite(p_hw->col_led_lines[0], level);
      break;
    case 4:
      digitalWrite(p_hw->col_led_lines[1], level);
      break;
    case 5:
      digitalWrite(p_hw->col_led_lines[2], level);
      break;
    default:
      return;
      break;
  } 
} 



// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *
//     BATTLE FIELD MANAGEMENT FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *

// -----------------------------------------------------------------
// * * * * * * * * * * * * * * * * * * * * * * * * *
static int  ReadFieldStatus(TrisFieldStatus_t *p_fs, const TrisHardwareInfo_t *p_hw) 
{
    int changed = false;
    int row;

    p_fs->board_clear = true;
    for (row = 0; row < A3INO_NUM_ROWS; row++) {
        changed |= ReadRow(p_fs, p_hw, row);    
    }

    return changed;
}

// -----------------------------------------------------------------
static int  ReadRow(TrisFieldStatus_t *p_fs, const TrisHardwareInfo_t *p_hw, int row) 
{
    int changed = false;
    int col;

    // drive the ROW
    HW_InitiateRowRead(p_hw, row);


    for (col = 0; col < A3INO_NUM_COLS; col++) {

        p_fs->prev_bstat[row][col] = p_fs->bstat[row][col];

        p_fs->bstat[row][col] = HW_ReadColumnState(p_hw, col);
        if (p_fs->bstat[row][col] != p_fs->prev_bstat[row][col]) {
            changed = true;
        }
        if (p_fs->bstat[row][col] == 0)
            p_fs->board_clear = false;
    }

    // undrive the ROW
    HW_TerminateRowRead(p_hw, row);

    return changed;
}


// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *
//     SWITCHES MANAGEMENT FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *

// -----------------------------------------------------------------
static int ReadAllSwitches(TrisSwitchStatus_t *p_ss, const TrisHardwareInfo_t *p_hw)
{
    int cnt;

    p_ss->changed_flag = false;

    for (cnt = 0; cnt < A3INO_NUM_SWITCHES; cnt++) {
        p_ss->prev_stat[cnt] = p_ss->curr_stat[cnt];
        p_ss->curr_stat[cnt] = HW_ReadSwitch(p_hw, cnt);
        if (p_ss->curr_stat[cnt] != p_ss->prev_stat[cnt]) {
            p_ss->schanged[cnt] = true;
            p_ss->changed_flag = true;
            if (p_ss->curr_stat[cnt] == LOW)
                p_ss->sw_event[cnt] = SW_EVENT_PRESSED;
            else
                p_ss->sw_event[cnt] = SW_EVENT_RELEASED;
        }
        else {
            p_ss->schanged[cnt] = false;
            p_ss->sw_event[cnt] = SW_EVENT_NONE;
        }
    }
    return p_ss->changed_flag;
}


// -----------------------------------------------------------------
static void ManageSwitchChange(const TrisSwitchStatus_t *p_ss)
{
    // TO BE COMPLETED
    printSwitchesStatus(p_ss);
}

// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *
//     TRIS ENGINE MANAGEMENT FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *

// -----------------------------------------------------------------
static void SetLedEffectByEngStatus(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw, int eng_status)
{
    switch (eng_status) {
        case TE_STAT_INIT:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_BLINK);
            break;

        case TE_STAT_WAIT_1ST_MOVE:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_ROTATE);
            break;

        case TE_STAT_DO_MOVE:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_NONE);
            break;

        case TE_STAT_WAIT_PAWN:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_NONE);
            break;

        case TE_STAT_WAIT_MOVE:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_ROTATE);
            break;

        case TE_STAT_GAME_WON:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_ALLON);
            break;

        case TE_STAT_GAME_LOST:
            SetLedEffect(p_ls, p_hw, LED_EFFECT_ALLOFF);
            break;

        default:
            // invalid state, do nothing...
            break;
    }
}








// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *
//     LEDS MANAGEMENT FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *

// --------------------------------------------------------
static void SetLedEffect(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw, int led_effect)
{
    int cnt;
    int level = LOW;

    if (led_effect == p_ls->led_effect_mode)
        return;

    // in any case (except ALLON), turns off
    // all the leds to reset led status
    if (led_effect == LED_EFFECT_ALLON)
        level = HIGH;

    for (cnt = 0; cnt < A3INO_NUM_LEDS; cnt++)
        HW_TurnLedByNdx(p_hw, cnt, level);

    // sets new status
    p_ls->led_effect_mode = led_effect;

}

// --------------------------------------------------------
static void ManageLedsEffect(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw)
{
    switch (p_ls->led_effect_mode) {
        case LED_EFFECT_BLINK:
            BlinkLeds(p_ls, p_hw);
            break;
        case LED_EFFECT_ROTATE:
            RotateLeds(p_ls, p_hw);
            break;
        default:
            break;
    }
}

// --------------------------------------------------------
#define BLINK_CNT_VAL       1

static void BlinkLeds(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw)
{
    int cnt;

    if (p_ls->blink_cnt <= 0) {

        if (p_ls->blink_cur_stat != HIGH)
            p_ls->blink_cur_stat = HIGH;
        else
            p_ls->blink_cur_stat = LOW;
        for (cnt = 0; cnt < A3INO_NUM_LEDS; cnt++)
           HW_TurnLedByNdx(p_hw, cnt, p_ls->blink_cur_stat);

        p_ls->blink_cnt = BLINK_CNT_VAL;
    }
    else
        --(p_ls->blink_cnt);
}

// --------------------------------------------------------
#define ROT_CNT_VAL         1

static void RotateLeds(TrisLedStatus_t *p_ls, const TrisHardwareInfo_t *p_hw)
{
  // ----------------------------------------
  if (p_ls->rot_cnt <= 0) {
    HW_TurnLedByNdx(p_hw, p_ls->rot_led_ndx, LOW);
    p_ls->rot_led_ndx = ComputeNextLedInRotation(p_ls->rot_led_ndx);
    
    HW_TurnLedByNdx(p_hw, p_ls->rot_led_ndx, HIGH);
    p_ls->rot_cnt = ROT_CNT_VAL;
  }
  else
    --(p_ls->rot_cnt);
}


// --------------------------------------------------------
static int ComputeNextLedInRotation(int led_ndx)
{
  int next_led_ndx = 0;

  if (led_ndx < (A3INO_NUM_LEDS - 1))
    next_led_ndx = led_ndx + 1;
  else
    next_led_ndx = 0;  
  
  return next_led_ndx;
    
}

// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *
//     TRIS ENGINE FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * *


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//    PRINT TO SERIAL FUNCTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


// -----------------------------------------------------------------
static void PrintBanner() {  
  Serial.begin(9600); // This sets up a serial connection, 9.6 kbts/s.
  Serial.println("TRISBOARD, by F. Piantini, 20140208");
  Serial.end();

  // DA METTERE A POSTO
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, LOW); 
  digitalWrite(1, LOW);
}

// -----------------------------------------------------------------
static void printFieldStatus(const TrisFieldStatus_t *p_fs) {
  int row, col;
  
  Serial.begin(9600); // This sets up a serial connection, 9.6 kbts/s.

  Serial.print("*** TRISBOARD Status changed ***\n");
  for (row = 0; row < A3INO_NUM_ROWS; row++) {
    for (col = 0; col < A3INO_NUM_COLS; col++) {
      Serial.print(p_fs->bstat[row][col]);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
  Serial.print("\n");

  Serial.end();

  // DA METTERE A POSTO
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, LOW); 
  digitalWrite(1, LOW);

}

// -----------------------------------------------------------------
static void printSwitchesStatus(const TrisSwitchStatus_t *p_ss) {
  int cnt;
  
  Serial.begin(9600); // This sets up a serial connection, 9.6 kbts/s.

  Serial.print("*** SWITCHES Status changed ***\n");
  for (cnt = 0; cnt < A3INO_NUM_SWITCHES; cnt++) {
    Serial.print(p_ss->curr_stat[cnt]);
    Serial.print(" ");
  }
  Serial.print("\n");

  Serial.end();

  // DA METTERE A POSTO
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(0, LOW); 
  digitalWrite(1, LOW);

}

