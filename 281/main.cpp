// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D

//  main.cpp
//  myProj03
//
//  Created by 周家西 on 3/23/20.
//  Copyright © 2020 周家西. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include "TableEntry.h"

using namespace std;

class Table{
private:
    unsigned int myCol = 0;
    char myOptr = ' ';
    // current value used for comparison
    bool myBool = false;
    int myInt = 0;
    double myDbl = 0;
    string myStr = " ";
    
public:
    unsigned int num_col = 0;
    unsigned int num_row = 0;
    string name = " ";
    vector<EntryType> col_types;
    unordered_map<string, unsigned int> col_names;
    vector<vector<TableEntry>> content;
    // generate index
    bool hash = false;
    bool bst = false;
    unsigned int indexCol = 0;
    unordered_map<TableEntry,vector<unsigned int>> myHash;
    map<TableEntry,vector<unsigned int>> myBST;
    
    
    void CreateTable();
    void InsertTable(unsigned int rows);
    void DeleteWhere(unsigned int Col, char optr);
    void Print(bool quietMode);
    void PrintWhere(unsigned int num, vector<unsigned int>& vec, bool quietMode);
    void GenIndex(string type, string col);
    
    //    Helper Function
    bool ToSelect(vector<TableEntry> & cur_row);
    unsigned int PrintIndex(TableEntry value,unsigned int num, vector<unsigned int>& vec, bool quietMode);
}; // Table Declaration

class DataBase{
private:
    bool quietMode = false;
    unordered_map<string, Table*> tables;
public:
    int GetFlag(int argc, char *argv[]);
    void ReadInput();
    void Join(string tb1, string tb2);
};

void Table::CreateTable(){
    cin >> num_col;
    col_types.resize(num_col);
    cout << "New table " << name << " with column(s) ";
    
    string cmd;
    for (unsigned int i = 0; i < num_col; ++i) {
        cin >> cmd;
        if (cmd == "string") {
            col_types[i] = EntryType::String;
        } else if (cmd == "double") {
            col_types[i] = EntryType::Double;
        } else if (cmd == "int") {
            col_types[i] = EntryType::Int;
        } else if (cmd == "bool") {
            col_types[i] = EntryType::Bool;
        } // end if-else
    } // end for - assign column_types
    for (unsigned int i = 0; i < num_col; ++i) {
        cin >> cmd;
        col_names[cmd] = i;
        cout << cmd << " ";
    } // end for - assign column_names
    cout << "created\n";
    
} // CreateTable

void Table::InsertTable(unsigned int rows){
    vector<TableEntry> cur_row;
    cur_row.reserve(num_col);
    content.reserve(num_row+rows);
    for (unsigned int j = num_row; j < num_row+rows; ++j) {
        // insert items into the current row
        for (unsigned int i = 0; i < num_col; ++i) {
            switch (col_types[i]) {
                case EntryType::String:{
                    string Str;
                    cin >> Str;
                    cur_row.emplace_back(Str);
                    break;
                }
                case EntryType::Double:{
                    double dou;
                    cin >> dou;
                    cur_row.emplace_back(dou);
                    break;
                }
                case EntryType::Int:{
                    int in;
                    cin >> in;
                    cur_row.emplace_back(in);
                    break;
                }
                case EntryType::Bool:{
                    bool B;
                    cin >> B;
                    cur_row.emplace_back(B);
                    break;
                }
                default:
                    // impossible
                    break;
            } // end switch
        } // end for
        content.push_back(cur_row);
        cur_row.clear();
        cur_row.reserve(num_col);
    } // end for
    // check whether there is index generated
    if (hash) {
        for (unsigned int i = num_row; i < num_row + rows; ++i) {
            myHash[content[i][indexCol]].push_back(i);
        } // end for
    } // if hash
    if (bst) {
        for (unsigned int i = num_row; i < num_row + rows; ++i) {
            myBST[content[i][indexCol]].push_back(i);
        } // end for
    } // if bst
    cout << "Added " << rows << " rows to " << name << " from position " << num_row << " to " << num_row+rows-1 << "\n";
    num_row += rows;
    
} // InsertTable

