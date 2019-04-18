// Assignment 5 COP4530
// Ryan Winter
// 11/21/17 (Fall 2017)

template <typename K, typename V>
HashTable<K, V>::HashTable(size_t size){ // 1 param constructor
	hTableSize = 0;
	hTable.resize(prime_below(size));			
}

template <typename K, typename V>
HashTable<K, V>::~HashTable(){ // destructor
	makeEmpty();
}

template <typename K, typename V>
bool HashTable<K, V>::contains(const K & k) const{ // check if key k is in the hash hTable.
	for(auto itr = hTable[myhash(k)].begin(); itr != hTable[myhash(k)].end(); ++itr){  // search through the linked list at index myhash(k)
		if(itr->first == k) // if itr-> first is the username then pass true back (user found)
			return true;
	}
	return false;
}
 
template <typename K, typename V>
bool HashTable<K, V>::match(const std::pair<K, V>& kv) const{ // check if key-value pair is in the hash hTable.	
	auto & whichList = hTable[myhash(kv.first)];						// sourced from lecture slides
	return find(begin(whichList), end(whichList), kv) != end(whichList);
}

template <typename K, typename V>
bool HashTable<K, V>::insert(const std::pair<K, V>& kv){ // add the key-value pair kv into the hash hTable. Don't add if kv is already in the hash hTable. If the key is the hash hTable but with a different value, the value should be updated to the new one with kv. Return true if kv is inserted or the value is updated; return false otherwise (i.e., if kv is in the hash hTable).
	int temp = hTableSize + 1;
	auto & whichList = hTable[myhash(kv.first)];						// sourced from lecture slides
	if(find(begin(whichList), end(whichList), kv) != end(whichList)) // if username/password is already in hashtable, return false
		return false;
	if(contains(kv.first) == false){ // if it doesnt contain the username then just add it
		whichList.push_back(kv);
		hTableSize++;
	}
	else{ // if table does contain the username then password must be different so change the password
		remove(kv.first);
		whichList.push_back((kv));
		hTableSize++;
	}
	if (temp > hTable.size())
		rehash();
	return true;
}

template <typename K, typename V>
bool HashTable<K, V>::insert(std::pair<K, V>&& kv){ // move version of insert.
	int temp = hTableSize + 1;
	auto & whichList = hTable[myhash(kv.first)];						// sourced from lecture slides
	if(find(begin(whichList), end(whichList), kv) != end(whichList))// if username/password is already in hashtable, return false
		return false;
	if(contains(kv.first) == false){ // if it doesnt contain the username then just add it
		whichList.push_back(move(kv));
		hTableSize++;
	}
	else{ // if table does contain the username then password must be different so change the password
		remove(kv.first);
		whichList.push_back(move(kv));
		hTableSize++;
	}
	if (temp > hTable.size())
		rehash();
	return true;
}

template <typename K, typename V>
bool HashTable<K, V>::remove(const K& k){ // delete the key k and the corresponding value if it is in the hash hTable. Return true if k is deleted, return false otherwise (i.e., if key k is not in the hash hTable).
	auto & whichList = hTable[myhash(k)];
	for(auto itr = whichList.begin(); itr != whichList.end(); ++itr){
		if(itr == end(whichList))
			return false;
		if(itr->first == k){
			whichList.erase(itr);
			hTableSize--;
			return true;
		}
	}
}

template <typename K, typename V>
void HashTable<K, V>::clear(){ // delete all elements in the hash hTable
	makeEmpty();
}
 
template <typename K, typename V>
bool HashTable<K, V>::load(const char* filename){ // load the content of the file with name filename into the hash hTable. In the file, each line contains a single pair of key and value, separated by a white space.
	ifstream instream;
	instream.clear();
	instream.open(filename);
	K key;
	V value;
	
	if(!instream) // file failed to open so return false
		return false;
	
	while(!instream.eof()){
		instream >> key >> value;
		insert(make_pair(key, value));
	}	

	instream.close();
	return true;
}

template <typename K, typename V>
void HashTable<K, V>::dump(){ // display all entries in the hash hTable. If an entry contains multiple key-value pairs, separate them by a semicolon character (:) (see the provided execuhTable for the exact output format).
	for(int i = 0; i < hTable.size(); i++){
		cout << "v[" << i << "]: ";
		for(auto itr = hTable[i].begin(); itr != hTable[i].end(); ++itr){
			if(itr != hTable[i].begin()) // only print colon when there are multipule key-value pairs
				cout << ":";
			cout << itr->first << " " << itr->second;
		}
		cout << endl;
	}
}

template <typename K, typename V>
bool HashTable<K, V>::write_to_file(const char* filename){ // write all elements in the hash hTable into a file with name filename. Similar to the file format in the load function, each line contains a pair of key-value pair, separated by a white space.
	ofstream outstream;
	outstream.open(filename);
	K key;
	V value;

	if(!outstream) // if error creating file return false
		return false;

	for(int i = 0; i < hTable.size(); i++){
		auto& whichList = hTable[i];
		auto itr = whichList.begin();

		if(!whichList.empty()){
			outstream << itr->first << " " << itr->second << endl;
			while(++itr != whichList.end()){
				outstream << itr->first << " " << itr->second << endl;
			}
		}
	}
	outstream.close();
	return true;
}

template <typename K, typename V>
void HashTable<K, V>::makeEmpty(){ // delete all elements in the hash hTable. The public interface clear() will call this function.
	hTableSize = 0; // reset the size variable to 0
	for(auto & x : hTable){ // sourced from lecture slides
		x.clear();
	}
}

template <typename K, typename V>
void HashTable<K, V>::rehash(){ // Rehash function. Called when the number of elements in the hash hTable is greater than the size of the vector.
	auto oldLists = hTable;				// code sourced from book section 5.5
	hTable.resize(prime_below(2 * hTable.size()));
	makeEmpty();
	for(auto & thisList : oldLists)
		for(auto & x : thisList)
			insert(move(x));
}

template <typename K, typename V>
size_t HashTable<K, V>::myhash(const K& k) const{ // return the index of the vector entry where k should be stored.
	
	// needs to handle collisons...??
	
	static hash<K> hashT;
	return hashT(k) % hTable.size();
}

template <typename K, typename V>
int HashTable<K, V>::getSize(){
	return hTableSize;
}

// GIVEN

template <typename K, typename V>
unsigned long HashTable<K, V>::prime_below (unsigned long n)
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
}

//Sets all prime number indexes to 1. Called by method prime_below(n) 
template <typename K, typename V>
void HashTable<K, V>::setPrimes(std::vector<unsigned long>& vprimes)
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
}