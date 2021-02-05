
PSP_FW_VERSION = 100

//TARGET = DE2_ALPHA

OBJS = ../ca3d/gallerydemo_src/main.o			\
       ../ca3d/gallerydemo_src/collision_detection.o	\
       ../ca3d/gallerydemo_src/gamestate.o		\
       ../ca3d/gallerydemo_src/highscores.o		\
       ../ca3d/gallerydemo_src/shooting.o		\
       ../ca3d/gallerydemo_src/ai.o			\
       ../ca3d/de2mapformat/component.o			\
       ../ca3d/de2mapformat/light.o			\
       ../ca3d/de2mapformat/lightmap.o			\
       ../ca3d/de2mapformat/de2mapformat.o		\
       ../ca3d/caUtil/audio/caAudio.o			\
       ../ca3d/caUtil/camera.o				\
       ../ca3d/caUtil/memory.o				\
       ../ca3d/caUtil/plane.o				\
       ../ca3d/caUtil/calog.o				\
       ../ca3d/caUtil/rendering.o			\
       ../ca3d/caUtil/system.o				\
       ../ca3d/caUtil/vectorMath.o			\
       ../ca3d/caUtil/vertex.o				\
       ../ca3d/caUtil/textures.o			\
       ../ca3d/caUtil/texture_formats.o			\
       ../ca3d/caUtil/font.o				\
       ../ca3d/caUtil/particles.o			\
       ../ca3d/caUtil/moveinfo.o			\
       ../ca3d/caUtil/md3.o

INCDIR =
CFLAGS = -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS = -lstdc++ -lpspgum_vfpu -lpspvfpu -lpspgu -lm -lpspaudiolib -lpspaudio -lpsppower

EXTRA_TARGETS = clean EBOOT.PBP removeobjects install

BUILDDATE = $(shell date "+%Y/%m/%d %k:%M:%S")

//PSP_EBOOT_TITLE = $(TARGET) $(BUILDDATE)

PSP_EBOOT_TITLE = Dungeons: Assault Run 1.0.6

PSP_EBOOT_ICON = "ball1.png"

PSPSDK=$(shell psp-config --pspsdk-path)

include $(PSPSDK)/lib/build.mak

install:
	cp -f EBOOT.PBP /Volumes/PSP1/PSP/GAME/PSPDAR/

removeobjects:
	-rm -f $(OBJS)