void Table::DeleteWhere(unsigned int Col, char optr){
    // read in the value
    myCol = Col;
    myOptr = optr;
    switch (col_types[Col]) {
        case EntryType::String:{
            cin >> myStr;
            break;
        }
        case EntryType::Double:{
            cin >> myDbl;
            break;
        }
        case EntryType::Int:{
            cin >> myInt;
            break;
        }
        case EntryType::Bool:{
            cin >> myBool;
            break;
        }
        default:
            
            break;
    } // end switch
    // remove if cur_row satisfies ToSelect
    auto it = remove_if(content.begin(), content.end(), [this](vector<TableEntry>& cur_row){
        return this->ToSelect(cur_row);
    });
    content.resize(it-content.begin());
    if (hash) {
        myHash.clear();
        for (unsigned int i = 0; i < (unsigned int) content.size(); ++i) {
            myHash[content[i][indexCol]].push_back(i);
        } // end for
    } else if (bst) {
        myBST.clear();
        for (unsigned int i = 0; i < (unsigned int) content.size(); ++i) {
            myBST[content[i][indexCol]].push_back(i);
        } // end for
    } // if
    cout << "Deleted " << num_row - content.size() << " rows from " << name << "\n";
    num_row = (unsigned int) content.size();
} // DeleteWhere

void Table::Print(bool quietMode){
    string cmd;
    unsigned int num;
    cin >> num; // N - number of columns to print
    vector<string> headers;
    headers.resize(num);
    vector<unsigned int> idx;
    idx.resize(num);
    
    for (unsigned int i = 0; i < num; ++i) {
        cin >> cmd;
        if (col_names.find(cmd) == col_names.end()) {
            cout << "Error: " << cmd <<" does not name a column in " << name << "\n";
            getline(cin,cmd);
            return;
        } else {
            idx[i] = col_names.at(cmd);
            headers[i] = cmd;
        }
    } // end for
    cin >> cmd; // where/all
    if (cmd == "WHERE") { // where
        cin >> cmd; // column name
        if (col_names.find(cmd) == col_names.end()) {
            cout << "Error: " << cmd <<" does not name a column in " << name << "\n";
            getline(cin,cmd);
            return;
        } else {
            cin >> myOptr;
            myCol = col_names.at(cmd);
            // print column names
            if (!quietMode) {
                for (unsigned int i= 0; i < num; ++i) {
                    cout << headers[i] << " ";
                }
                cout << "\n";
            } // if
            PrintWhere(num, idx, quietMode);
        } // if-else
    } else { // all
        if (!quietMode) {
            for (unsigned int i= 0; i < num; ++i) {
                cout << headers[i] << " ";
            }
            cout << "\n";
            for (unsigned int i = 0; i < num_row; ++i) {
                for (unsigned int j = 0; j < num; ++j) {
                    cout << content[i][idx[j]] << " ";
                } // end inner for
                cout << "\n";
            } // end for
        } // end if
        cout << "Printed " << num_row << " matching rows from " << name << "\n";
    } // end if-else
} // Print

void Table::PrintWhere(unsigned int num, vector<unsigned int>& vec, bool quietMode){
    unsigned int num_printed = 0;
    // read in the value
    switch (col_types[myCol]) {
        case EntryType::String:{
            cin >> myStr;
            TableEntry value{myStr};
            num_printed = PrintIndex(value, num, vec, quietMode);
            break;
        }
        case EntryType::Double:{
            cin >> myDbl;
            TableEntry value{myDbl};
            num_printed = PrintIndex(value, num, vec, quietMode);
            break;
        }
        case EntryType::Int:{
            cin >> myInt;
            TableEntry value{myInt};
            num_printed = PrintIndex(value, num, vec, quietMode);
            break;
        }
        case EntryType::Bool:{
            cin >> myBool;
            TableEntry value{myBool};
            num_printed = PrintIndex(value, num, vec, quietMode);
            break;
        }
        default:
            break;
    } // end switch
    // when no index
    if (!(hash && myCol == indexCol && myOptr == '=') && !(bst && myCol == indexCol)) {
        // print where the condition is satisfied
        for (unsigned int i = 0; i < num_row; ++i) {
            if (ToSelect(content[i])) {
                if (!quietMode) {
                    for (unsigned int j = 0; j < num; ++j) {
                        cout << content[i][vec[j]] << " ";
                    } // end inner for
                    cout << "\n";
                } // end if
                ++num_printed;
            } // end if
        } // end for
    } // if no index
    
    cout << "Printed " << num_printed << " matching rows from " << name << "\n";
} // PrintWhere

