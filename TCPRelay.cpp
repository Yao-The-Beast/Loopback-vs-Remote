#include "PracticalSocket.h"  // For Socket, ServerSocket, and SocketException
#include <iostream>           // For cerr and cout
#include <vector>
#include <cstdlib>            // For atoi()
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <pthread.h>
#include <mutex>

using namespace std;

const int NUM_THREADS = 1024;
pthread_t threads[NUM_THREADS];
int id = 0;
bool flag = true;
const unsigned int RCVBUFSIZE = 128;    // Size of receive buffer

void HandleTCPClient(TCPSocket *sock); // TCP client handling function

unsigned short relayPort = 1111; 
unsigned short echoServPort = 1112;
string echoServAddress = "127.0.0.1";  

TCPServerSocket relaySock(relayPort);   

int main(int argc, char *argv[]) {
  try {
    while (true){ 
        HandleTCPClient(relaySock.accept());       // Wait for a client to connect
    }
  }catch (SocketException &e){
    cerr << e.what() << endl;
    exit(1);
  }
  return 0;
}

void* worker(void* socket){
    TCPSocket* sock = (TCPSocket *)socket;
    TCPSocket sendSock(echoServAddress, echoServPort);
    while (true){
      char echoBuffer[RCVBUFSIZE];
      int recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE);
      if (recvMsgSize == RCVBUFSIZE){
        //relay to server
        sendSock.send(echoBuffer, RCVBUFSIZE); 
      }else if (recvMsgSize > 0){
        std::cout << "WRONG MESSAGE SIZE" << std::endl;
      }
    }
}

// TCP client handling function
void HandleTCPClient(TCPSocket *sock) {
  cout << "Handling client ";
  try {
    cout << sock->getForeignAddress() << ":";
  } catch (SocketException e) {
    cerr << "Unable to get foreign address" << endl;
  }
  try {
    cout << sock->getForeignPort();
  } catch (SocketException e) {
    cerr << "Unable to get foreign port" << endl;
  }
  std::cout << std::endl;
  int thisId = id;
  id++;
  pthread_create(&threads[thisId],NULL,worker,(void*)sock);
  pthread_join(threads[thisId],NULL);
}
