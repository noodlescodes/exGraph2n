#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <bitset>
#include <string>
#include <fstream>

using namespace std;

const int length = 6;
int perm[length];

uint32_t base_mat[length];
uint32_t created_mat[2 * length];

uint32_t top_magic = 4294901760;
uint32_t bottom_magic = 4294967295;

string working_permutations = "";

void valid_perms_to_file() {
	ofstream file;
	file.open("valid" + to_string(length) + ".txt");
	file << working_permutations;
	file.close();
}

// prints the current permutation
void print_perm() {
	for(int i = 0; i < length; i++) {
		cout << perm[i] << " ";
	}
	cout << endl;
}

// sets the initial permutation to use
void first_perm() {
	for(int i = 0; i < length; i++) {
		perm[i] = i;
	}
}

void add_perm_to_string() {
	for(int i = 0; i < length; i++) {
		working_permutations += to_string(perm[i]);
	}
	working_permutations += "\n";
}

// sets an entry in the base matrix
void set_entry_base_mat(uint32_t i, uint32_t j) {
	base_mat[i] |= (1 << (32 - j - 1));
	base_mat[j] |= (1 << (32 - i - 1));
}

// sets an entry in the created matrix
void set_entry_created_mat(uint32_t i, uint32_t j) {
	created_mat[i] |= (1 << (32 - j - 1));
	created_mat[j] |= (1 << (32 - i - 1));
}

// prints the base matrix
void print_base_mat() {
	for(int i = 0; i < length; i++) {
		cout << i << " " << bitset<32>(base_mat[i]) << endl;
	}
}

// prints the created matrix
void print_created_mat() {
	cout << "   ";
	for(int i = 0; i < 32; i++) {
		cout << i % 10;
	}
	cout << endl;
	for(int i = 0; i < 2 * length; i++) {
		if(i < 10) {
			cout << i << "  " << bitset<32>(created_mat[i]) << endl;
		}
		else {
			cout << i << " " << bitset<32>(created_mat[i]) << endl;
		}
	}
}

// the edge list for the base matrix goes here
void create_base_mat() {
	/*ex(10;4) Petersen graph
	set_entry_base_mat(0, 1);
	set_entry_base_mat(1, 2);
	set_entry_base_mat(2, 3);
	set_entry_base_mat(3, 4);
	set_entry_base_mat(0, 4);
	set_entry_base_mat(0, 5);
	set_entry_base_mat(1, 6);
	set_entry_base_mat(2, 7);
	set_entry_base_mat(3, 8);
	set_entry_base_mat(4, 9);
	set_entry_base_mat(5, 7);
	set_entry_base_mat(6, 8);
	set_entry_base_mat(7, 9);
	set_entry_base_mat(5, 8);
	set_entry_base_mat(6, 9);*/

	/*ex(6;4) C_6*/
	set_entry_base_mat(0, 1);
	set_entry_base_mat(1, 2);
	set_entry_base_mat(2, 3);
	set_entry_base_mat(3, 4);
	set_entry_base_mat(4, 5);
	set_entry_base_mat(5, 0);

	/*ex(4;4) P_4
	set_entry_base_mat(0, 1);
	set_entry_base_mat(1, 2);
	set_entry_base_mat(2, 3);*/

	/*ex(5;4) C_5
	set_entry_base_mat(0, 1);
	set_entry_base_mat(1, 2);
	set_entry_base_mat(2, 3);
	set_entry_base_mat(3, 4);
	set_entry_base_mat(4, 0);*/

	/*ex(3;4) P_3 
	set_entry_base_mat(0,1);
	set_entry_base_mat(1,2);*/
	
}

// creates the created matrix which is two copies
// of the base matrix without edges joining them
void create_created_mat() {
	for(int i = 0; i < length; i++) {
		created_mat[i] = base_mat[i];
		created_mat[i + length] = (base_mat[i] >> length);
	}
}

// resets the created matrix to two copies of the
// base matrix without edges joining them
void reset_created_mat() {
	for(int i = 0; i < length; i++) {
		created_mat[i] &= (top_magic << (16 - length));
	}
	for(int i = length; i < 2 * length; i++) {
		created_mat[i] &= (bottom_magic >> length);
	}
}

// checks for triangles
bool check_triangles() {
	for(int i = 0; i < 2 * length - 1; i ++) {
		for(int j = i + 1; j < 2 * length; j++) {
			if((created_mat[i] & (1 << (31 - j))) > 0) {
				if((created_mat[i] & created_mat[j]) > 0) {
					return true;
				}
			}
		}
	}
	return false;
}

// checks for squares
bool check_squares() {
	const unsigned int N = 1 << 31;
	for(int j = 0; j < 2 * length; j++) {
		for(int i = 0; i < 2 * length; i++) {
			if((i != j) && (((N >> j) & created_mat[i]) == 0) && __builtin_popcount(created_mat[i] & created_mat[j]) > 1) {
				return true;
			}
		}
	}
	return false;
}

// sets edges between the two base matrices.
void update_created_mat() {
	for(int i = 0; i < length; i++) {
		set_entry_created_mat(i, perm[i] + length);
	}
}

int main() {
	int number_working = 0;
	// set the first permutation
	first_perm();

	// set up the base matrix
	create_base_mat();
	// set up the created matrix which is two copies
	//of the base matrix without edges joining them
	create_created_mat();

	do {
		// make sure the matrix we're manipulating is the correct matrix
		reset_created_mat();

		// join the base matrices with the current permutation
		update_created_mat();

		// displace the current permutation
		//print_perm();

		// display the current matrix
		//print_created_mat();
		//cout << endl;
		
		// check for triangles (this should never happen)
		if(check_triangles()) {
			//cout << "++++++++++++++++" << endl;
			continue;
		}
		
		// check for squares (this should happen most of the time)
		if(check_squares()) {
			//cout << "#################" << endl;
			continue;
		}

		//cout << "Following perm works: ";
		//print_perm();
		//cout << endl << "=====================" << endl;
		
		// keep track of the number of permutations that work
		++number_working;

		// keep a list of all the permutations that work
		add_perm_to_string();
	} while(next_permutation(perm, perm + length));

	cout << "Writing all working maps to file" << endl;
	valid_perms_to_file();

	cout << "Total number of mappings that work: " << number_working << endl;
	return 0;
}