unsigned int Table::PrintIndex(TableEntry value, unsigned int num, vector<unsigned int>& vec, bool quietMode){
    unsigned int num_printed = 0;
    if (hash && myCol == indexCol && myOptr == '=') {
        for (auto it = myHash.begin(); it!=myHash.end(); ++it) {
            if (it->first == value) { // find the key (value)
                num_printed = (unsigned int) it->second.size();
                if (!quietMode) {
                    for (unsigned int i = 0; i < num_printed; ++i) {
                        for (unsigned int j = 0; j < num; ++j) {
                            cout << content[it->second[i]][vec[j]] << " ";
                        }
                        cout << "\n";
                    } // for*2
                } // end if
                break;
            }
        } // for loop through my Hash table
    } else if (bst && myCol == indexCol) {
        switch (myOptr) {
            case '=':{
                for (auto it = myBST.begin(); it!=myBST.end(); ++it) {
                    if (it->first == value) { // find the key (value)
                        num_printed = (unsigned int) it->second.size();
                        if (!quietMode) {
                            for (unsigned int i = 0; i < num_printed; ++i) {
                                for (unsigned int j = 0; j < num; ++j) {
                                    cout << content[it->second[i]][vec[j]] << " ";
                                }
                                cout << "\n";
                            } // for*2
                        } // end if
                        break;
                    }
                } // for loop through my BST table
                break;
            }
            case '<':{
                auto it = myBST.begin();
                while (it != myBST.end() && it->first < value) {
                    num_printed += (unsigned int) it->second.size();
                    if (!quietMode) {
                        for (unsigned int i = 0; i < (unsigned int) it->second.size(); ++i) {
                            for (unsigned int j = 0; j < num; ++j) {
                                cout << content[it->second[i]][vec[j]] << " ";
                            }
                            cout << "\n";
                        } // for*2
                    } // end if
                    ++it;
                } // while
                break;
            }
            case '>':{
                bool found = false;
                auto rec = myBST.begin();
                for (auto it = myBST.begin(); it!=myBST.end(); ++it) {
                    if (it->first > value) {
                        rec = it;
                        found = true;
                        break;
                    } // if
                } // for
                if (found) {
                    for (auto it = rec; it !=myBST.end(); ++it) {
                        num_printed += (unsigned int) it->second.size();
                        if (!quietMode) {
                            for (unsigned int i = 0; i < (unsigned int) it->second.size(); ++i) {
                                for (unsigned int j = 0; j < num; ++j) {
                                    cout << content[it->second[i]][vec[j]] << " ";
                                }
                                cout << "\n";
                            } // for*2
                        } // end if
                    } // for
                } // if there is an element > value
                
                break;
            }
            default:
                break;
        } //  end switch
    } // if-else
    return num_printed;
} // Print with index generated

void Table::GenIndex(string type, string col){
    if (col_names.find(col) == col_names.end()) {
        cout << "Error: " << col <<" does not name a column in " << name << "\n";
        getline(cin,type);
        return;
    } else {
        // clear old index table
        hash = false;
        bst = false;
        myHash.clear();
        myBST.clear();
        // new index
        indexCol = col_names.at(col);
        if (type == "hash") {
            hash = true;
            for (unsigned int i = 0; i < num_row; ++i) {
                myHash[content[i][indexCol]].push_back(i);
            } // end for
        } else { // bst
            bst = true;
            for (unsigned int i = 0; i < num_row; ++i) {
                myBST[content[i][indexCol]].push_back(i);
            } // end for
        }// end if-else
        cout << "Created " << type << " index for table " << name << " on column " << col << "\n";
    } // if-else
} // GenIndex

