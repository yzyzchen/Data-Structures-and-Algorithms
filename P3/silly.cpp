// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <vector>
#include <iostream>
#include <utility>
#include <map>
#include <unordered_map>
#include <exception>
#include <algorithm>
#include "getopt.h"
#include "TableEntry.h"
#include <limits>

//PROGRAMMER: YUZHOU CHEN
//PROGRAM NAME: MINE ESCAPE
//DATE: 03/30/2024

using namespace std;
/* --- Helper Functions --- */
TableEntry convert(EntryType& t_type, string& t_value) {
	//String
	if (t_type == EntryType::String) return TableEntry(t_value);
	//Double
	else if (t_type == EntryType::Double) return TableEntry(stod(t_value));
	//Int
    else if (t_type == EntryType::Int) return TableEntry(stoi(t_value));
	//Boolean
	else return TableEntry(t_value == "true");
}

EntryType convertType(char& type){
    //String
	if (type == 's') return EntryType::String;
	//Double
	else if (type == 'd') return EntryType::Double;
	//Int
    else if (type == 'i') return EntryType::Int;
	//Boolean
	else return EntryType::Bool;
}

/* --- Less Than Functor Class --- */
class LessThan {
private:
	size_t m_colNum;
	TableEntry m_value;

public:
	LessThan(size_t t_colNum, TableEntry t_value) : 
		m_colNum{ t_colNum }, m_value{ t_value } {}

	bool operator()(vector<TableEntry>& t_row) {
		return t_row[m_colNum] < m_value;
	}

	bool operator()(const TableEntry& t_entry) {
		return t_entry < m_value;
	}

	bool operator()(pair<const TableEntry, int>& t_pair) {
		return t_pair.first < m_value;
	}
};


/* --- Equal to Functor Class --- */
class Equal {
private:
	size_t m_colNum;
	TableEntry m_value;

public:
	Equal(size_t t_colNum, TableEntry t_value) :
		m_colNum{ t_colNum }, m_value{ t_value } {}

	bool operator()(vector<TableEntry>& t_row) {
		return t_row[m_colNum] == m_value;
	}

	bool operator()(const TableEntry& t_entry) {
		return t_entry == m_value;
	}

	bool operator()(pair<const TableEntry, int>& t_pair) {
		return t_pair.first == m_value;
	}
};

/* --- Greater Than to Functor Class --- */
class GreaterThan {
private:
	size_t m_colNum;
	TableEntry m_value;

public:
	GreaterThan(size_t t_colNum, TableEntry t_value) :
		m_colNum{ t_colNum }, m_value{ t_value } {}

	bool operator()(vector<TableEntry>& t_row) {
		return t_row[m_colNum] > m_value;
	}

	bool operator()(const TableEntry& t_entry) {
		return t_entry > m_value;
	}

	bool operator()(pair<const TableEntry, int>& t_pair) {
		return t_pair.first > m_value;
	}
};


class Table{
    private:
	//real containers
        vector<vector<TableEntry>> table;
        //metadata
		//columns_types, same as the sequence in the "table" indexing
        vector<EntryType> columns_types;

	//hashed column index
		//hashed columns_types(string), values are the index 
        unordered_map<string, size_t> columns_hash;

	//indexing
        //binary search tree, size_t is the row_number
        map<TableEntry, vector<size_t>> bst;
        //hashing, size_t is the row_number
        unordered_map<TableEntry, vector<size_t>> hash;

        //table name
        string t_name;
        //table total columns
        size_t colNum;
        //index type
        string index_type;
		//index column
        size_t index_col_num;


	//functions
        //default ctor
        Table(): t_name{""}{};
        //value ctor
        Table(string table_name, size_t num_col): t_name{table_name}, colNum{num_col}{
            columns_types.reserve(num_col);
            columns_hash.reserve(2 * num_col);
        };
        //create a table, start from reading colNames
        void insertRows();
		//printing ALL
        void printALL(vector<string> vec);
		//return printing count
        size_t printWHERE(vector<string> vec, size_t obj, string OP, bool isQuiet);
		//delete
        void deleteWHERE(size_t columnNumber, string method_type, string obj_name);
		//should be called after insert(), delete() and DataBase::join()
        void updateIndex();

