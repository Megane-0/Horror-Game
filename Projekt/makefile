# set this variable to the directory in which you saved the common files
commondir = ../common/

all : lab4-1 lab4-2 lab4-3 lab4-4 lab4-5 lab4-6

lab4-1 : lab4-1.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab4-1 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab4-1.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

lab4-2 : lab4-2.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab4-2 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab4-2.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

lab4-3 : lab4-3.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab4-3 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab4-3.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

lab4-4 : lab4-4.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab4-4 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab4-4.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

lab4-5 : lab4-5.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o lab4-5 -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES lab4-5.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

projekt : projekt.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	g++ -Wall -o projekt -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES projekt.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c -lX11 -lGL -lm

clean :
	rm lab4-1
	rm lab4-2
	rm lab4-3
	rm lab4-4
	rm lab4-5
	rm lab4-6

