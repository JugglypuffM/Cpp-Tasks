#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <map>
using namespace std;

extern const string PATH_TABLES;
extern const string FLAG;
extern map<string, int> size_of_types;
extern map <string, string[2]> key_attributes;
extern string* table_names;
extern int amount_tables;
extern string input;
extern ifstream in;

void write_to_log(string, string, int = 0);

long int get_hash(string);

int count(string, char);

bool is_int(string);
void Select();
void Edit();
void Delete();
void Add();
void Exams();
void Selected();
void Unselect();

class Table_hash {
	struct index_item {
		long int hash, index;
		bool deleted = false;
	};
	string address;
	index_item* index_table = nullptr;
	int n = 0;

public:
	void add(string, long int);
	void add(long int, long int);

	long int find_elem_with_index(long int);

	void delete_elem(long int);
	void activate_elem(long int);
	bool check_deleted(int);

	void replace_hash(int, int);

	long int find_position(long int);
	int find_hash(int);

	long int* get_indexes(long int, int*, bool = false);

	void print_table();
};

class Table {
	bool deleted = false;
	int size_of_entry = 0;
	void init_hash_tables();
public:
	map<string, int> hashed;
	string* attributes, * types, path;
	Table_hash** hash_tables;
	int amount_of_columns, n;

	Table(string, string, string);

	string* get_entry(int);
	string** get_all_entries();

	void delete_entry(int);
	void physically_clear_entries();

	void edit_entry(int, string*);

	void add_entry(string*);

	void print_table();
};

extern map <string, Table*> tables;
int* get_entries_matching(Table*, string*, int*, bool = false);
string* parse_entry(string, Table*);
