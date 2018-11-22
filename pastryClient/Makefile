

CLIENT_OBJS = client_main.o clientDatabase.o mtorrent.o chunkDownloader.o fileHandler.o commandHandler.o peerHandler.o peerListener.o peerMessageHandler.o download.o downloadManager.o chunkSaver.o
LDFLAGS +=  -lpthread -lcrypto -lssl
CFLAGS += -g -I$(PROJECT_ROOT)/include

COMMON_OBJS := $(wildcard $(PROJECT_ROOT)/common/*.o)

all: tclient

tclient: $(CLIENT_OBJS)
	$(CXX) -o $@ $^ $(COMMON_OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm tclient $(CLIENT_OBJS)
