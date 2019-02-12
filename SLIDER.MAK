# IBM Developer's Workframe/2 Make File Creation run at 13:40:30 on 12/31/91

# Make File Creation run in directory:
#   D:\WORK;

.SUFFIXES:

.SUFFIXES: .c .rc

ALL: SLIDER.EXE \
     SLIDER.RES

slider.exe:  \
  SLIDER.OBJ \
  SLIDER.RES \
  slider.def \
  SLIDER.MAK
   @REM @<<SLIDER.@0
     /CO /M /ST:8000 /PM:PM +
     SLIDER.OBJ
     slider.exe
     slider.map
     
     slider.def;
<<
   LINK386.EXE @SLIDER.@0
   RC SLIDER.RES slider.exe

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Ss /Q /Foslider.obj /Ti /W2 /Gm /Gs- /C .\$*.c

!include SLIDER.DEP
