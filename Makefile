EXECF = avrs

SRC_DIR = src
OBJ_DIR = bin

OBJF =	surface.o \
        listener.o \
        soundsource.o \
        dxfreader.o \
        configfilereader.o \
       	tokenizer.o \
       	convolver.o \
        virtualenvironment.o \
        headfilter.o \
        trackersim.o \
        rttools.o \
        player.o \
        input.o \
        configuration.o \
        fdn.o \
        system.o \
        wiimote.o \
        trackerwiimote.o \
        posit.o \
        timer.o \
        hrtfconvolver.o

OUT_OBJF = $(addprefix $(OBJ_DIR)/,$(OBJF))        
        
LIBS = -lfftw3f -lc -lm -lstk -lasound -lpthread -ldxflib -larmadillo -lbluetooth -lcwiid -lANN \
       -lboost_system -lboost_filesystem

INCLUDES = -I/usr/realtime/include \
           -I/usr/src/linux/include 
           
CXXFLAGS = -O2  -pipe #-std=c++11 #-Wall
CFLAGS = -O2 -Wall -pipe

LDFLAGS = -L/usr/realtime/lib

MACROS = -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__ -D_REENTRANT  #-DWIIMOTE_TRACKER

all: $(EXECF)

$(EXECF): $(OUT_OBJF) $(OBJ_DIR)/svd.o $(OBJ_DIR)/main.o
	$(CXX) $(LDFLAGS) $(OUT_OBJF) $(OBJ_DIR)/svd.o $(OBJ_DIR)/main.o -o $(OBJ_DIR)/$(EXECF) $(LIBS)
	@echo "$(EXECF) build..."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp $(SRC_DIR)/common.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) $(MACROS) -c $< -o $@
	
$(OBJ_DIR)/svd.o: $(SRC_DIR)/svd.c $(SRC_DIR)/svd.h
	$(CC) $(CFLAGS) -c $< -o $@
	
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/system.cpp $(SRC_DIR)/system.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(MACROS) -c $< -o $@

clean:
	@echo "Cleaning..."
	@find $(OBJ_DIR) -name '*.o' -exec rm "{}" \;
	@if [ -f $(OBJ_DIR)/$(EXECF) ]; then rm -f $(OBJ_DIR)/$(EXECF); fi