void DataBase::Join(string tb1, string tb2){
    string col1;
    string col2;
    string cmd;
    unsigned int tb = 0;
    unsigned int N = 0;
    cin >> col1 >> cmd >> col2 >> cmd >> cmd >> N;
    auto & _tb1=tables.at(tb1);
    auto & _tb2=tables.at(tb2);
    if (_tb1->col_names.find(col1) == _tb1->col_names.end()) {
        cout << "Error: " << col1 <<" does not name a column in " << tb1 << "\n";
        getline(cin,cmd);
        return;
    } else if (_tb2->col_names.find(col2) == _tb2->col_names.end()){
        cout << "Error: " << col2 <<" does not name a column in " << tb2 << "\n";
        getline(cin,cmd);
        return;
    } else {
        unsigned int idxCol1 = _tb1->col_names.at(col1);
        unsigned int idxCol2 = _tb2->col_names.at(col2);
        // print column name
        vector<unsigned int> idxTable;
        vector<unsigned int> idxColumn;
        idxTable.resize(N);
        idxColumn.resize(N);
        for (unsigned int i = 0; i < N; ++i) {
            cin >> cmd >> tb;
            if (tb == 1 && _tb1->col_names.find(cmd) == _tb1->col_names.end()) {
                cout << "Error: " << cmd <<" does not name a column in " << tb1 << "\n";
                getline(cin,cmd);
                return;
            } else if (tb == 2 && _tb2->col_names.find(cmd) == _tb2->col_names.end()) {
                cout << "Error: " << cmd <<" does not name a column in " << tb2 << "\n";
                getline(cin,cmd);
                return;
            } else {
                idxTable[i] = tb;
                if (!quietMode) {
                    cout << cmd << " ";
                }
                if (tb == 1) {
                    idxColumn[i] = _tb1->col_names.at(cmd);
                } else { // tb2
                    idxColumn[i] = _tb2->col_names.at(cmd);
                } // inner if-else
            } // end if-else
        } // for
        if (!quietMode) {
            cout << "\n";
        }
        // iterate through tb1
        unsigned int num_printed = 0;
        
        for (unsigned int i = 0; i < _tb1->num_row; ++i) {
            
            if (_tb2->hash && _tb2->indexCol == idxCol2) { // table 2 has a hash table
                auto it = _tb2->myHash.find(_tb1->content[i][idxCol1]);
                if (it != _tb2->myHash.end()) {
                    for (unsigned int j = 0; j < (unsigned int)it->second.size(); ++j) {
                        // print matched rows
                        if (!quietMode) {
                            for (unsigned int k = 0; k < N; ++k) {
                                if (idxTable[k] == 1) {
                                    cout << _tb1->content[i][idxColumn[k]] << " ";
                                } else { // tb2
                                    cout << _tb2->content[it->second[j]][idxColumn[k]] << " ";
                                } // if-else
                            } // print for loop
                            cout << "\n";
                        } // if not quietMode
                        ++num_printed;
                    }
                }
            } else if (_tb2->bst && _tb2->indexCol == idxCol2) {
                auto it = _tb2->myBST.find(_tb1->content[i][idxCol1]);
                if (it != _tb2->myBST.end()) {
                    for (unsigned int j = 0; j < (unsigned int)it->second.size(); ++j) {
                        // print matched rows
                        if (!quietMode) {
                            for (unsigned int k = 0; k < N; ++k) {
                                if (idxTable[k] == 1) {
                                    cout << _tb1->content[i][idxColumn[k]] << " ";
                                } else { // tb2
                                    cout << _tb2->content[it->second[j]][idxColumn[k]] << " ";
                                } // if-else
                            } // print for loop
                            cout << "\n";
                        } // if not quietMode
                        ++num_printed;
                    } // for
                } // if
            } else {
                for (unsigned int j = 0; j < _tb2->num_row; ++j) {
                    // matching rows
                    if (_tb1->content[i][idxCol1] == _tb2->content[j][idxCol2]) {
                        // print matched rows
                        if (!quietMode) {
                            for (unsigned int k = 0; k < N; ++k) {
                                if (idxTable[k] == 1) {
                                    cout << _tb1->content[i][idxColumn[k]] << " ";
                                } else { // tb2
                                    cout << _tb2->content[j][idxColumn[k]] << " ";
                                } // if-else
                            } // print for loop
                            cout << "\n";
                        } // if not quietMode
                        ++num_printed;
                    } // end if
                } // inner for
            } // if-else
            
        } // outer for
        
        cout << "Printed " << num_printed << " rows from joining " << tb1 << " to " << tb2 << "\n";
    } // if-else
    
} // Join

