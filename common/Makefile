COMMON_OBJS := utils.o NetworkInterfacer.o logHandler.o errorMsg.o
CFLAGS += -g -I$(PROJECT_ROOT)/include

all: $(COMMON_OBJS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm $(COMMON_OBJS)
