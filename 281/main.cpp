//
//  main.cpp
//  EECS281_P3
//
//  Created by 曾宏 on 2018/11/11.
//  Copyright © 2018年 zfh. All rights reserved.
//

#include <iostream>
#include <getopt.h>
#include <vector>
#include <map>
#include <unordered_map>
#include "TableEntry.h"
using std::cin; // put under the other usings
using std::cout; // put under the other usings
using std::boolalpha; // put under the other usings
using namespace std;
struct joinPrint{
    size_t colIndex = 0;
    size_t tableindex = 0;
};
class Table{
public:
    size_t colNum = 0;
    string tableName;
    vector<EntryType> colType;
    vector<string> colName;
    vector<bool> hashIndex;
    vector<bool> bstIndex;
    map<TableEntry,vector<size_t>> myMap;
    unordered_map<TableEntry, vector<size_t>> myHash;
    vector<vector<TableEntry>> data;
};
class SillyQL{
    bool quiteMode = false;
    bool quit = false;
    vector<Table> datas;
public:
    void getopt(int argc, char * argv[]);
    void getCommands();
};
int main(int argc, char * argv[]) {
//#ifdef __APPLE__
//    string inputfile = "/Users/Derrick/Desktop/EECS281_P3/EECS281_P3/test-3-table-commands.txt";
//    if (!freopen(inputfile.c_str(), "r", stdin)){
//        cout<<"Cao"<<"\n";
//
//        string outputfile = "/Users/Derrick/Downloads/Samples-Mac-Unix (2)/no-tnt-large-R-in.txt";
//        if (!freopen(inputfile.c_str(), "w", stdout)){
//            cout<<"Cao"<<"\n";
//        }
//    }
//
//#endif
    ios_base::sync_with_stdio(false);
    cin >> boolalpha; // add these two lines
    cout << boolalpha;
    SillyQL mySillyQL;
    mySillyQL.getopt(argc, argv);
    mySillyQL.getCommands();
    return 0;
}

