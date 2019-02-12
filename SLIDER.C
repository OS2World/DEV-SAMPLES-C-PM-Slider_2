/***************************************************************
 * Author:  Myron McCloud
 *
 * Date:   1/17/92
 *
 * File Name:  Slider.c
 *
 * Description: Example of a Slider.
 *
 * Purpose:
 *
 * Parameter:  None.
 *
 * Returns:
 *
 * Comments:
 *
 ***************************************************************/
#define INCL_WIN
#define INCL_PM
#define INCL_WINSTDSLIDER      // slider control class
#define INCL_WINWINDOWMGR      // editfield control class
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include "slider.h"

main (void)
{
   HAB      hab;
   HMQ      hmq;
   QMSG     qmsg;
   HWND     hwndParent;
   HWND     hwndClient;
   HWND     hwndSlider;

   static CHAR szClientClass [] = "Client Window";

   ULONG flCreateFlags = FCF_TITLEBAR
                         | FCF_SYSMENU
                         | FCF_SIZEBORDER
                         | FCF_MINMAX
                         | FCF_SHELLPOSITION
                         | FCF_TASKLIST
                         | FCF_MENU
                         ;

   hab = WinInitialize (0);

   hmq = WinCreateMsgQueue (hab, 0);

   WinRegisterClass ((HAB)    hab
                    ,(PSZ)    szClientClass
                    ,(PFNWP)  ClientWinProc
                    ,(ULONG)  0L
                    ,(USHORT) 0
                    );

   hwndParent = WinCreateStdWindow (HWND_DESKTOP
                                   ,WS_VISIBLE
                                   ,&flCreateFlags
                                   ,szClientClass
                                   ,""
                                   ,0L
                                   ,0
                                   ,ID_FRAMERC
                                   ,&hwndClient
                                   );

   while (WinGetMsg (hab, &qmsg, NULL, 0, 0) )
   {
      WinDispatchMsg (hab, &qmsg);
   } /* endwhile */

   WinDestroyWindow (hwndSlider);
   WinDestroyWindow (hwndParent);
   WinDestroyMsgQueue (hmq);
   WinTerminate (hab);
   return 0;
}

/***************************************************************
 * Function Name:  ClientWinProc
 *
 * Description: Window procedure for the Client
 *
 * Purpose:
 *
 * Parameter      Description
 * --------------------------------------------------------------
 * hwnd           HWND
 * msg            USHORT
 * mp1            MPARAM1
 * mp2            MPARAM2
 *
 * Returns:
 *
 * Comments:
 *
 ***************************************************************/

