#include "processor.hpp"


using namespace std;
using namespace std::chrono;

///////////////////////////////
///// block mplementation /////
///////////////////////////////

unsigned char* block::calculate_merkle_root() {
	// Calculate and return the Merkle root. 

	// For now, we can just implement this by calculating the 
	// SHA1 of the first element.
	// TODO: This probably needs to be heap allocated.
	unsigned char* hash = new unsigned char[SHA_DIGEST_LENGTH];
	const unsigned char* data_to_hash = 
	    (const unsigned char*) transaction_array[0]->sender_public_key.c_str();
 	SHA1(data_to_hash, strlen((const char*)data_to_hash) + 1, hash);
 	return hash;

}
  
char* block::verify_block_number() {
 // Asks neighbors for the previous block, and checks to make sure its 
 // block_number is one less than this one's.
  return NULL;
}

unsigned char* block::calculate_finhash() {
	unsigned char* hash = new unsigned char[SHA_DIGEST_LENGTH];
	const unsigned char* data_to_hash = this->calculate_merkle_root();
	unsigned char* buffer =
			new unsigned char[SHA_DIGEST_LENGTH + sizeof(unsigned long long)];
	memcpy(buffer, data_to_hash, SHA_DIGEST_LENGTH);
	memcpy(buffer + SHA_DIGEST_LENGTH, &this->magic, sizeof(unsigned long long));
	SHA1(buffer, SHA_DIGEST_LENGTH + sizeof(unsigned long long), hash);
	return hash;
}

/////////////////////////////////////
///// blockchain implementation /////
/////////////////////////////////////

// Check if a new block is valid to be added to this blockchain.
// Make sure that none of the transactions in this block
// are already in the blockchain.
bool blockchain::verify_transactions(block* b) {
	// Iterate over all transactions in the block.
	for (int i = 0; i < NUM_TRANSACTIONS_PER_BLOCK; ++i) {
		// If you ever find one, return false.
		if (voted.find(b->transaction_array[i]->sender_public_key) 
			                                               != voted.end()) {
			return false;
		}
	}
	return true;
}

// Overloaded function - this will just check if the current
// transactions in blockchain are valid.
bool blockchain::verify_transactions() {
  // Iterate over all of the blocks, and check that there are
  // not two transactions from the same person.
	return true;
}

BlockList::iterator blockchain::check_if_block_in_chain(block* b) {
	// Iterate over the blocks, and check if any have a matching final hash.
	for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
		if (strcmp((*it)->finhash, b->finhash) == 0) {
			return it;
		}
	}
	return blocks_.end();
}

block* get_parent_block_from_neighbor(block* b) {
	return NULL;
}

void blockchain::repair_blockchain(block* b) {
	// This list will contain the blocks that need to be added to blockchain.
	std:list<block*> blocks_needed;

	// Get all of the blocks that need to be added to chain
	block* current_block = b;
	BlockList::iterator it;

	while(true) {
		it = check_if_block_in_chain(current_block);
		if (it != blocks_.end())
			break;
		blocks_needed.push_back(current_block);
		block* current_block = get_parent_block_from_neighbor(current_block);
	}

	// Now blocks_needed contains the blocks needed to be added to chain.
	// Also "it" is the iterator positioned at the location in the blockchain
	// where the two chains diverge.

	return;

}

block_validity_code blockchain::check_block_validity(block* b) {
  // Make sure none of the votes had previously been made.

  // Make sure the new block has the correct prev block.
  block* head_block = get_head_block();
  if (b->prev_block_SHA1 != head_block->merkle_root)
    return PREV_BLOCK_NONMATCH;

  bool transaction_verification = verify_transactions(b);
  if (!transaction_verification) 
    return TRANSACTION_INVALID;
  
  // If everything checks out, return 1.
  return OK;
}

bool blockchain::add_block(block* b) {
  // Call the appropriate code to make sure all of the transactions are valid.
  int block_validity_result = check_block_validity(b);
  if (block_validity_result == 1) {
    // Add block to the blockchain
    chain_length++;
    blocks_.push_back(b);
    return true;
  }
  return false;
}

block* blockchain::get_head_block(){
	// Get the head block. Don't remove it from the chain.
	if (chain_length == 0)
		return NULL;
	return blocks_.back();
}

block::block() {
	cout << "called block constructor\n";
	prev_block_SHA1 = NULL;
	magic = 0;
	merkle_root = NULL;
	max_ind = 0;
	verifier_public_key = NULL;
	finhash = NULL;
}

/////////////////////////////////////////////
///// Synchronized Queue Implementation /////
/////////////////////////////////////////////


template <class T>
void synchronized_queue<T>::init() {
	pthread_mutex_init(&lock_, NULL);
	pthread_cond_init(&cv_, NULL);
}

template <class T>
void synchronized_queue<T>::push(T t) {
	pthread_mutex_lock(&lock_);
	queue_.push(t);
	pthread_cond_signal(&cv_);
	pthread_mutex_unlock(&lock_);
}

template <class T>
T synchronized_queue<T>::pop() {
	T ret;
	pthread_mutex_lock(&lock_);
	while (queue_.size() == 0) {
		pthread_cond_wait(&cv_, &lock_);
	}
	ret = queue_.back();
	queue_.pop();
	pthread_mutex_unlock(&lock_);
	return ret;
}

template <class T>
T synchronized_queue<T>::pop_nonblocking() {
	T ret;
	pthread_mutex_lock(&lock_);
	if (queue_.size() == 0) {
		pthread_mutex_unlock(&lock_);
		return NULL;
	}
	ret = queue_.back();
	queue_.pop();
	pthread_mutex_unlock(&lock_);
	return ret;
}

template <class T>
bool synchronized_queue<T>::empty() {
	return queue_.size() == 0;
}

template class synchronized_queue<transaction*>;
template class synchronized_queue<block*>;
