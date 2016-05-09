#ifndef __PEER_H__
#define __PEER_H__
#include <openssl/sha.h>
#include "processor.hpp"
#include "communication.hpp"
#include "rsa.hpp"
#include "client.hpp"
#include "server.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <queue>
#include <openssl/rsa.h>

unsigned char* SHA1(const unsigned char* s, size_t size, unsigned char* md);

typedef struct comm_thread_args {
	synchronized_queue<transaction*>* tq;
	synchronized_queue<block*>* bq;
	synchronized_queue<std::string*>* peerq;
	blockchain* bc;
	Client* client;
} comm_thread_args;

typedef struct processing_thread_args {
	synchronized_queue<transaction*>* tq;
	synchronized_queue<block*>* bq;
	synchronized_queue<std::string*>* peerq;
	blockchain* bc;
	Client* client;
} processing_thread_args;

#endif