        friend class DataBase;
};

void Table::insertRows(){
    string useless, input;
    size_t add_row_num, cur_size = table.size();
    cin >> add_row_num >> useless;

    //reserve rows first
    size_t row_num = cur_size + add_row_num;
    table.reserve(row_num);
    
    for(size_t row = cur_size; row < row_num; row++){
        //reserve the col, then insert
        //table[row].reserve(columns_types.size());
        //start inserting
        vector<TableEntry> aux;
        aux.reserve(colNum);
        for (size_t i = 0; i < colNum; i++){
            cin >> input;
            aux.emplace_back(convert(columns_types[i], input));
            // cout << table[row][i] << " ";
        }
        //cout << endl << table.size() << endl;
        table.emplace_back(aux);

    }
    cout << "% " << "Added " << add_row_num <<" rows to " << t_name <<" from position " << cur_size << " to " << row_num - 1 << endl;
};


size_t Table::printWHERE(vector<string> vec, size_t obj, string OP, bool isQuiet){
	string obj_name;
	cin >> obj_name;
    TableEntry ttt = convert(columns_types[obj], obj_name);
    size_t print_count = 0;
    //loop vec and print names
    cout << "% ";
    if(!isQuiet){
        for (auto element : vec) cout << element << " ";
            cout << endl;
    }
    //printing rows
    switch (OP[0])
    {
        case '=':
        {   
            //if there is bst
            if (index_type[0] == 'b' && index_col_num == obj){
                auto findIt = bst.find(ttt);
                if(findIt != bst.end()){
                    //loop this map.second, which is the vector stores row location
                    for(auto element : findIt->second){
                        if(!isQuiet){
                            //loop name vector
                            for (auto ele : vec)
                                cout << table[element][columns_hash[ele]] << " ";
                            cout << endl;
                        }
                        print_count = findIt->second.size();
                    }
                }
            //if ther is a hash
            }else{
                auto comp = Equal(obj,convert(columns_types[obj],obj_name));
                for (size_t i = 0; i < table.size(); i++){
                    if(comp(table[i][obj])){
                        if(!isQuiet){
                            //loop name vector
                            for (auto ele : vec)
                                cout << table[i][columns_hash[ele]] << " ";
                            cout << endl;
                        }
                        print_count++;
                    }
                }
            }
            break;
        }

        case '>':
        {
            //if there is bst
            if (index_type[0] == 'b' && index_col_num == obj){
                //loop bst, from the first row to last
                for (auto index = bst.upper_bound(ttt); index != bst.end(); index++){
                    //loop this map.second, which is the vector stores row location
                    for(auto element : index->second){
                        if(!isQuiet){
                            //loop name vector
                            for (auto ele : vec)
                                cout << table[element][columns_hash[ele]] << " ";
                            cout << endl;
                        }
                        print_count++;
                    }
                }
            //no bst, then use hash
            }else{
                auto comp = GreaterThan(obj,convert(columns_types[obj],obj_name));
                for (size_t i = 0; i < table.size(); i++){
                    if(comp(table[i][obj])){
                        if(!isQuiet){
                            //loop name vector
                            for (auto ele : vec)
                                cout << table[i][columns_hash[ele]] << " ";
                            cout << endl;
                        }
                        print_count++;
                    }
                }
            }
            break;
        } 

        case '<':
        {
            //if there is bst
            if (index_type[0] == 'b' && index_col_num == obj){
                //loop bst, from the first row to last
                for (auto index = bst.begin(); index != bst.lower_bound(ttt); index++){
                    //loop this map.second, which is the vector stores row location
                    for(auto element : index->second){
                        if(!isQuiet){
                            //loop name vector
                            for (auto ele : vec)
                                cout << table[element][columns_hash[ele]] << " ";
                            cout << endl;
                        }
                        print_count++;
                    }
                }
            //no bst, then use hash
            }else{
                auto comp = LessThan(obj,convert(columns_types[obj],obj_name));
                for (size_t i = 0; i < table.size(); i++){
                    if(comp(table[i][obj])){
                        if(!isQuiet){
                            //loop name vector
                            for (auto ele : vec)
                                cout << table[i][columns_hash[ele]] << " ";
                            cout << endl;
                        }
                        print_count++;
                    }
                }
            }
            break;
        }
        default:
            break;
    }

    return print_count;
};

