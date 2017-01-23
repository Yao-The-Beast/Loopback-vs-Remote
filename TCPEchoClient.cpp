/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PracticalSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>



using namespace std;

const unsigned int MESSAGES = 100000;
const int NUM_THREADS = 1;
int echoStringLen = 128; 

string relayAddr = "127.0.0.1";  
unsigned short relayPort = 1111;

void* worker(void* threadId){
  try {
    // Establish connection with the echo server
    TCPSocket sock(relayAddr, relayPort);
  
    int messagesSent = 0;
    struct timeval tv;
    while (messagesSent < MESSAGES){
      gettimeofday(&tv,NULL);
      long long int currentTime = tv.tv_sec * 1000000 + tv.tv_usec;
      char echoString[128];
      sprintf(echoString, "%lld", currentTime);
      sock.send(echoString, echoStringLen); 
      messagesSent++;
      usleep(50);
    }
    sock.closeSocket();
  } catch(SocketException &e) {
    cerr << e.what() << endl;
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  
  pthread_t threads[NUM_THREADS];
  for (int i =0; i < NUM_THREADS; i++){
    int feedback = pthread_create(&threads[i], NULL, worker, (void *)i);
    if (feedback != 0) 
      cout << "error" ;
  }
  
  for (int i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i],NULL);
  }
  std::cout << "END OF CLIENT" << std::endl;
  return 0;
}
