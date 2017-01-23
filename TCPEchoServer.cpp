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
//#include <mutex>

using namespace std;

const unsigned int MESSAGES = 100000;
const int NUM_THREADS = 1;
pthread_t threads[NUM_THREADS];
int connections = 0;
int id = 0;
bool flag = true;
const unsigned int RCVBUFSIZE = 128;    // Size of receive buffer

unsigned short echoServPort = 1112;  

TCPServerSocket servSock(echoServPort);     // Server Socket object
void HandleTCPClient(TCPSocket *sock); // TCP client handling function
std::mutex mtx;

int main(int argc, char *argv[]) {
  try {
    while (true){ 
      //if reach the exact amount
      if (connections == NUM_THREADS){
        for (int i = 0; i < NUM_THREADS; i++)
          pthread_join(threads[i],NULL);
        servSock.closeSocket();
        break;
      }else{
        HandleTCPClient(servSock.accept());       // Wait for a client to connect
      }
    }
  }catch (SocketException &e){
    cerr << e.what() << endl;
    exit(1);
  }
  return 0;
}

void* worker(void* socket){

  int messagesReceived = 0;
  std::vector<long long int> latencies;
  
  TCPSocket* sock = (TCPSocket *)socket;
  
  struct timeval tv;
  while (messagesReceived < MESSAGES){
    char echoBuffer[RCVBUFSIZE];
    int recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE);
    if (recvMsgSize == RCVBUFSIZE){
      gettimeofday(&tv, NULL);
      std::string receivedMessage(echoBuffer);
      long long int sentTime = atoll(echoBuffer);
      long long int currentTime = tv.tv_sec * 1000000 + tv.tv_usec;
      long long int latency = currentTime - sentTime;
      if (latencies.size() < MESSAGES){
        latencies.push_back(latency);
      }
      messagesReceived++;
    }
  }
  if (flag == false){
    delete sock;
    return NULL;
  }

//  mtx.lock();
  if (flag == true){
    flag = false;
    std::ofstream of("latencies.txt");
    long long int sum = 0;
    for (std::vector<long long int>::iterator it = latencies.begin(); it != latencies.end(); it++){
      std::stringstream ss;
      ss << *it;
      std::string output = ss.str() + "\n";
      of << output;
      sum += *it;
    }
    of.close();
    
    std::sort(latencies.begin(), latencies.end());
    std::cout << "Median Latency is: " << latencies[latencies.size() / 2] << std::endl;
    std::cout << "Average Latency is: " << sum/latencies.size() << std::endl;
  }
//  mtx.unlock();
  delete sock;
  return NULL;
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
  connections++;
  pthread_create(&threads[thisId],NULL,worker,(void*)sock);
}