void Table::printALL(vector<string> vec){
    //print out column names
    for (auto element : vec) cout << element << " ";
    cout << endl;
    //print out columns
	for (size_t i = 0; i < table.size(); i++){
        for (auto element : vec){
            cout << table[i][columns_hash[element]] << " ";
        }
        cout << endl;
    
	}
};

class DataBase{
    public:
        unordered_map<string, Table*> tables;
        void getOptions(int argc, char** argv);
        void readCommand();
        bool isQuiet = false;
        //dtor
        ~DataBase();

    private:
        //check tableName
        Table* tableNameCheck(string t_name);
        //before this function be implemented, tableNameCheck() should be used first
        size_t colNameCheck(string col_name, string table_name);
        void addTable();
        //remove a table
        void remove();
        void insert();
        //delete the required rows, refer to the table.delete()
        void deleteItem();
        void generateIndex();
        // void updateIndex(string col_name, string table_name, string index_type);
        void join();
        void print();
};

void Table::deleteWHERE(size_t columnNumber, string method_type, string obj_name){
    
    switch (method_type[0])
    {
        case '=':
        {
            Equal comp(columnNumber, convert(columns_types[columnNumber], obj_name));
            table.erase(remove_if(table.begin(), table.end(), comp), table.end());
            break;
        }
        case '>':
        {
            GreaterThan comp(columnNumber, convert(columns_types[columnNumber], obj_name));
            table.erase(remove_if(table.begin(), table.end(), comp), table.end());
            break;
        }     
        case '<':
        {
            LessThan comp(columnNumber, convert(columns_types[columnNumber], obj_name));
            table.erase(remove_if(table.begin(), table.end(), comp), table.end());
            break;
        }
        default:
            break;
    }
};


void Table::updateIndex()
{

    //check if empty. if not, clear it
    // if(!hash.empty()) hash.clear();
    // if(!bst.empty()) bst.clear();
    hash.clear();
    bst.clear();
    //update, for bst
    if (index_type[0] == 'b') {
		//Populate BST with all values in column
		for (size_t i = 0; i < table.size(); ++i) {
			bst[table[i][index_col_num]].push_back(i);
		}
	}
    //update for hash
    else if (index_type[0] == 'h'){
        //hash could be resrved
        hash.reserve(table.size());
		//Populate BST with all values in column
		for (size_t i = 0; i < table.size(); ++i) {
			hash[table[i][index_col_num]].push_back(i);
		}
    }
	else return;
};

void DataBase::getOptions(int argc, char** argv)
{
    int option_index = 0, option = 0;
    struct option longOpts[] = {{"quiet", no_argument, nullptr, 'q'},
                                {"help", no_argument, nullptr, 'h'},
                                {nullptr, 0, nullptr, '\0'} };

    while ((option = getopt_long(argc, argv, "qh", longOpts, &option_index)) != -1)
    {
        switch (option) 
        {
            case 'q':
                isQuiet = true;
                break;

            case 'h':
                cerr << "This program reads in user input from command line and performs"
                    << "operations on a table based on the user input\n"
                    << "Usage: \'./letter\n\t[--quiet | -q]\n"
                    << "\t[--help | -h]\n";
                exit(0);

            default: break;
        }
    }
};

Table* DataBase::tableNameCheck(string t_name)
{
    auto it = tables.find(t_name);
    if(it == tables.end()){
        return nullptr;
    }
    else  return it->second;
}

size_t DataBase::colNameCheck(string col_name, string table_name)
{
    auto it = tables[table_name]->columns_hash.find(col_name);
    if(it == tables[table_name]->columns_hash.end()){
        return std::numeric_limits<size_t>::max();
    }
    else return it->second;
}

