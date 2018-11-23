COMMON_OBJS := seeder.o message.o utils.o encoder.o decoder.o NetworkInterfacer.o logHandler.o errorMsg.o TrackerServiceServer.o md5.o
CFLAGS += -g -I$(PROJECT_ROOT)/include

all: $(COMMON_OBJS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm $(COMMON_OBJS)
