#if !defined TRISPAWN_INCLUDE_
#define TRISPAWN_INCLUDE_

// Pawn position type
#define PAWN_POS_NONE       0
#define PAWN_POS_ANGLE      1
#define PAWN_POS_SIDECENTER 2
#define PAWN_POS_CENTER     3

// ************************************************************************
// ---------------------------------------------
typedef struct {
    int row;
    int col;

} TrisFieldCoords_t;

// ---------------------------------------------
#define A3INO_MAX_NUM_MOVES     10
typedef struct {
    int                 num_moves;
    TrisFieldCoords_t   moves[A3INO_MAX_NUM_MOVES];

} TrisMovesSequence_t;

// ************************************************************************
// ---------------------------------------------
// fields event codes
typedef enum {
    FEVENT_NONE,            //  0
    FEVENT_PAWN_ADDED,      //  1
    FEVENT_PAWN_REMOVED,    //  2

    FEVENT_ILLEGAL = 99     // 99

} TrisPawnEventType_t;

// ---------------------------------------------
#define A3INO_MAX_NUM_EVENTS    10
typedef struct {
    TrisPawnEventType_t ev_code;
    TrisFieldCoords_t   pcoords;

} TrisPawnEvent_t;

// ---------------------------------------------
typedef struct {
    int num_events;
    TrisPawnEvent_t tp_evs[A3INO_MAX_NUM_EVENTS];

} TrisPawnEventSet_t;


// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
int TBPos_IsAngle(const TrisFieldCoords_t *p_pos);
int TBPos_IsSideCenter(const TrisFieldCoords_t *p_pos);
int TBPos_IsCenter(const TrisFieldCoords_t *p_pos);
int TBPos_GetType(const TrisFieldCoords_t *p_pos);
void TBPos_OppositeAngle(const TrisFieldCoords_t *p_pos, TrisFieldCoords_t *p_opp);
void TBPos_OppositeSideCenter(const TrisFieldCoords_t *p_pos, TrisFieldCoords_t *p_opp);


void MoveSequenceInit(TrisMovesSequence_t *p_ms);
int MoveSequenceAddMove(TrisMovesSequence_t *p_ms, const TrisFieldCoords_t *p_pos);

void EventSetReset(TrisPawnEventSet_t *p_pes);
int  AddEventToSet(TrisPawnEventSet_t *p_pes, const TrisPawnEvent_t *p_ev);



#endif // #if !defined TRISPAWN_INCLUDE_