void DataBase::readCommand()
{
    string input;
    string useless;

    do{
        std::cin >> input;
        if (cin.fail()) 
        {
            cerr << "Error: Reading from cin has failed" << endl;
            exit(1);
        } // if
        char cmd = input[0];
            
        switch (cmd) {
            case 'C':
                addTable();
                break;
            case 'R':
                remove();
                break;
            case 'I':
                insert();
                break;
            case 'D': 
                deleteItem();
                break;
            case 'G':
                generateIndex();
                break;
            case 'J':
                join();
                break;
            case 'P': 
                print();
                break;
            case '#':
                cout << "% ";
                getline(cin, useless);
                break;
            case 'Q':
                std::cout << "% Thanks for being silly!" << endl;
                return;
            default:
                getline(cin, useless);
                cout << "% " << "Error: unrecognized command" << endl;
                break;
        }
    } while (true);
}

void DataBase::addTable(){
    string table_name;
    cin >> table_name;
    //name check
    if (tableNameCheck(table_name)){
        string useless;
        cout << "% " << "Error during CREATE: Cannot create already existing table " << table_name << endl;
        getline(cin, useless);
        return;
    }
    size_t col_num;
    cin >> col_num;
    //value ctor new table
    Table* new_table = new Table{table_name, col_num};
    string type;
    //loop for col_types
    for (size_t i = 0; i < col_num; i++){
        cin >> type;
        auto it = convertType(type[0]);
        new_table->columns_types.push_back(it);
    }
    //hashing the column names and report to output
    cout << "% New table "<< table_name << " with column(s) ";
    string col_name;
    for (size_t i = 0; i < col_num; i++){
        cin >> col_name;
        new_table->columns_hash[col_name] = i;
        cout << col_name << " ";
    }
    tables[new_table->t_name] = new_table;
    cout << "created" << endl;
};

void DataBase::remove(){
    string table_name;
    cin >> table_name;
    auto it = tableNameCheck(table_name);
    if (!it){
        string useless;
        cout << "% " << "Error during REMOVE: " << table_name << " does not name a table in the database" << endl;
        getline(cin, useless);
        return;
    }
    delete it;
    tables.erase(table_name);
    cout << "% Table " << table_name << " removed" << endl;
};

void DataBase::insert(){
    string table_name, useless;
    cin >> useless >> table_name;
    auto it = tableNameCheck(table_name);
    if (!it){
        cout << "% " << "Error during INSERT: " << table_name << " does not name a table in the database" << endl;
        getline(cin, useless);
        return;
    }
    // size_t col_num;
    //check if the cin >> useless got the "ROWS"
    it->insertRows();
    it->updateIndex();
};



void DataBase::deleteItem(){
    string useless, table_name, methodType, col_name, obj_name;
    cin >> useless >> table_name >> useless >> col_name >> methodType >> obj_name;
    auto it = tableNameCheck(table_name);
    if (!it){
        string useless;
        // getline(cin, useless);
        cout << "% " << "Error during DELETE: " << table_name << " does not name a table in the database" << endl;
        return;
    }
    size_t col_number = colNameCheck(col_name, table_name);
    if (col_number == std::numeric_limits<size_t>::max()){
        // getline(cin, useless);
        cout << "% " << "Error during DELETE: " << col_name << " does not name a column in " << table_name << endl;
        return;
    }
    size_t curr = it->table.size();
    it->deleteWHERE(col_number, methodType, obj_name);
    size_t deleteCount = curr - it->table.size();
    cout << "% " << "Deleted " << deleteCount << " rows from " << table_name << endl;
    //if #deleted elements > 0, update the index
    it->updateIndex();
};




void DataBase::generateIndex(){
    string useless, table_name, indexType, col_name;
    cin >> useless >> table_name >> indexType >> useless >> useless >> col_name;
    auto it = tableNameCheck(table_name);
    if (!it){
        string useless;
        // getline(cin, useless);
        cout << "% " << "Error during GENERATE: " << table_name << " does not name a table in the database" << endl;
        return;
    }
	auto col_num = colNameCheck(col_name, table_name);
    if (col_num == std::numeric_limits<size_t>::max()){
        // getline(cin, useless);
        cout << "% " << "Error during GENERATE: " << col_name << " does not name a column in " << table_name << endl;
        return;
    }
    //change the indexing paras in the table
    it->index_col_num = col_num;
    it->index_type = indexType;
    //then update indexes
    it->updateIndex();
    //print out
    cout << "% " << "Created " << indexType << " index for table " << table_name <<" on column " << col_name << ", with " << ((indexType[0] == 'b') ? tables[table_name]->bst.size() : tables[table_name]->hash.size()) << " distinct keys" << endl;
};