void SillyQL::getopt(int argc, char * argv[]){
    opterr = true; // Give us help with errors
    int choice;
    int option_index = 0;
    option long_options[] = {
        {"quite", no_argument, nullptr, 'q'},
        {"help", no_argument,nullptr,'h'},
        { nullptr, 0,                 nullptr, '\0' }
    };
    while ((choice = getopt_long(argc, argv, "qh", long_options, &option_index)) != -1) {
        switch (choice) {
            case 'q':
                quiteMode = true;
                break;
            case 'h' :
                cout<<"I do not want to help you\n";
                exit (0);
            default:
                cerr << "Error: invalid option" << endl;
                exit(1);
        } // switch
    } // while
}
void SillyQL::getCommands(){
    while(!quit){
        cout << "% ";
        string Command = "Def";
        cin >> Command;
        //QUIT
        if(Command == "QUIT"){
            //I never use points, so goodbye memory leak!!!
            cout << "Thanks for being silly!\n";
            quit = true;
        }
        //CEREATE
        else if(Command == "CREATE"){
            bool tableNameError = false;
            size_t numOfCol = 0;
            Table newTable;
            cin >> newTable.tableName >> numOfCol;
            for (size_t j = 0; j < datas.size() ; ++ j){
                if(newTable.tableName == datas[j].tableName){
                    tableNameError = true;
                    cout << "Error: Cannot create already existing table "<< newTable.tableName<<"\n";
                    break;
                }
            }
            if(tableNameError) {
                string rubbish;
                getline(cin,rubbish);
                continue;
            }
            string buff;
            for (size_t i = 0; i < numOfCol; ++i){
                cin>>buff;
                if(buff == "string"){
                    newTable.colType.push_back(EntryType::String);
                }
                else if (buff == "bool"){
                    newTable.colType.push_back(EntryType::Bool);
                }
                else if (buff == "double"){
                    newTable.colType.push_back(EntryType::Double);
                }
                else if (buff == "int"){
                    newTable.colType.push_back(EntryType::Int);
                }
                else {
                    //Some error checking stuff here.
                }
            }
            cout << "New table "<<newTable.tableName<< " with column(s) ";
            for (size_t i = 0; i < numOfCol; ++i){
                cin>>buff;
                cout<<buff<<" ";
                newTable.colName.push_back(buff);
                newTable.bstIndex.push_back(false);
                newTable.hashIndex.push_back(false);
            }
            cout << "created\n";
            newTable.colNum = numOfCol;
            datas.push_back(newTable);
        }
        //INSERT
        else if(Command == "INSERT"){
            bool inserted = false;
            string buff;
            string tableName = "def";
            cin >> buff;
            cin >> tableName;
            for(size_t i = 0; i < datas.size(); ++i){
                if(datas[i].tableName == tableName){
                    size_t originalSize = datas[i].data.size();
                    inserted = true;
                    size_t rowNum = 0;
                    cin >> rowNum >> buff;
                    for(size_t j = 0; j < rowNum; ++j){
                        vector<TableEntry> thisRow;
                        for(size_t k =0; k < datas[i].colNum; ++k ){
                            bool Bool;
                            int Int;
                            string String;
                            double Double;
                            switch(datas[i].colType[k]){
                                case EntryType::String:
                                    cin >> String;
                                    thisRow.emplace_back(String);
                                    break;
                                case EntryType::Int:
                                    cin >> Int;
                                    thisRow.emplace_back(Int);
                                    break;
                                case EntryType::Double:
                                    cin >> Double;
                                    thisRow.emplace_back(Double);
                                    break;
                                case EntryType::Bool:
                                    cin >> Bool;
                                    thisRow.emplace_back(Bool);
                            }
                        }
                        datas[i].data.push_back(thisRow);
                        for(size_t j = 0 ; j < datas[i].colNum; ++j){
                            if(datas[i].bstIndex[j]){
                                datas[i].myMap[thisRow[j]].push_back(datas[i].data.size()-1);
                            }
                            if(datas[i].hashIndex[j]){
                                datas[i].myHash[thisRow[j]].push_back(datas[i].data.size()-1);
                            }
                        }
                        
                    }
                    cout << "Added "<<rowNum << " rows to "<<tableName<<" from position "<<originalSize<< " to "<< originalSize+rowNum-1<<"\n";
                }
            }
            if(!inserted){
                int rows = 0;
                cin >> rows;
                rows+=1;
                string rubbish;
                for(int i = 0; i < rows ; ++i){
                    getline(cin,rubbish);
                }
                cout << "Error: "<<  tableName<< " does not name a table in the database\n";
                // exit(1);
            }
        }
        //PRINT
        else if(Command == "PRINT"){
            char operation = '0';
            bool tbNameError = true;
            size_t colNum = 0;
            size_t printedRows = 0;
            size_t checkCol = 0;
            string buff;
            string tableName = "def";
            vector<size_t> indexs;
            cin >> buff;
            cin >> tableName;
            bool clNameError = true;
            for(size_t i = 0; i < datas.size(); ++i){
                if(datas[i].tableName == tableName){
                    tbNameError = false;
                    cin >> colNum;
                    for(size_t j = 0; j< colNum; ++j){
                        clNameError = true;
                        cin >> buff;
                        for(size_t k = 0; k < datas[i].colName.size(); ++k){
                            if(buff == datas[i].colName[k]){
                                indexs.push_back(k);
                                clNameError = false;
                                break;
                            }
                        }
                        if(clNameError){
                            cout<<"Error: "<<buff<< " does not name a column in "<<tableName<<"\n";
                            // exit(1);
                        }
                    }
                    if(clNameError){
                        //  cout<<"No such column\n";
                        // exit(1);
                        string rubbish;
                        getline(cin,rubbish);
                        break;
                    }
                    
                    cin >> buff;
                    if(buff != "WHERE"){
                        if(!quiteMode){
                            for (size_t n = 0; n < indexs.size(); ++n){
                                cout << datas[i].colName[indexs[n]] << " ";
                            }
                            cout << "\n";
                        }
                        for (size_t m = 0; m < datas[i].data.size(); ++m){
                            if(!quiteMode){
                                for (size_t n = 0; n < indexs.size(); ++n){
                                    cout << datas[i].data[m][indexs[n]] << " ";
                                }
                                cout << "\n";
                            }
                            ++printedRows;
                        }
                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                    }
                    else {
                        cin>>buff;
                        bool checkColBstIndex = false;
                        bool checkColNameError = true;
                        for(size_t k = 0; k < datas[i].colName.size(); ++k){
                            if(buff == datas[i].colName[k]){
                                checkCol = k;
                                checkColNameError = false;
                            }
                        }
                        if(checkColNameError){
                            //cout<<"No such column to check\n";
                            //exit(1);
                            string rubbish;
                            getline(cin,rubbish);
                            cout<<"Error: "<< buff <<" does not name a column in "<<tableName <<"\n";
                            break;
                        }
                        if(datas[i].bstIndex[checkCol]){
                            checkColBstIndex = true;
                        }
                        if(!quiteMode){
                            for (size_t n = 0; n < indexs.size(); ++n){
                                cout << datas[i].colName[indexs[n]] <<" ";
                            }
                            cout << "\n";
                        }
                        cin >> operation;
                        bool Bool;
                        string String;
                        int Int;
                        double Double;
                        switch(operation){
                            case '=':
                                switch(datas[i].colType[checkCol]){
                                    case EntryType::Bool:
                                        cin >> Bool;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first == Bool){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] == Bool){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                    case EntryType::Double:
                                        cin >> Double;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first == Double){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] == Double){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                    case EntryType::Int:
                                        cin >> Int;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first == Int){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] == Int){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                        
                                    case EntryType::String:
                                        cin >> String;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first == String){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] == String){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                }
                                break;
                            case '<':
                                switch(datas[i].colType[checkCol]){
                                    case EntryType::Bool:
                                        cin >> Bool;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first < Bool){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] < Bool){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                    case EntryType::Double:
                                        cin >> Double;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first < Double){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] < Double){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                    case EntryType::Int:
                                        cin >> Int;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first < Int){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] < Int){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                        
                                    case EntryType::String:
                                        cin >> String;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first < String){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] < String){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                }
                                break;
                            case '>':
                                switch(datas[i].colType[checkCol]){
                                    case EntryType::Bool:
                                        cin >> Bool;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first > Bool){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] > Bool){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                    case EntryType::Double:
                                        cin >> Double;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first > Double){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] > Double){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                    case EntryType::Int:
                                        cin >> Int;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first > Int){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] > Int){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                        
                                    case EntryType::String:
                                        cin >> String;
                                        if(checkColBstIndex){
                                            for(auto it = datas[i].myMap.begin(); it != datas[i].myMap.end(); ++it){
                                                if(it->first > String){
                                                    if(!quiteMode){
                                                        for(auto it1 = it->second.begin(); it1 != it->second.end(); ++it1){
                                                            for (size_t n = 0; n < indexs.size(); ++n){
                                                                cout << datas[i].data[*it1][indexs[n]] << " ";
                                                            }
                                                            cout << "\n";
                                                        }
                                                    }
                                                    printedRows += it->second.size();
                                                }
                                            }
                                        }
                                        else{
                                            for (size_t m = 0; m < datas[i].data.size(); ++m){
                                                if(datas[i].data[m][checkCol] > String){
                                                    if(!quiteMode){
                                                        for (size_t n = 0; n < indexs.size(); ++n){
                                                            cout << datas[i].data[m][indexs[n]] << " ";
                                                        }
                                                        cout << "\n";
                                                    }
                                                    ++printedRows;
                                                }
                                            }
                                        }
                                        cout << "Printed " << printedRows << " matching rows from " << tableName<< "\n";
                                        break;
                                }
                                break;
                        }
                    }
                }
            }
            if(tbNameError) {
                //  cout << "No such table.\n";
                // exit(1);
                string rubbish;
                getline(cin,rubbish);
                cout << "Error: "<<  tableName<< " does not name a table in the database\n";
            }
        }
        //REMOVE
        else if(Command == "REMOVE"){
            bool removed = false;
            string tableName = "def";
            cin >> tableName;
            for(auto iter = datas.begin(); iter != datas.end(); ++iter){
                if(iter->tableName == tableName){
                    datas.erase(iter);
                    removed = true;
                    cout << "Table "<< tableName << " deleted\n";
                    break;
                }
            }
            if(!removed){
                //  cout << "Not the name of a table in the database!!\n";
                //   exit(1);
                string rubbish;
                getline(cin,rubbish);
                cout << "Error: "<<  tableName<< " does not name a table in the database\n";
            }
        }
        //DELETE
        else if(Command == "DELETE"){
            bool tbNameError = true;
            string buff;
            cin >> buff;
            string tableName = "def";
            cin >> tableName;
            cin >> buff;
            string colName = "def";
            cin >> colName;
            char operation = '0';
            cin >> operation;
            bool Bool;
            int Int;
            string String;
            double Double;
            size_t checkCol = 0;
            for (size_t i = 0; i < datas.size(); ++i){
                if(datas[i].tableName == tableName){
                    tbNameError = false;
                    bool colNameError = true;
                    for(size_t j = 0; j < datas[i].colName.size(); ++j){
                        if(datas[i].colName[j] == colName){
                            colNameError = false;
                            checkCol = j;
                        }
                    }
                    if(colNameError){
                        cout<<"Error: "<< colName <<" does not name a column in "<<tableName <<"\n";
                        string rubbish;
                        getline(cin,rubbish);
                        break;
                    }
                    bool deleted = false;
                    switch (datas[i].colType[checkCol]){
                        case EntryType::Double:
                            cin >> Double;
                            switch(operation){
                                case '=':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] == Double){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] == Double){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                case '>':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] > Double){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] > Double){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                    
                                case '<':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] < Double){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] < Double){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                            }
                            break;
                            
                        case EntryType::Int:
                            cin >> Int;
                            switch(operation){
                                case '=':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] == Int){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] == Int){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                case '>':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] > Int){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] > Int){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                    
                                case '<':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] < Int){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] < Int){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                            }
                            break;
                        case EntryType::String:
                            cin >> String;
                            switch(operation){
                                case '=':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] == String){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] == String){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                case '>':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] > String){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] > String){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                    
                                case '<':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] < String){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] < String){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                            }
                            break;
                        case EntryType::Bool:
                            cin >> Bool;
                            switch(operation){
                                case '=':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] == Bool){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] == Bool){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                case '>':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] > Bool){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] > Bool){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                                    
                                case '<':
                                    deleted = false;
                                    for(size_t k = 0; k < datas[i].data.size(); ++k){
                                        if(datas[i].data[k][checkCol] < Bool){
                                            deleted = true;
                                            size_t index = k + 1;
                                            while(index < datas[i].data.size() && datas[i].data[index][checkCol] < Bool){
                                                ++index;
                                            }
                                            if(index == datas[i].data.size()){
                                                cout << "Deleted "<< datas[i].data.size() - k<< " rows from "<<tableName<<"\n";
                                                datas[i].data.resize(k);
                                                break;
                                            }
                                            else{
                                                swap(datas[i].data[index], datas[i].data[k]);
                                            }
                                        }
                                    }
                                    if(!deleted){
                                        cout << "Deleted "<<"0 rows from "<<tableName<<"\n";
                                    }
                                    break;
                            }
                            break;
                    }
                    for(size_t m = 0; m < datas[i].colNum; ++m){
                        if(datas[i].hashIndex[m]){
                            datas[i].myHash.clear();
                            for(size_t n = 0 ; n<datas[i].data.size(); ++n){
                                datas[i].myHash[datas[i].data[n][m]].push_back(n);
                            }
                        }
                        if(datas[i].bstIndex[m]){
                            datas[i].myMap.clear();
                            for(size_t n = 0 ; n<datas[i].data.size(); ++n){
                                datas[i].myMap[datas[i].data[n][m]].push_back(n);
                            }
                            
                        }
                    }
                    
                    
                }
            }
            if(tbNameError){
                //      cout << "No such table!\n";
                //    exit(1);
                string rubbish;
                getline(cin,rubbish);
                cout << "Error: "<<  tableName<< " does not name a table in the database\n";
            }
        }
        //GENERATE
        else if (Command == "GENERATE"){
            bool tableNameError = true;
            bool colNameError = true;
            string buff;
            cin >> buff;
            string tableName = "def";
            string indexType = "def";
            cin >> tableName >> indexType;
            cin >> buff;
            cin >> buff;
            string colName = "def";
            cin >> colName;
            size_t indexCol = 0;
            for (size_t i = 0; i < datas.size(); ++i){
                if(datas[i].tableName == tableName){
                    tableNameError = false;
                    for (size_t j = 0; j < datas[i].colName.size(); ++j){
                        if(datas[i].colName[j] == colName){
                            indexCol = j;
                            colNameError = false;
                        }
                    }
                    if(colNameError){
                        cout<<"Error: "<<colName <<" does not name a column in "<< tableName<<"\n";
                        string rubbish;
                        getline(cin,rubbish);
                        break;
                    }
                    for (size_t j = 0; j < datas[i].colName.size(); ++j){
                        datas[i].hashIndex[j] = false;
                        datas[i].bstIndex[j] = false;
                    }
                    datas[i].myMap.clear();
                    datas[i].myHash.clear();
                    if(indexType == "hash"){
                        datas[i].hashIndex[indexCol] = true;
                        for (size_t k = 0; k < datas[i].data.size(); ++k){
                            //  datas[i].myHash[datas[i].data[k][indexCol]];
                            datas[i].myHash[datas[i].data[k][indexCol]].push_back(k);
                        }
                    }
                    else if (indexType == "bst"){
                        datas[i].bstIndex[indexCol] = true;
                        for (size_t k = 0; k < datas[i].data.size(); ++k){
                            //   datas[i].myMap[datas[i].data[k][indexCol]];
                            datas[i].myMap[datas[i].data[k][indexCol]].push_back(k);
                        }
                    }
                }
            }
            if (tableNameError){
                string rubbish;
                getline(cin,rubbish);
                cout << "Error: "<<  tableName<< " does not name a table in the database\n";
            }
            else{
                cout << "Created "<< indexType <<" index for table "<<tableName<<" on column "<<colName<<"\n";
            }
        }
        //JOIN
        else if (Command == "JOIN"){
            bool bstIndex = false;
            bool hashIndex = false;
            bool col1NameError = true;
            bool col2NameError = true;
            bool table1NameError = true;
            bool table2NameError = true;
            size_t totalPrint = 0;
            size_t numCol = 0;
            size_t table1Index = 0;
            size_t table2Index = 0;
            size_t col1Index = 0;
            size_t col2Index = 0;
            string buff;
            string tableName1 = "def";
            string tableName2 = "def";
            string colName1 = "def";
            string colName2 = "def";
            cin >> tableName1 >> buff >> tableName2 >> buff >> colName1 >> buff >> colName2 >> buff >> buff >> numCol;
            for(size_t j = 0 ; j < datas.size(); ++j){
                if(datas[j].tableName == tableName1){
                    table1NameError = false;
                    table1Index = j;
                }
                if(datas[j].tableName == tableName2){
                    table2NameError = false;
                    table2Index = j;
                }
            }
            if(table1NameError ){
                string rubbish;
                getline(cin,rubbish);
                cout << "Error: "<<  tableName1<< " does not name a table in the database\n";
                //exit(1);
                continue;
            }
            if(table2NameError ){
                string rubbish;
                getline(cin,rubbish);
                cout << "Error: "<<  tableName2<< " does not name a table in the database\n";
                continue;
            }
            for(size_t i = 0; i < datas[table1Index].colName.size();++i){
                if(colName1 == datas[table1Index].colName[i]){
                    col1Index = i;
                    col1NameError = false;
                }
            }
            for(size_t i = 0; i < datas[table2Index].colName.size();++i){
                if(colName2 == datas[table2Index].colName[i]){
                    col2Index = i;
                    col2NameError = false;
                }
            }
            if(col1NameError){
                string rubbish;
                getline(cin,rubbish);
                cout<<"Error: "<<colName1 <<" does not name a column in "<< tableName1<<"\n";
                //  exit(1);
                continue;
            }
            if(col2NameError){
                string rubbish;
                getline(cin,rubbish);
                cout<<"Error: "<<colName2 <<" does not name a column in "<< tableName2<<"\n";
                //  exit(1);
                continue;
            }
            
            if(datas[table2Index].bstIndex[col2Index]) bstIndex = true;
            if(datas[table2Index].hashIndex[col2Index]) hashIndex = true;
            vector<joinPrint> printOrder;
            for(size_t i = 0 ; i < numCol ; ++i){
                bool colNameError = true;
                string colName = "def";
                int tableIndex = 0;
                joinPrint jp;
                cin >> colName >> tableIndex;
                if(tableIndex == 1){
                    for(size_t k = 0; k < datas[table1Index].colName.size(); ++k){
                        if(datas[table1Index].colName[k] == colName){
                            colNameError = false;
                            jp.colIndex = k;
                            jp.tableindex = table1Index;
                            printOrder.push_back(jp);
                        }
                    }
                }
                else{
                    for(size_t k = 0; k < datas[table2Index].colName.size(); ++k){
                        if(datas[table2Index].colName[k] == colName){
                            colNameError = false;
                            jp.colIndex = k;
                            jp.tableindex = table2Index;
                            printOrder.push_back(jp);
                        }
                    }
                }
                
                if(colNameError){
                    if(tableIndex==1){
                        string rubbish;
                        getline(cin,rubbish);
                        cout<<"Error: "<<colName <<" does not name a column in "<< tableName1<<"\n";
                    }
                    else{
                        string rubbish;
                        getline(cin,rubbish);
                        cout<<"Error: "<<colName <<" does not name a column in "<< tableName2<<"\n";
                    }
                    //     exit(1);
                    break;
                }
            }
            if(!quiteMode){
                for(size_t i = 0; i < printOrder.size(); ++i){
                    cout << datas[printOrder[i].tableindex].colName[printOrder[i].colIndex] << " ";
                }
                cout << "\n";
            }
            for(size_t i = 0; i < datas[table1Index].data.size(); ++i){
                if(bstIndex){
                    auto it = datas[table2Index].myMap.find(datas[table1Index].data[i][col1Index]);
                    if(it!=datas[table2Index].myMap.end()){
                        size_t numToprint = datas[table2Index].myMap[datas[table1Index].data[i][col1Index]].size();
                        totalPrint += numToprint;
                        if(!quiteMode){
                            for(size_t j = 0; j < numToprint; ++j){
                                size_t rowNum = datas[table2Index].myMap[datas[table1Index].data[i][col1Index]][j];
                                for(size_t k = 0; k < printOrder.size(); ++k){
                                    if(printOrder[k].tableindex == table1Index){
                                        cout<<datas[table1Index].data[i][printOrder[k].colIndex]<< " ";
                                    }
                                    else{
                                        cout<<datas[table2Index].data[rowNum][printOrder[k].colIndex]<< " ";
                                    }
                                }
                                cout << "\n";
                            }
                        }
                        
                    }
                }
                else if (hashIndex){
                    auto it = datas[table2Index].myHash.find(datas[table1Index].data[i][col1Index]);
                    if(it!=datas[table2Index].myHash.end()){
                        size_t numToprint = datas[table2Index].myHash[datas[table1Index].data[i][col1Index]].size();
                        totalPrint += numToprint;
                        if(!quiteMode){
                            for(size_t j = 0; j < numToprint; ++j){
                                size_t rowNum = datas[table2Index].myHash[datas[table1Index].data[i][col1Index]][j];
                                for(size_t k = 0; k < printOrder.size(); ++k){
                                    if(printOrder[k].tableindex == table1Index){
                                        cout<<datas[table1Index].data[i][printOrder[k].colIndex]<< " ";
                                    }
                                    else{
                                        cout<<datas[table2Index].data[rowNum][printOrder[k].colIndex]<< " ";
                                    }
                                }
                                cout << "\n";
                            }
                        }
                        
                    }
                }
                else{
                    for(size_t j = 0; j < datas[table2Index].data.size(); ++j){
                        if(datas[table1Index].data[i][col1Index] == datas[table2Index].data[j][col2Index]){
                            ++totalPrint;
                            if(!quiteMode){
                                for(size_t k = 0; k < printOrder.size(); ++k){
                                    if(printOrder[k].tableindex == table1Index){
                                        cout<<datas[table1Index].data[i][printOrder[k].colIndex]<< " ";
                                        
                                    }
                                    else{
                                        cout<<datas[table2Index].data[j][printOrder[k].colIndex]<< " ";
                                        
                                    }
                                }
                                cout << "\n";
                            }
                            
                        }
                    }
                }
            }
            cout << "Printed "<<totalPrint << " rows from joining "<<tableName1 << " to "<< tableName2 << "\n";
        }
        else if (Command[0] == '#'){
            string buff;
            getline(cin,buff);
            continue;
        }
    }
}


