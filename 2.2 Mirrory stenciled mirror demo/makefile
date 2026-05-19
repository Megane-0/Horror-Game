# set this variable to the directory where you saved the common files
commondir = ../common/

all : mirrory.exe

mirrory.exe: mirrory.cpp $(commondir)GL_utilities.c $(commondir)LoadTGA.c $(commondir)Windows/MicroGlut.c
	g++ -Wall -o mirrory.exe \
	-I$(commondir) \
	-I$(commondir)Windows \
	-DGL_GLEXT_PROTOTYPES \
	mirrory.cpp \
	$(commondir)GL_utilities.c \
	$(commondir)LoadTGA.c \
	$(commondir)Windows/MicroGlut.c \
	-lopengl32 -lgdi32 -lglew32

clean :
	del mirrory.exe