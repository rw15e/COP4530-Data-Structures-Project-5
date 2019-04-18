// Assignment 5 COP4530
// Ryan Winter
// 11/21/17 (Fall 2017)

#include "passserver.h"
#include "hashtable.h"

PassServer::PassServer(size_t size) : pServer(size){ // default constructor
	//HashTable<string,string> pServer(size);
}
PassServer::~PassServer(){ // destructor
	pServer.clear();
}

bool PassServer::load(const char* filename){ // load a password file into the HashTable object. Each line contains a pair of username and encrypted password.
	return(pServer.load(filename));
}

bool PassServer::addUser(std::pair<string, string>& kv){ //  add a new username and password.  The password passed in is in plaintext, it should be encrypted before insertion.
	return(pServer.insert(make_pair(kv.first,encrypt(kv.second))));
}

bool PassServer::addUser(std::pair<string, string>&& kv){ // move version of addUser.
	return(pServer.insert(move(make_pair(kv.first,encrypt(kv.second)))));
}

bool PassServer::removeUser(const string& k){ // delete an existing user with username k.
	return(pServer.remove(k));
}

bool PassServer::changePassword(const pair<string, string>& p, const string& newpassword){ //  change an existing user's password. Note that both passwords passed in are in plaintext. They should be encrypted before you interact with the hash table. If the user is not in the hash table, return false. If p.second does not match the current password, return false. Also return false if the new password and the old password are the same (i.e., we cannot update the password).
	if(pServer.match(make_pair(p.first, encrypt(p.second))) == true && encrypt(p.second) != encrypt(newpassword)){ // encrypt password before interacting with the hash table!
		pServer.insert(make_pair(p.first, encrypt(newpassword)));
		return true;
	}
	return false;
}

bool PassServer::find(const string& user){ // check if a user exists (if user is in the hash table).
	return(pServer.contains(user));
}

void PassServer::dump(){ // show the structure and contents of the HashTable object to the screen. Same format as the dump() function in the HashTable class template.
	pServer.dump();
}

size_t PassServer::size(){ // return the size of the HashTable (the number of username/password pairs in the table).
	return(pServer.getSize());
}

bool PassServer::write_to_file(const char* filename){ // save the username and password combination into a file. Same format as the write_to_file() function in the HashTable class template.
	return(pServer.write_to_file(filename));
}

string PassServer::encrypt(const string& str){ // encrypt the parameter str and return the encrypted string.
	char salt[] = "$1$########";
	char *password = new char [100];
	strcpy(password, str.c_str()); // convert str to char array password
	string newPass = crypt(password, salt);
	return newPass.substr(12); // cut off first 12 characters (we only need the next 22 of 34)
}