MRESULT ClientWinProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
{

   #define TICK  10
   #define SPACE 10

   BOOL       rc;
   USHORT     usIndex;
   SHORT      usArrows;                // Fudge Factor
   ENTRYFDATA entryfdata;
   HPS        hps;
   RECTL      rcl;
   CHAR       buff[4];
   CHAR       *cData;
   static     SLDCDATA   sldcdata;     // slider control data structure
   static     WNDPARAMS  wndparams;    // slider window parameters structure
   static     PARAM      param;        // slider presentation data structure
   static     HWND       hwndSlider;   // handle to slider window
   static     HWND       hwndDisplay;  // handle to entryfield window
   static     HWND       hwndMenu;
   static     SHORT      ClientCx;
   static     SHORT      Clientcy;
   static     SHORT      SliderWindowCx;
   static     SHORT      NewShaftCx;
   static     SHORT      PixelsBetweenTicks;
   static     ULONG      ulArmPosition;
   static     ULONG      color = CLR_RED;

   ULONG SliderCreateFlags = SLS_HORIZONTAL
                           | SLS_BUTTONSLEFT
                           | SLS_HOMELEFT
                           | SLS_PRIMARYSCALE1
                           | SLS_RIBBONSTRIP
                           | WS_VISIBLE
                           ;
   ULONG EntryFieldCreateFlags = ES_LEFT
                               | WS_VISIBLE
                               | ES_MARGIN
                               ;

   switch (msg)
   {
      case WM_COMMAND:
         switch (SHORT1FROMMP (mp1))
         {
            case ID_HSLIDER:
               /**
                *
                * Create window with horizonal slider
                *
                **/
               hwndMenu=WinWindowFromID (WinQueryWindow (hwnd,QW_PARENT)
                                        ,FID_MENU);

               WinEnableMenuItem (hwndMenu, ID_HSLIDER, FALSE);
               WinEnableMenuItem (hwndMenu, ID_STOP, TRUE);

               sldcdata.cbSize = sizeof (sldcdata); // Size of control block
               sldcdata.usScale1Increments = TICK ; // # of divisions on scale
               sldcdata.usScale1Spacing    = SPACE; // pels between increments
               sldcdata.usScale2Increments = TICK ; // # of divisions on scale
               sldcdata.usScale2Spacing    = SPACE; // pels between increments
               hwndSlider = WinCreateWindow (hwnd               // parent window
                                            ,WC_SLIDER          // Class
                                            ,""                 // window text
                                            ,SliderCreateFlags  // create style
                                            ,0                  // x = position
                                            ,35                 // y = position
                                            ,120                // cx= width
                                            ,35                 // cy= height
                                            ,hwnd               // window owner
                                            ,HWND_TOP           // where to ins
                                            ,WINDOW_ID          // window id
                                            ,(PVOID) &sldcdata  // ctl data
                                            ,(PVOID) NULL       // pres param
                                            );

               // Set background color of slider window
               WinSetPresParam (hwndSlider
                               ,PP_BACKGROUNDCOLORINDEX
                               ,sizeof(ULONG),&color
                               );
               /**
                *
                * place shaft window flush against left side of slider window
                * (x=0)place shaft window bottom flush against bottom of slider
                * window (y=0)
                *
                **/
               WinSendMsg (hwndSlider
                          ,SLM_SETSLIDERINFO
                          ,MPFROMSHORT  (SMA_SHAFTPOSITION)
                          ,MPFROM2SHORT (0,0)
                          );

               /**
                *
                * Create entryfield window to display slider arm position
                *
                **/
               entryfdata.cb           = sizeof (entryfdata);
               entryfdata.cchEditLimit = 256                ;
               entryfdata.ichMinSel    = 0                  ;
               entryfdata.ichMaxSel    = 256                ;
               hwndDisplay = WinCreateWindow (hwnd
                                             ,WC_ENTRYFIELD
                                             ,"Arm Position"
                                             ,EntryFieldCreateFlags
                                             ,100
                                             ,160
                                             ,85
                                             ,20
                                             ,hwnd
                                             ,HWND_TOP
                                             ,WINDOW_ID + 1
                                             ,(PVOID) &entryfdata
                                             ,NULL
                                             );
               /**
                *
                * This message sets the size of the a tick mark for the
                * primary scale. NOTE: all tick marks are initially set to
                * a size of 0 (INVISIBLE).
                *
                **/
               WinSendMsg (hwndSlider
                          ,SLM_SETTICKSIZE
                          ,MPFROM2SHORT (SMA_SETALLTICKS, 5)
                          ,NULL
                          );
               /**
                *
                * For each tick mark set text above the tick mark for the
                * primary scale. Tick mark does not have to be visible to
                * have text. Text is centered on the tick mark.
                *
                **/
               for (usIndex = 0; usIndex < TICK; usIndex ++)
               {
                  _itoa (usIndex, buff, 10);

                  WinSendMsg (hwndSlider
                             ,SLM_SETSCALETEXT
                             ,MPFROMSHORT (usIndex)
                             ,MPFROMP (buff)
                             );
               }

               /**
                *
                * place shaft window flush against left side of slider window
                * (x=0)place shaft window bottom flush against bottom of slider
                * window (y=0)
                *
                **/
                WinSendMsg (hwndSlider
                           ,SLM_SETSLIDERINFO
                           ,MPFROMSHORT  (SMA_SHAFTPOSITION)
                           ,MPFROM2SHORT (0,0)
                           );

             break;


           case ID_VSLIDER:
               /**
                *
                * Create window with vertical slider.
                * This is left as an exercise for the student.
                *
                **/
               WinMessageBox (HWND_DESKTOP
                             ,hwnd
                             ,"Vertical slider not implemented."
                             ,"Information"
                             ,WINDOW_ID + 2
                             ,MB_CANCEL
                             );
               break;

           case ID_STOP:
               /**
                *
                * Close Slider window.
                *
                **/
               WinEnableMenuItem (hwndMenu, ID_STOP, FALSE);
               WinEnableMenuItem (hwndMenu, ID_HSLIDER, TRUE);
               WinDestroyWindow (hwndSlider);
               WinDestroyWindow (hwndDisplay);
               break;

           case ID_EXIT:                 // Exit selected
               WinPostMsg (hwnd, WM_QUIT, NULL, NULL);
               break;

         } /* endswitch on WM_COMMAND messages */
         break;


      case WM_PAINT:
         hps = WinBeginPaint (hwnd, 0, 0);
         GpiErase (hps);
         WinQueryWindowRect (hwnd, &rcl);
         WinFillRect ((HPS) hps, (PRECTL) &rcl, (LONG) CLR_BLUE);
         WinEndPaint (hps);
         break;


      case WM_SIZE:
         /**
          *
          * Save current slider arm position
          *
          **/
          ulArmPosition =(LONG) WinSendMsg (hwndSlider
                                           ,SLM_QUERYSLIDERINFO
                                           ,MPFROM2SHORT (SMA_SLIDERARMPOSITION
                                           ,SMA_INCREMENTVALUE)
                                           ,NULL
                                           );
         /**
          *
          * Calulates the new (Cx) size of SliderWindow and ShaftWindow
          * in addition determine the distance between the ShaftWindow
          * and the Arrow window in pixels.
          *
          **/
         ClientCx           = SHORT1FROMMP (mp2)        ;
         Clientcy           = SHORT2FROMMP (mp2)        ;
         SliderWindowCx     = ClientCx                  ;
         usArrows           = 20                        ;
         NewShaftCx         = SliderWindowCx - usArrows ;
         PixelsBetweenTicks = NewShaftCx / TICK         ;

         sldcdata.usScale1Spacing = PixelsBetweenTicks; // space between ticks
         sldcdata.usScale2Spacing = PixelsBetweenTicks; // space between ticks

         wndparams.fsStatus       = WPM_CTLDATA       ; // param selection
         wndparams.cchText        = 0                 ; // len of window text
         wndparams.pszText        = NULL              ; // window text
         wndparams.cbPresParams   = 0                 ; // len of pres param
         wndparams.pPresParams    = NULL              ; // specific pres param
         wndparams.cbCtlData      = sizeof (sldcdata) ; // len of specific data
         wndparams.pCtlData       = &sldcdata         ; // window specific data

         // This call allow the general positioning of a window
         WinSetWindowPos (hwndSlider
                         ,HWND_TOP
                         ,0
                         ,35
                         ,SliderWindowCx
                         ,35
                         ,SWP_MOVE | SWP_SIZE | SWP_SHOW
                         );

         // This message is sent to set or change the sliders window parameters
         WinSendMsg (hwndSlider
                    ,WM_SETWINDOWPARAMS
                    ,&wndparams
                    ,NULL
                    );
         /**
          *
          * place shaft window flush against left side slider window (x=0)
          * place shaft window bottom flush against bottom of slider window
          * (y=0)
          *
          **/
          WinSendMsg (hwndSlider
                     ,SLM_SETSLIDERINFO
                     ,MPFROMSHORT  (SMA_SHAFTPOSITION)
                     ,MPFROM2SHORT (0,0)
                     );

         /**
          *
          * Restore slider arm position
          *
          **/
          rc = (BOOL) WinSendMsg (hwndSlider
                                 ,SLM_SETSLIDERINFO
                                 ,MPFROM2SHORT (SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE)
                                 ,MPFROMSHORT ((USHORT) ulArmPosition)
                                 );

         // This call allow the general positioning of editfield window
         WinSetWindowPos (hwndDisplay
                         ,HWND_TOP
                         ,0
                         ,Clientcy - 35
                         ,SliderWindowCx
                         ,35
                         ,SWP_MOVE | SWP_SIZE | SWP_SHOW
                         );
         break;


     case WM_CONTROL:
         /**
          * Process the WM_CONTROL messages for the slider
          **/
          switch (SHORT2FROMMP (mp1))
          {
            case SLN_CHANGE:
            {
               LONG ulValue;
               ulValue = (LONG) WinSendMsg ((HWND) WinWindowFromID
                                           (hwnd, WINDOW_ID)
                                           ,SLM_QUERYSLIDERINFO
                                           ,MPFROM2SHORT (SMA_SLIDERARMPOSITION
                                           ,SMA_INCREMENTVALUE)
                                           ,NULL
                                           );

               /**
                * Convert slider arm position to ascii and place
                * ascii text into entryfield window
                **/
               cData = _ltoa (ulValue, buff, 10);
               WinSetWindowText ((HWND) hwndDisplay, (PSZ) cData);
               break;
            }

          } /* endswitch on WM_CONTROL notifcation messages */
          break;

      default:
         return (WinDefWindowProc (hwnd, msg, mp1, mp2));
         break;

   } /* endswitch on msg */

   return (MRESULT) TRUE;

} /* end of ClientWinProc */