void DataBase::ReadInput(){
    string cmd = " ";
    string temp = " ";
    char optr = ' ';
    unsigned int integer;
    cout << "% ";
    cin >> cmd;
    while (cmd != "QUIT") {
        switch (cmd.front()) {
            case 'C': // create
                cin >> cmd;
                if (tables.find(cmd) != tables.end()) {
                    cout << "Error: Cannot create already existing table "<< cmd << "\n";
                    getline(cin,cmd);
                } else {
                    Table* cur_table = new Table;
                    cur_table->name = cmd;
                    cur_table->CreateTable();
                    tables[cmd] = cur_table;
                } // end if-else
                break;
            case 'I': // insert
                cin >> cmd; // INTO
                cin >> cmd; // table name
                if (tables.find(cmd) == tables.end()) {
                    cout << "Error: "<< cmd <<" does not name a table in the database\n";
                    cin >> integer; // N
                    getline(cin,cmd);
                    for (unsigned int i = 0; i < integer; ++i) {
                        getline(cin,cmd);
                    } // for
                } else {
                    cin >> integer;
                    cin >> temp; // Rows
                    tables.at(cmd)->InsertTable(integer);
                    
                } // end if-else
                break;
            case 'D': // delete
                cin >> cmd; // FROM
                cin >> cmd; // table name
                if (tables.find(cmd) == tables.end()) {
                    cout << "Error: "<< cmd <<" does not name a table in the database\n";
                    getline(cin,cmd);
                } else {
                    string Col;
                    cin >> Col >>Col; // column name
                    if (tables.at(cmd)->col_names.find(Col) == tables.at(cmd)->col_names.end()) {
                        cout << "Error: "<< Col <<" does not name a column in " << cmd << "\n";
                        getline(cin,cmd);
                    }else{
                        cin >> optr; // operator
                        tables.at(cmd)->DeleteWhere(tables.at(cmd)->col_names.at(Col),optr);
                    } // end if-else
                } // end if-else
                break;
            case 'G': // generate index
                cin >> cmd; // FOR
                cin >> cmd; // table name
                if (tables.find(cmd) == tables.end()) {
                    cout << "Error: "<< cmd <<" does not name a table in the database\n";
                    getline(cin,cmd);
                } else {
                    string temp_col;
                    string temp_type;
                    cin >> temp_type >> temp_col >> temp_col >> temp_col; // index type & column name
                    tables.at(cmd)->GenIndex(temp_type, temp_col);
                } // if-else
                break;
            case 'P': // print
                cin >> cmd; // FROM
                cin >> cmd; // table name
                if (tables.find(cmd) == tables.end()) {
                    cout << "Error: "<< cmd <<" does not name a table in the database\n";
                    getline(cin,cmd);
                } else {
                    tables.at(cmd)->Print(quietMode);
                } // end if-else
                break;
            case 'J': {// join
                string tb1;
                string tb2;
                cin >> tb1 >> cmd >> tb2 >> cmd;
                if (tables.find(tb1) == tables.end()) {
                    cout << "Error: "<< tb1 <<" does not name a table in the database\n";
                    getline(cin,cmd);
                } else if (tables.find(tb2) == tables.end()){
                    cout << "Error: "<< tb2 <<" does not name a table in the database\n";
                    getline(cin,cmd);
                } else {
                    Join(tb1, tb2);
                } // if-else
                break;
            }
            case 'R': // remove
                cin >> cmd; // table name
                if (tables.find(cmd) == tables.end()) {
                    cout << "Error: "<< cmd <<" does not name a table in the database\n";
                    getline(cin,cmd);
                } else {
                    auto it = tables.find(cmd);
                    delete it->second;
                    tables.erase(cmd);
                    cout << "Table " << cmd << " deleted\n";
                } // if-else
                break;
            case '#': // ignore comments
                getline(cin, cmd);
                break;
            default:
                cout << "Error: Unrecognized command\n";
                break;
        } // end switch
        cout << "% ";
        cin >> cmd;
    } // While Loop
    
    for (auto it = tables.begin(); it!= tables.end(); ++it) {
        delete it->second;
    }
    cout << "Thanks for being silly!\n";
    
} // ReadInput

