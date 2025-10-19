#include <iostream>
#include <fstream>
#include <string>

class AlphabeticalHashTable { //I used ChatGPT for putting some parts of the hashmap together
    private:
        struct Bucket { //alphabetical buckets such as bucket a, bucket b, etc.
            std::string* words;
            int size;
            int capacity;

            //BUCKET FUNCTIONS
            //Constructor
            Bucket() : words(nullptr), size(0), capacity(0) {}

            //Destructor
            ~Bucket() {delete[] words;}

            //Adding New Word To a Bucket
            void push_back(const std::string& word) { 
                if (size == capacity) {

                    int new_capacity = capacity == 0 ? 1 : capacity * 2; //increase capacity

                    std::string* new_words = new std::string[new_capacity]; //allocate memory for the new array with more capacity
                    for (int i = 0; i < size; ++i) {
                        new_words[i] = words[i];
                    }
                    delete[] words; //deallocate memory for the previously allocated memory

                    words = new_words;
                    capacity = new_capacity;
                }
                words[size++] = word; //once memory is allocated add the word to the array
            }

            //Checking If a Word Is In a Bucket
            bool contains(const std::string& word) { //O(n) time complexity :(
                for (int i = 0; i < size; ++i) { 
                    if (words[i] == word) return true;
                }
                return false;
            }

            //Sorting - I took heap functions from a previous assignment I did last semester
            void heapSort() {
                //Build a max heap
                int n = size;
                for (int i = n / 2 - 1; i >= 0; i--)
                    heap(n, i);

                //Extract elements from the heap one by one
                for (int i = n - 1; i > 0; i--) {
                    std::swap(words[0], words[i]); //Move the current root (largest element) to the end of the array
                    heap(i, 0);
                }
            }

            void heap(int n, int i) {
                int max = i; //stores max value found in a sub portion
                int l = 2 * i + 1;
                int r = 2 * i + 2;

                //Udate max value found
                if (l < n && words[l] > words[max])
                    max = l;

                if (r < n && words[r] > words[max])
                    max = r;

                //If max isn't initial max change the order to store max in the order it should be stored in
                if (max != i) {
                    std::swap(words[i], words[max]);
                    heap(n, max);
                }
            }
        };

        Bucket* buckets;
        static const int ALPHABET_SIZE = 26;

        //HASH MAP FUNCTIONS
        //Collision Handleing Function
        int hashFunction(const std::string& key) const { 
            if (key.empty()) return -1; 
            char intial = tolower(key[0]);

            if (intial < 'a' || intial > 'z') return -1; 
            return intial - 'a'; //returns index based on alphabetical index
        }

    public:
        //Constructor
        AlphabeticalHashTable() : buckets(new Bucket[ALPHABET_SIZE]) {}

        //Destructor
        ~AlphabeticalHashTable() {delete[] buckets;} //I didn't really test this

        //Inserts Item to Hash Map
        void insert(const std::string& key) {
            int index = hashFunction(key); //finds index
            if (index == -1) return; //if error occurs, just skip the word
            buckets[index].push_back(key); 
        }

        //Checks If Item Is In Hash Map
        bool find(const std::string& key) const {
            int index = hashFunction(key); //determines the right bucket to check
            if (index == -1) return false;
            return buckets[index].contains(key); //checks if it is in the bucket
        }

        //Adds Words to a Hash Map from File given
        void add_words(const std::string& filename) {
            //locate file
            std::ifstream file(filename);
            std::string word;

            //error handling
            if (!file) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            //iterate over the file to add words
            while (std::getline(file, word)) {
                insert(word);
            }
        }

        //Compares Words from Hash Map to File given
        void compare_words(const std::string& filename, AlphabeticalHashTable& targetHashTable) {
            //locate file
            std::ifstream file(filename);
            std::string word;

            //error handling
            if (!file) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            //check if each word is the previous hashmap given
            while (std::getline(file, word)) {
                if (find(word)) {
                    targetHashTable.insert(word); //if yes insert it into the new hash map
                }
            }
        }

        //Sorting All Buckets in Hash Map
        void sort_all_buckets() { 
            for (int i = 0; i < ALPHABET_SIZE; i++) {
                buckets[i].heapSort();
            }
        }

        //Writes Words from Hash Map from File
        void write_words(const std::string& filename) const {
            //open file to write
            std::ofstream file(filename);

            //error handling
            if (!file) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            //go over the hashmap and add it to the file
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                for (int j = 0; j < buckets[i].size; ++j) {
                    file << buckets[i].words[j] << "\n";
                }
            }
        }
    };


int main() {
    AlphabeticalHashTable hashTable;
    hashTable.add_words("f1.txt"); //adds contents of "f1.txt" to hashTable

    AlphabeticalHashTable foundWords;
    hashTable.compare_words("f2.txt", foundWords); //adds common contents of hashTable and "f2.txt" to foundWords
    foundWords.sort_all_buckets(); //sort foundWords in alphabetical order

    foundWords.write_words("out.txt"); //writes foundWords to "out.txt"

    return 0;
}