void DataBase::join(){
	string table1, table2, col1, col2, useless;
	cin >> table1;
	//table check
	auto it1 = tableNameCheck(table1);
    if (!it1){
        cout << "% " << "Error during JOIN: " << table1 << " does not name a table in the database" << endl;
        getline(cin, useless);
        return;
    }
	cin >> useless >> table2;
	auto it2 = tableNameCheck(table2);
    if (!it2){
        cout << "% " << "Error during JOIN: " << table2 << " does not name a table in the database" << endl;
        getline(cin, useless);
        return;
    }
	//column check
	cin >> useless >> col1;
	size_t colNum1 = colNameCheck(col1, table1);
	if (colNum1 == std::numeric_limits<size_t>::max()){
            cout << "% " << "Error during JOIN: " << col1 << " does not name a column in " << table1 << endl;
            getline(cin, useless);
            return;
    }
	cin >> useless >> col2;
	size_t colNum2 = colNameCheck(col2, table2);
	if (colNum2 == std::numeric_limits<size_t>::max()){
            cout << "% " << "Error during JOIN: " << col2 << " does not name a column in " << table2 << endl;
            getline(cin, useless);
            return;
    }

	//keep checking printing paras
	size_t num, colNumber;
	int colBelong;
	bool isTable1;
	string col_name, table_name;
	cin >> useless >> useless >> num;
	//this vector will store pair(the column number in table_name. is that in the table1)
	// vector<pair<size_t, bool >> print_cols_name;
    vector<pair<string, bool >> print_cols_name;
    // vector<string> names;
    print_cols_name.reserve(num);
    // names.reserve(num);

    for (size_t i = 0; i < num; i++){
        cin >> col_name >> colBelong;
		table_name = (colBelong == 1) ? table1 : table2;
		isTable1 = (colBelong == 1) ? true : false;
		colNumber = colNameCheck(col_name, table_name);
		//for any invalid col name detected, stop immediately
        if (colNumber == std::numeric_limits<size_t>::max()){
            cout << "% " << "Error during JOIN: " << col_name << " does not name a column in " << table_name << endl;
            getline(cin, useless);
            return;
        }
        print_cols_name.push_back(make_pair(col_name, isTable1));
        // names.push_back(col_name);
    }
    cout << "% ";
    if(!isQuiet){
        //print names
        for (auto element : print_cols_name) cout << element.first << " ";
        cout << endl;
    }

//everthing looks fine, so continue
	//start joining and printing
	int joinCount = 0;

    // cout << it1->index_col_num << "--" << it1->index_type << "--" << it2->index_col_num << "--" << it2->index_type;
	//check if bst exists in the table1
	if((it2->index_type[0] == 'b' && it2->index_col_num == colNum2)){
		//loop table1
        // cout << "bst" << endl;
		for (auto element : it1->table){
            //try to find in the bst2, locate row2
            auto obj = it2->bst.find(element[colNum1]);
            //if find something
            if (obj != it2->bst.end()){
            //decide which table should be printed, then print
                for(auto item : obj->second){
                    if(!isQuiet){
                        //loop the name vector first
                        for (auto el : print_cols_name){
                        //which table should be printed?
                            if (el.second){
                                //if belongs to the table1
                                cout << element[it1->columns_hash[el.first]]<< " ";
                            }else{
                                //if belongs to the table2
                                cout << it2->table[item][it2->columns_hash[el.first]] << " ";
                            }
                        }
                        //print out new line
                        cout << endl;
                    }
                    //should keep conting even for the quiet model
                    joinCount++;
                }
            }
        }
    //else if hash the table2
	}else if((it2->index_type[0] == 'h'&& it2->index_col_num == colNum2)){
        // cout << "hash" << endl;
		for (auto element : it1->table){
            //try to find in the bst2, locate row2
            auto obj = it2->hash.find(element[colNum1]);
            //if find something
            if (obj != it2->hash.end()){
            //decide which table should be printed, then print
                for(auto item : obj->second){
                    if(!isQuiet){
                        //loop the name vector first
                        for (auto el : print_cols_name){
                        //which table should be printed?
                            if (el.second){
                                //if belongs to the table1
                                cout << element[it1->columns_hash[el.first]]<< " ";
                            }else{
                                //if belongs to the table2
                                cout << it2->table[item][it2->columns_hash[el.first]] << " ";
                            }
                        }
                        //print out new line
                        cout << endl;
                    }
                    //should keep conting even for the quiet model
                    joinCount++;
                }
            }
        }
    //no bst and hash, generate hash
	}else{
        // cout << "hashhash" << endl;
        unordered_map<TableEntry, vector<size_t>> hashhash;
        // //hash could be resrved
        hashhash.reserve(it2->table.size());
		//Populate BST with all values in column
		for (size_t i = 0; i < it2->table.size(); ++i) {
			hashhash[it2->table[i][colNum2]].push_back(i);
		}
		for (auto element : it1->table){
            //try to find in the bst2, locate row2
            auto obj = hashhash.find(element[colNum1]);
            //if find something
            if (obj != hashhash.end()){
            //decide which table should be printed, then print
                for(auto item : obj->second){
                    if(!isQuiet){
                        //loop the name vector first
                        for (auto el : print_cols_name){
                        //which table should be printed?
                            if (el.second){
                                //if belongs to the table1
                                cout << element[it1->columns_hash[el.first]]<< " ";
                            }else{
                                //if belongs to the table2
                                cout << it2->table[item][it2->columns_hash[el.first]] << " ";
                            }
                        }
                        //print out new line
                        cout << endl;
                    }
                    //should keep conting even for the quiet model
                    joinCount++;
                }
            }
        }
	}
	cout << "Printed " << joinCount << " rows from joining " << table1 << " to " << table2 << endl;
};


