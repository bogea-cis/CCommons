OUTPUT_FILE = libCEFTMFPTR.so
CPP = g++

OUTPUT_DIR = Release

INCLUDE_DIRS =  -I ../libusb-1.0.9/source/libusb \
		-I ./include \
		-I ./jdk_includes \
		-I ./jdk_includes/linux \
		-I ./common
				
LIBRARY_PATH =  -L ../libusb-1.0.9/deploy/Linux

LIBS = -lpthread -lusb-1.0

DEBUG_FLAGS = -Wall -O2 -g3 -fno-inline -fmessage-length=0 -fno-strict-aliasing -fPIC -pthread $(INCLUDE_DIRS) $(LIBRARY_PATH) -DLIBUSB10 -Dlinux
RELEASE_FLAGS = -Wall -O2 -fno-inline -fmessage-length=0 -fno-strict-aliasing -fPIC -pthread $(INCLUDE_DIRS) $(LIBRARY_PATH) -DLIBUSB10 -Dlinux

SOURCES = $(shell find ./ -iname "*.cpp")
OBJS = $(SOURCES:.cpp=.o)

C_SOURCES = $(shell find ./ -iname "*.c")
C_OBJS = $(C_SOURCES:.c=.o)

all: 
	make $(OUTPUT_FILE)

$(OUTPUT_FILE) : createOutputDir $(C_OBJS) $(OBJS)
	@echo $(C_OBJS)
	@echo $(OBJS)
	$(CPP) $(RELEASE_FLAGS) $(C_OBJS) $(OBJS) $(LIBS) -shared -o $(OUTPUT_DIR)/$(OUTPUT_FILE)
	rm -rf ./*/*.o
	
createOutputDir:
	@mkdir -p $(OUTPUT_DIR)

%.o : %.cpp
	@echo $<
	$(CPP) $(RELEASE_FLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(RELEASE_FLAGS) -c $< -o $@	
	
clean:
	rm -rf ./*/*.o

