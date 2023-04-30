/****************************
*
* SLIDER.H
*
****************************/

INT main (void);

// #pragma linkage (ClientWinProc, system) //Seems to be for IBM compiler
MRESULT ClientWinProc (HWND, USHORT, MPARAM, MPARAM);


#define WINDOW_ID   1

#define ID_FRAMERC  10
#define ID_SLIDER_SUBMENU 100
#define ID_HSLIDER 101
#define ID_VSLIDER 102
#define ID_STOP    103
#define ID_EXIT    110