void DataBase::print(){
    string useless, table_name, col_name, printType;
    size_t num;
    cin >> useless >> table_name >> num;
    // cout << useless << "----" << table_name << "---" << num;
    auto it = tableNameCheck(table_name);
    if (!it){
        cout << "% " << "Error during PRINT: " << table_name << " does not name a table in the database" << endl;
        getline(cin, useless);
        return;
    }
    //col name check, only the first wrong col_name will be printed, then stop
    vector<string> print_cols_name;
    print_cols_name.reserve(num);
    for (size_t i = 0; i < num; i++){
        cin >> col_name;
        if (colNameCheck(col_name, table_name) == std::numeric_limits<size_t>::max()){
            cout << "% " << "Error during PRINT: " << col_name << " does not name a column in " << table_name << endl;
            getline(cin, useless);
            return;
        }
        print_cols_name.push_back(col_name);
    }
    cin >> printType;
    // cout << printType << "---";
	//if printALL
    if (printType[0] == 'A'){
        cout << "% ";
        if (!isQuiet) it->printALL(print_cols_name);
        cout << "Printed " << it->table.size() << " matching rows from " << table_name << endl;
    }
	//else printWHERE
	else{
		string OP;
		cin >> col_name >> OP;//useless is the "=", since it is always "="
		size_t col_number = colNameCheck(col_name, table_name);
		if (col_number == std::numeric_limits<size_t>::max()){
            cout << "% " << "Error during PRINT: " << col_name << " does not name a column in " << table_name << endl;
            getline(cin, useless);
            return;
        }
		size_t print_count = it->printWHERE(print_cols_name, col_number, OP, isQuiet);
		cout << "Printed " << print_count << " matching rows from " << table_name << endl;
	}
};

DataBase::~DataBase(){
    for (auto i: tables) delete i.second;
};

int main(int argc, char** argv) 
{
    ios_base::sync_with_stdio(false); // you should already have this
    cin >> std::boolalpha;  // add these two lines
    std::cout << std::boolalpha; // add these two lines
    // cout << "% ";
    DataBase data;
    data.getOptions(argc, argv);
    data.readCommand();
    return 0;
}
