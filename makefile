CXX = g++ -std=c++11
CXXFLAGS = -Wall -ansi -pedantic -g

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl -pthread
endif

all: TCPEchoClient TCPEchoServer TCPRelay TCPEchoServer-Thread 

TCPEchoClient: TCPEchoClient.cpp PracticalSocket.cpp PracticalSocket.h
	$(CXX) $(CXXFLAGS) -o TCPEchoClient TCPEchoClient.cpp PracticalSocket.cpp $(LIBS) -lpthread

TCPEchoServer: TCPEchoServer.cpp PracticalSocket.cpp PracticalSocket.h
	$(CXX) $(CXXFLAGS) -o TCPEchoServer TCPEchoServer.cpp PracticalSocket.cpp $(LIBS) -lpthread

TCPRelay: TCPRelay.cpp PracticalSocket.cpp PracticalSocket.h
	$(CXX) $(CXXFLAGS) -o TCPRelay TCPRelay.cpp PracticalSocket.cpp $(LIBS) -lpthread

TCPEchoServer-Thread: TCPEchoServer-Thread.cpp PracticalSocket.cpp PracticalSocket.h
	$(CXX) -D_GNU_SOURCE -o TCPEchoServer-Thread TCPEchoServer-Thread.cpp PracticalSocket.cpp $(LIBS) -lpthread

clean:
	$(RM) TCPEchoClient TCPEchoServer RCPRelay TCPEchoServer-Thread 