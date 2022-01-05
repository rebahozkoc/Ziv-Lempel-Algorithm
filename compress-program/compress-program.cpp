#include <iostream>
#include <vector>
#include <fstream>

template <class HashedObj>
class HashTable
{
public:
    explicit HashTable(const HashedObj& notFound, int size = 101);
    HashTable(const HashTable& rhs)
        : ITEM_NOT_FOUND(rhs.ITEM_NOT_FOUND),
        array(rhs.array), currentSize(rhs.currentSize) { }

    const HashedObj& find(const HashedObj& x) const;

    void makeEmpty();
    void insert(const HashedObj& x);
    void remove(const HashedObj& x);

    enum EntryType { ACTIVE, EMPTY, DELETED };
private:
    struct HashEntry
    {
        HashedObj element;
        EntryType info;

        HashEntry(const HashedObj& e = HashedObj(), EntryType i = EMPTY)
            : element(e), info(i) { }
    };

    std::vector<HashEntry> array;
    int currentSize;
    const HashedObj ITEM_NOT_FOUND;

    bool isActive(int currentPos) const;
    int findPos(const HashedObj& x) const;
    void rehash();
};

// returns true if n is a prime number
bool isPrime(int n){
    if (n == 2 || n == 3)
        return true;

    if (n == 1 || n % 2 == 0)
        return false;

    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return false;

    return true;
}

// Finds the smallest prime number greater than n
int nextPrime(int n){
    if (n % 2 == 0)
        n++;

    for (; !isPrime(n); n += 2)
        ;

    return n;
}


template <class HashedObj>
void HashTable<HashedObj>::makeEmpty(){
    for (int i = 0; i < array.size(); i++) {
        // set every item to a default HashEntry (empty) 
        array[i] = HashEntry();
    }
    currentSize = 0;
}

// Hash a string with using the hash table size.
int hash(const std::string& key, int tableSize){
    int hashVal = 0;

    for (int i = 0; i < key.length(); i++)
        hashVal = 37 * hashVal + key[i];

    hashVal = hashVal % tableSize;

    if (hashVal < 0)
        hashVal = hashVal + tableSize;

    return(hashVal);

}

/*Find the right position index for an element*/
template <class HashedObj>
int HashTable<HashedObj>::findPos(const HashedObj& x) const {

    int p = hash(x, array.size()); // current position

    //linear probing
    while (array[p].info != EMPTY && array[p].element != x) {
        // look at the succeeding index if it is empty or is the element x
        p += 1; 

        if (p >= array.size())             // perform the mod
            p = p - array.size();          // if necessary
    }

    return p;
}


/*** Construct the hash table.*/
template <class HashedObj>
HashTable<HashedObj>::HashTable(const HashedObj& notFound, int size)
    : ITEM_NOT_FOUND(notFound), array(nextPrime(size)), currentSize(0){
    makeEmpty();
}


template <class HashedObj>
bool HashTable<HashedObj>::isActive(int currentPos) const{
    return array[currentPos].info == ACTIVE;
}

/*Remove the element if it is in the table*/
template <class HashedObj>
void HashTable<HashedObj>::remove(const HashedObj& x){
    int currentPos = findPos(x);

    if (isActive(currentPos))
        array[currentPos].info = DELETED;
}

/* Find and return the reference of an onject. Returns the NOTFOUND item if not not found*/
template <class HashedObj>
const HashedObj& HashTable<HashedObj>::find(const HashedObj& x) const{
    int currentPos = findPos(x);

    if (isActive(currentPos))
        return array[currentPos].element;

    return   ITEM_NOT_FOUND;
}


/**
* Insert item x into the hash table. If the item is
* already present, then do nothing.
*/
template <class HashedObj>
void HashTable<HashedObj>::insert(const HashedObj& x){
    // Insert x as active
    int currentPos = findPos(x);

    if (isActive(currentPos))
        return;

    array[currentPos] = HashEntry(x, ACTIVE);

    // enlarge the hash table if necessary 
    if (++currentSize >= array.size() / 2)
        rehash();
}


/**
* Expand the hash table.
*/
template <class HashedObj>
void HashTable<HashedObj>::rehash(){
    std::vector<HashEntry> oldArray = array;

    // Create new double-sized, empty table
    array.resize(nextPrime(2 * oldArray.size()));

    for (int j = 0; j < array.size(); j++) {
        array[j].info = EMPTY;
    }
    // Copy table over
    currentSize = 0;
    for (int i = 0; i < oldArray.size(); i++)
        if (oldArray[i].info == ACTIVE)
            insert(oldArray[i].element);
}

/*Key-value pairs for storing dict elements of the zipping algorithm*/
struct item {
    int key;
    std::string value;
    item() : key(0), value(""){}
    item(int key, std::string val= "") : key(key), value(val) {}
    // returns true if the values of the items are the same. Keys are not important.
    bool operator==(const item& rhs) const{
        return this->value == rhs.value;
    }
    bool operator!=(const item& rhs) const{
        return !(*this == rhs);
    }
};

/*Re-implement the hash function for the item struct.*/
int hash(const item& key, int tableSize) {
    return hash(key.value, tableSize);
}


int main(){
    item NOTFOUND = item(-1, std::string(""));
    

    // Create the hash table with the all ascii values.
    HashTable<item> h_table = HashTable<item>(NOTFOUND, 512);
    for (int i = 0; i < 256; i++) {
        char temp = i;
        h_table.insert(item(i, std::string(1, temp)));
    }

    std::ifstream in_file("compin");
    std::ofstream o_file("compout");

    int counter = 255;
    std::string p = "";
    char c;
    // iterate through compin char-by-char.
    while (!in_file.eof()){
        in_file.get(c);
        // If the current prefix exists in the hash table add one more char
        if (h_table.find(item(0, p + c)) != NOTFOUND) {
            p = p + c;
        }
        else {
            // If Hash table has lower items than the given threshold add prefix + first char to hash table
            if (counter < 4095) {
                h_table.insert(item(++counter, p + c));
            }
            // Print out the corresponding key for the prefix.
            o_file  << h_table.find(item(0, p)).key << " ";
            std::cout << h_table.find(item(0, p)).key << " ";
            p = c;
        }
    }
    in_file.close();
    o_file.close();
    return 0;
}