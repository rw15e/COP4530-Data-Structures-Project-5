// Assignment 5 COP4530
// Ryan Winter
// 11/21/17 (Fall 2017)

#include "passserver.h"
#include "hashtable.h"
#include <termios.h> // needed for the hiding of passwords

using namespace std;
using namespace cop4530;

unsigned long prime_below(unsigned long x);				// two helpful functions to determine the proper prime numbers used in setting up the vector size. Whenever you need to set hash table to a new size "sz", call prime_below(sz) to determine the new proper underlying vector size. These two functions have been provided in hashtable.h and hashtable.hpp.
void setPrimes(std::vector<unsigned long>& primevec);
static const unsigned int max_prime = 1301081;

void Menu(){
	cout << "\n\n";
	cout << "l - Load From File" << endl;
	cout << "a - Add User" << endl;
	cout << "r - Remove User" << endl;
	cout << "c - Change User Password" << endl;
	cout << "f - Find User" << endl;
	cout << "d - Dump HashTable" << endl;
	cout << "s - HashTable Size" << endl;
	cout << "w - Write to Password File" << endl;
	cout << "x - Exit program" << endl;
	cout << "\nEnter choice : ";
};

unsigned long prime_below(unsigned long n)
{
  if (n > max_prime)
    {
      std::cerr << "** input too large for prime_below()\n";
      return 0;
    }
  if (n == max_prime)
    {
      return max_prime;
    }
  if (n <= 1)
    {
		std::cerr << "** input too small \n";
      return 0;
    }

  // now: 2 <= n < max_prime
  std::vector <unsigned long> v (n+1);
  setPrimes(v);
  while (n > 2)
    {
      if (v[n] == 1)
	return n;
      --n;
    }

  return 2;
};

void setPrimes(std::vector<unsigned long>& vprimes)
{
  int i = 0;
  int j = 0;

  vprimes[0] = 0;
  vprimes[1] = 0;
  int n = vprimes.capacity();

  for (i = 2; i < n; ++i)
    vprimes[i] = 1;

  for( i = 2; i*i < n; ++i)
    {
      if (vprimes[i] == 1)
        for(j = i + i ; j < n; j += i)
          vprimes[j] = 0;
    }
};

int main(){
	int pHashSize = 0; // holds preferred hash size (from user)
	string menuChoice = " "; // holds users menu choice
	string file = " "; // holds users filename
	string user = " "; // holds users username
	string pass = " "; // holds users password
	struct termios systemFlags, localFlags; // used for hiding password
	tcgetattr(fileno(stdin), &systemFlags);
    localFlags = systemFlags;
    localFlags.c_lflag &= ~ECHO;
	
	cout << "Enter preferred hash table capacity: ";
	cin >> pHashSize;
	
	if(pHashSize <= 1){
		cout << "** input too small" << endl;
		cout << "set to default capacity" << endl;
		pHashSize = 11;	
	}
	cout << prime_below(pHashSize) << endl;
	
	//cout << endl << pHashSize << endl << "PHASH SIZE" << endl;				// testing..
	
	PassServer server(pHashSize); // create passserver based on preferred hash size
	
	do{
		Menu(); // print the menu
		cin >> menuChoice; // accept users menu choice
		
		if(menuChoice == "l"){ // load from file
			cout << "Enter password file name to load from: ";
			cin >> file; 
			if(server.load(file.c_str()) == false) 				// if file doesnt exist or cant be opened
				cout << "Error: Cannot open file " << file;
			cout << endl;
		}
		else if(menuChoice == "a"){ // add user	
			cout << "Enter username: ";
			cin >> user;
			cout << "Enter password: ";
			
            tcsetattr(fileno(stdin), TCSANOW, &localFlags); // turns user echo off
			cin >> pass;	// take in input with echo off
			tcsetattr(fileno(stdin), TCSANOW, &systemFlags); // turns user echo back on

			if(server.addUser(make_pair(user, pass)) == true)
				cout << endl << "User " << user << " added.";
			else
				cout << "*****Error: User already exists. Could not add user.";
			cout << endl;
		}
		else if(menuChoice == "r"){ // remove user 
			cout << "Enter username: ";
			cin >> user;
			if(server.removeUser(user) == true)
				cout << "User " << user << " deleted.";
			else
				cout << "*****Error: User not found.  Could not delete user";
			cout << endl;
		}
		else if(menuChoice == "c"){ // change user password
			string newPass = " ";
			cout << "Enter username    : ";
			cin >> user;
			cout << "Enter password    : ";
			
            tcsetattr(fileno(stdin), TCSANOW, &localFlags);	// turns user echo off
            cin >> pass;
			tcsetattr(fileno(stdin), TCSANOW, &systemFlags); // turns user echo back on
			tcsetattr(fileno(stdin), TCSANOW, &localFlags);	 // turns user echo off
            cout << endl << "Enter new password: ";
            cin >> newPass;
			tcsetattr(fileno(stdin), TCSANOW, &systemFlags); // turns user echo back on
			
			cout << endl;
			if(server.changePassword(make_pair(user, pass), newPass) == true)
				cout << "Password changed for user " << user;
			else
				cout << "*****Error: Could not change user password";
			cout << endl;
		}
		else if(menuChoice == "f"){ // find user
			cout << "Enter username    : ";
			cin >> user;
			cout << endl;
			if(server.find(user) == true)
				cout << "User '" << user << "' found.";
			else
				cout << "User '" << user << "' not found.";
			cout << endl;
		}
		else if(menuChoice == "d") // dump hash table
			server.dump();
		else if(menuChoice == "s") // hash table size
			cout << "Size of hashtable: " << server.size() << endl;
		else if(menuChoice == "w"){ // write to password file
			cout << "Enter password file name to write to: ";
			cin >> file;
			if(server.write_to_file(file.c_str()) == false)
				cout << "Error: File write failure" << endl;
		}
		else if(menuChoice == "x") // exit program
			exit(0); 
		
		else
			cout << "*****Error: Invalid entry.  Try again.";

	}while(menuChoice != "x"); // run until user enters x (quit option)
		
	return 0; 
}