int DataBase::GetFlag(int argc, char **argv){
    int choice;
    int option_index = 0;
    option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"quiet", no_argument, nullptr, 'q'},
        {nullptr, 0, nullptr, '\0'}};
    while ((choice = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1)
    {
        switch (choice)
        {
            case 'h':
                cout << "Help ---\n";
                exit(0);
                break;
            case 'q':
                quietMode = true;
                break;
            default:
                cerr << "Error: invalid option" << endl;
                exit(1);
        } // switch
    } // while
    return 0;
} // GetFlag

bool Table::ToSelect(vector<TableEntry> & cur_row){
    switch (col_types[myCol]) {
        case EntryType::String:{
            switch (myOptr) {
                case '<':
                    return (cur_row[myCol] < myStr);
                    break;
                case '=':
                    return (cur_row[myCol] == myStr);
                    break;
                case '>':
                    return (cur_row[myCol] > myStr);
                    break;
                default:
                    cout << "Error: wrong operator!\n";
                    return false;
                    break;
            } // switch operator
            break;
        }
        case EntryType::Double:{
            switch (myOptr) {
                case '<':
                    return (cur_row[myCol] < myDbl);
                    break;
                case '=':
                    return (cur_row[myCol] == myDbl);
                    break;
                case '>':
                    return (cur_row[myCol] > myDbl);
                    break;
                default:
                    cout << "Error: wrong operator!\n";
                    return false;
                    break;
            } // switch operator
            break;
        }
        case EntryType::Int:{
            switch (myOptr) {
                case '<':
                    return (cur_row[myCol] < myInt);
                    break;
                case '=':
                    return (cur_row[myCol] == myInt);
                    break;
                case '>':
                    return (cur_row[myCol] > myInt);
                    break;
                default:
                    cout << "Error: wrong operator!\n";
                    return false;
                    break;
            } // switch operator
            break;
        }
        case EntryType::Bool:{
            switch (myOptr) {
                case '<':
                    return (cur_row[myCol] < myBool);
                    break;
                case '=':
                    return (cur_row[myCol] == myBool);
                    break;
                case '>':
                    return (cur_row[myCol] > myBool);
                    break;
                default:
                    cout << "Error: wrong operator!\n";
                    return false;
                    break;
            } // switch operator
            break;
        }
        default:
            cout << "Error: wrong column type!\n";
            return false;
            break;
    }
} // ToSelect


int main(int argc, char ** argv) {
    ios_base::sync_with_stdio(false);
    cin >> boolalpha;
    cout << boolalpha;

    DataBase myDataBase;
    myDataBase.GetFlag(argc, argv);
    myDataBase.ReadInput();
    
    return 0;
}
