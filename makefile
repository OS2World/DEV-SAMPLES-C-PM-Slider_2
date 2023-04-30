# nmake makefile
#
# Tools used:
#  Compile::Watcom Resource Compiler
#  Compile::GNU C
#  Make: nmake or GNU make
all : slider.exe

slider.exe : slider.obj slider.res slider.def
	gcc -Zomf slider.obj slider.res slider.def -o slider.exe
	wrc slider.res

slider.obj : slider.c slider.h
	gcc -Wall -Zomf -c -O2 slider.c -o slider.obj

slider.res : slider.rc 
	wrc -r slider.rc

clean :
	rm -rf *exe *RES *obj