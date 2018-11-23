

CLIENT_OBJS = clientDatabase.o client_main.o commandHandler.o message.pb.o node.o peerCommunicator.o peerHandler.o peerListener.o peerMessageHandler.o 
LDFLAGS +=  -lpthread -lcrypto -lssl -lprotobuf
CFLAGS += -g -I$(PROJECT_ROOT)/include

COMMON_OBJS := $(wildcard $(PROJECT_ROOT)/common/*.o)

all: pastryClient

pastryClient: $(CLIENT_OBJS)
	$(CXX) -o $@ $^ $(COMMON_OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm pastryClient $(CLIENT_OBJS)
