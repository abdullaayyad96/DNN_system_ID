#include "__cf_PD_CONTROL.h"
#include <math.h>
#include "PD_CONTROL_acc.h"
#include "PD_CONTROL_acc_private.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simtarget/slSimTgtMdlrefSfcnBridge.h"
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T PD_CONTROL_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T * bufSzPtr ,
int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T tMinusDelay ,
real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr , boolean_T
isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr ) { int_T
testIdx ; int_T tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T * tBuf =
* tBufPtr ; real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if (
istransportdelay ) { numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail <
( bufSz - 1 ) ) ? ( tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] )
&& ! isfixedbuf ) { int_T j ; real_T * tempT ; real_T * tempU ; real_T *
tempX = ( NULL ) ; real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024
; if ( newBufSz > * maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU
= ( real_T * ) utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if (
tempU == ( NULL ) ) { return ( false ) ; } tempT = tempU + newBufSz ; if (
istransportdelay ) tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j
++ ) { tempT [ j - tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if
( istransportdelay ) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j <
tail ; j ++ ) { tempT [ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz -
tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j + bufSz - tail ] =
xBuf [ j ] ; } if ( * lastPtr > tail ) { * lastPtr -= tail ; } else { *
lastPtr += ( bufSz - tail ) ; } * tailPtr = 0 ; * headPtr = bufSz ; utFree (
uBuf ) ; * bufSzPtr = newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if (
istransportdelay ) * xBufPtr = tempX ; } else { * tailPtr = testIdx ; }
return ( true ) ; } real_T PD_CONTROL_acc_rt_TDelayInterpolate ( real_T
tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T bufSz ,
int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput ,
boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) { int_T i ;
real_T yout , t1 , t2 , u1 , u2 ; if ( ( newIdx == 0 ) && ( oldestIdx == 0 )
&& ( tMinusDelay > tStart ) ) return initOutput ; if ( tMinusDelay <= tStart
) return initOutput ; if ( ( tMinusDelay <= tBuf [ oldestIdx ] ) ) { if (
discrete ) { return ( uBuf [ oldestIdx ] ) ; } else { int_T tempIdx =
oldestIdx + 1 ; if ( oldestIdx == bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [
oldestIdx ] ; t2 = tBuf [ tempIdx ] ; u1 = uBuf [ oldestIdx ] ; u2 = uBuf [
tempIdx ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else {
yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ;
real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } return yout ; } } if (
minorStepAndTAtLastMajorOutput ) { if ( newIdx != 0 ) { if ( * lastIdx ==
newIdx ) { ( * lastIdx ) -- ; } newIdx -- ; } else { if ( * lastIdx == newIdx
) { * lastIdx = bufSz - 1 ; } newIdx = bufSz - 1 ; } } i = * lastIdx ; if (
tBuf [ i ] < tMinusDelay ) { while ( tBuf [ i ] < tMinusDelay ) { if ( i ==
newIdx ) break ; i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } } else { while
( tBuf [ i ] >= tMinusDelay ) { i = ( i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i =
( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } * lastIdx = i ; if ( discrete ) {
double tempEps = ( DBL_EPSILON ) * 128.0 ; double localEps = tempEps *
muDoubleScalarAbs ( tBuf [ i ] ) ; if ( tempEps > localEps ) { localEps =
tempEps ; } localEps = localEps / 2.0 ; if ( tMinusDelay >= ( tBuf [ i ] -
localEps ) ) { yout = uBuf [ i ] ; } else { if ( i == 0 ) { yout = uBuf [
bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ; } } } else { if ( i == 0 ) {
t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1 ] ; } else { t1 = tBuf [ i -
1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ; u2 = uBuf [ i ] ; if ( t2 ==
t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else {
real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout
= f1 * u1 + f2 * u2 ; } } return ( yout ) ; } void rt_ssGetBlockPath (
SimStruct * S , int_T sysIdx , int_T blkIdx , char_T * * path ) {
_ssGetBlockPath ( S , sysIdx , blkIdx , path ) ; } void rt_ssSet_slErrMsg (
SimStruct * S , void * diag ) { if ( ! _ssIsErrorStatusAslErrMsg ( S ) ) {
_ssSet_slErrMsg ( S , diag ) ; } else { _ssDiscardDiagnostic ( S , diag ) ; }
} void rt_ssReportDiagnosticAsWarning ( SimStruct * S , void * diag ) {
_ssReportDiagnosticAsWarning ( S , diag ) ; } static void mdlOutputs (
SimStruct * S , int_T tid ) { real_T * lastU ; real_T lastTime ; int32_T
isHit ; B_PD_CONTROL_T * _rtB ; P_PD_CONTROL_T * _rtP ; X_PD_CONTROL_T * _rtX
; DW_PD_CONTROL_T * _rtDW ; _rtDW = ( ( DW_PD_CONTROL_T * ) ssGetRootDWork (
S ) ) ; _rtX = ( ( X_PD_CONTROL_T * ) ssGetContStates ( S ) ) ; _rtP = ( (
P_PD_CONTROL_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_PD_CONTROL_T * )
_ssGetModelBlockIO ( S ) ) ; _rtB -> B_1_0_0 = 0.0 ; _rtB -> B_1_0_0 += _rtP
-> P_2 * _rtX -> Internal_CSTATE [ 2 ] ; ssCallAccelRunBlock ( S , 1 , 1 ,
SS_CALL_MDL_OUTPUTS ) ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_4 ; _rtB -> B_1_2_0 =
PD_CONTROL_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer , *
uBuffer , _rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail , _rtDW ->
TransportDelay_IWORK . Head , _rtP -> P_5 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } _rtB -> B_1_3_0 = _rtB -> B_1_0_0_m - _rtB -> B_1_2_0 ;
ssCallAccelRunBlock ( S , 0 , 0 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_0_0_1 != 0.0
) { ssSetStopRequested ( S , 1 ) ; } { if ( ( _rtDW ->
TAQSigLogging_InsertedFor_LTISystem_at_outport_0_PWORK . AQHandles || _rtDW
-> TAQSigLogging_InsertedFor_LTISystem_at_outport_0_PWORK . SlioLTF ) &&
ssGetLogOutput ( S ) ) { sdiSlioSdiWriteSignal ( _rtDW ->
TAQSigLogging_InsertedFor_LTISystem_at_outport_0_PWORK . AQHandles , _rtDW ->
TAQSigLogging_InsertedFor_LTISystem_at_outport_0_PWORK . SlioLTF , 0 ,
ssGetTaskTime ( S , 1 ) , ( char * ) & _rtB -> B_1_0_0 + 0 ) ; } } { if ( (
_rtDW -> TAQSigLogging_InsertedFor_MATLABFunction_at_outport_0_PWORK .
AQHandles || _rtDW ->
TAQSigLogging_InsertedFor_MATLABFunction_at_outport_0_PWORK . SlioLTF ) &&
ssGetLogOutput ( S ) ) { sdiSlioSdiWriteSignal ( _rtDW ->
TAQSigLogging_InsertedFor_MATLABFunction_at_outport_0_PWORK . AQHandles ,
_rtDW -> TAQSigLogging_InsertedFor_MATLABFunction_at_outport_0_PWORK .
SlioLTF , 0 , ssGetTaskTime ( S , 1 ) , ( char * ) & _rtB -> B_0_0_1 + 0 ) ;
} } } if ( ( _rtDW -> TimeStampA >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB
>= ssGetT ( S ) ) ) { _rtB -> B_1_8_0 = 0.0 ; } else { lastTime = _rtDW ->
TimeStampA ; lastU = & _rtDW -> LastUAtTimeA ; if ( _rtDW -> TimeStampA <
_rtDW -> TimeStampB ) { if ( _rtDW -> TimeStampB < ssGetT ( S ) ) { lastTime
= _rtDW -> TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ; } } else { if (
_rtDW -> TimeStampA >= ssGetT ( S ) ) { lastTime = _rtDW -> TimeStampB ;
lastU = & _rtDW -> LastUAtTimeB ; } } _rtB -> B_1_8_0 = ( _rtB -> B_1_2_0 - *
lastU ) / ( ssGetT ( S ) - lastTime ) ; } _rtB -> B_1_9_0 = _rtP -> P_6 *
_rtB -> B_1_8_0 ; _rtB -> B_1_10_0 = _rtP -> P_7 * _rtB -> B_1_3_0 ; _rtB ->
B_1_11_0 = _rtB -> B_1_10_0 + _rtB -> B_1_9_0 ; UNUSED_PARAMETER ( tid ) ; }
static void mdlOutputsTID2 ( SimStruct * S , int_T tid ) { B_PD_CONTROL_T *
_rtB ; P_PD_CONTROL_T * _rtP ; _rtP = ( ( P_PD_CONTROL_T * ) ssGetModelRtp (
S ) ) ; _rtB = ( ( B_PD_CONTROL_T * ) _ssGetModelBlockIO ( S ) ) ; _rtB ->
B_1_0_0_m = _rtP -> P_8 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { real_T * lastU ;
B_PD_CONTROL_T * _rtB ; P_PD_CONTROL_T * _rtP ; DW_PD_CONTROL_T * _rtDW ;
_rtDW = ( ( DW_PD_CONTROL_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_PD_CONTROL_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( ( B_PD_CONTROL_T * )
_ssGetModelBlockIO ( S ) ) ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( ( _rtDW ->
TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! PD_CONTROL_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK . CircularBufSize , & _rtDW -> TransportDelay_IWORK .
Tail , & _rtDW -> TransportDelay_IWORK . Head , & _rtDW ->
TransportDelay_IWORK . Last , simTime - _rtP -> P_4 , tBuffer , uBuffer , (
NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK . Head ] = _rtB ->
B_1_0_0 ; } if ( _rtDW -> TimeStampA == ( rtInf ) ) { _rtDW -> TimeStampA =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA ; } else if ( _rtDW ->
TimeStampB == ( rtInf ) ) { _rtDW -> TimeStampB = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB ; } else if ( _rtDW -> TimeStampA < _rtDW -> TimeStampB
) { _rtDW -> TimeStampA = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA ; }
else { _rtDW -> TimeStampB = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB ;
} * lastU = _rtB -> B_1_2_0 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID2 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { B_PD_CONTROL_T * _rtB ;
P_PD_CONTROL_T * _rtP ; X_PD_CONTROL_T * _rtX ; XDot_PD_CONTROL_T * _rtXdot ;
_rtXdot = ( ( XDot_PD_CONTROL_T * ) ssGetdX ( S ) ) ; _rtX = ( (
X_PD_CONTROL_T * ) ssGetContStates ( S ) ) ; _rtP = ( ( P_PD_CONTROL_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_PD_CONTROL_T * ) _ssGetModelBlockIO ( S
) ) ; _rtXdot -> Internal_CSTATE [ 0 ] = 0.0 ; _rtXdot -> Internal_CSTATE [ 1
] = 0.0 ; _rtXdot -> Internal_CSTATE [ 2 ] = 0.0 ; _rtXdot -> Internal_CSTATE
[ 0 ] += _rtP -> P_0 [ 0 ] * _rtX -> Internal_CSTATE [ 0 ] ; _rtXdot ->
Internal_CSTATE [ 0 ] += _rtP -> P_0 [ 1 ] * _rtX -> Internal_CSTATE [ 1 ] ;
_rtXdot -> Internal_CSTATE [ 1 ] += _rtP -> P_0 [ 2 ] * _rtX ->
Internal_CSTATE [ 0 ] ; _rtXdot -> Internal_CSTATE [ 2 ] += _rtP -> P_0 [ 3 ]
* _rtX -> Internal_CSTATE [ 1 ] ; _rtXdot -> Internal_CSTATE [ 0 ] += _rtP ->
P_1 * _rtB -> B_1_11_0 ; } static void mdlInitializeSizes ( SimStruct * S ) {
ssSetChecksumVal ( S , 0 , 2162185147U ) ; ssSetChecksumVal ( S , 1 ,
803964939U ) ; ssSetChecksumVal ( S , 2 , 1465672028U ) ; ssSetChecksumVal (
S , 3 , 1895949167U ) ; { mxArray * slVerStructMat = NULL ; mxArray *
slStrMat = mxCreateString ( "simulink" ) ; char slVerChar [ 10 ] ; int status
= mexCallMATLAB ( 1 , & slVerStructMat , 1 , & slStrMat , "ver" ) ; if (
status == 0 ) { mxArray * slVerMat = mxGetField ( slVerStructMat , 0 ,
"Version" ) ; if ( slVerMat == NULL ) { status = 1 ; } else { status =
mxGetString ( slVerMat , slVerChar , 10 ) ; } } mxDestroyArray ( slStrMat ) ;
mxDestroyArray ( slVerStructMat ) ; if ( ( status == 1 ) || ( strcmp (
slVerChar , "10.0" ) != 0 ) ) { return ; } } ssSetOptions ( S ,
SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork ( S ) != sizeof (
DW_PD_CONTROL_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_PD_CONTROL_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_PD_CONTROL_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & PD_CONTROL_rtDefaultP ) ; rt_InitInfAndNaN ( sizeof ( real_T ) ) ; }
static void mdlInitializeSampleTimes ( SimStruct * S ) { { SimStruct * childS
; SysOutputFcn * callSysFcns ; childS = ssGetSFunction ( S , 0 ) ;
callSysFcns = ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ]
= ( SysOutputFcn ) ( NULL ) ; } slAccRegPrmChangeFcn ( S , mdlOutputsTID2 ) ;
} static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
