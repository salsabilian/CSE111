//Shayan Salsabilian
//Dominic Bartoni
// $Id: main.cpp,v 1.12 2020-10-22 16:50:08-07 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <regex>
#include <fstream>
using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"
#include "xless.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void parsing(listmap<string, string>& list1, 
istream& infile, char* filename){
  regex comment_regex{R"(^\s*(#.*)?$)"};
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
  int i = 0;
   for(;;){
    ++i;
    string line;
    getline(infile, line);
    smatch result;
    if(infile.eof()) break;
     if (regex_search (line, result, comment_regex)) {
       if(strcmp(filename," ")==0){
         cout<<"-: "<<i<<": "<<line<<endl;
       }else{
         cout<<filename<<": "<<i<<": "<<line<<endl;
       }
       continue;
     }if (regex_search (line, result, key_value_regex)) {
       if(result[1] == "" && result[2] == ""){
         if(strcmp(filename, " ")==0){
           cout<<"-: "<<i<<": "<<"="<<endl;
         }else{
           cout<<filename<<": "<<i<<": "<<"="<<endl;
         }
         for(auto &start : list1){
           cout<<start.first<<" = "<<start.second<<endl;
          }
       }else if(result[1] == ""){
         if(strcmp(filename, " ")==0){
           cout<<"-: "<<i<<": ="<<result[2]<<endl;
         }else{
           cout<<filename<<": "<<i<<": ="<<result[2]<<endl;
         }
         for(auto &start : list1){
           if(list1.keyless(start.second, result[2]) == false 
           && list1.keyless(result[2], start.second)==false){
            cout<<start.first<<" = "<<start.second<<endl;
           }
          }
       }else if(result[2] == ""){
         auto itor = list1.find(result[1]);
         if(itor!=list1.end())
           list1.erase(itor);
         }else{
           str_str_pair pair(result[1], result[2]);
           if(strcmp(filename, " ")==0){
             cout << "-: "<<i<<": "<< result[1] <<"="<< result[2]<<endl;
           }else{
             cout<<filename<<": "<<
             i<<": "<<result[1]<<"="<<result[2]<<endl;
           } 
           list1.insert(pair);
           cout <<result[1]<<" = "<<result[2]<<endl;
         }
       }else if (regex_search (line, result, trimmed_regex)) {
         auto itor = list1.find(result[1]);
         if(strcmp(filename, " ")==0){
           cout <<"-: "<<i<<": "<<result[1]<<endl;
         }else{
           cout<<filename<<": "<<i<<": "<<result[1]<<endl;
         }
         if(itor == list1.end()){
           cerr<<result[0]<<": key not found"<<endl;
         }else{
           cout <<(*itor).first<< " = "<<(*itor).second<<endl;
         }
       }
   }
}

int main (int argc, char** argv) {
   int status = 0;
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   listmap<string, string> list1;
   if(argc == 1){
       char cin_name[]= " ";
       parsing(list1, cin, cin_name);
   }else{
     for (int i=1; i<argc; ++i) {
      if(strcmp(argv[i],"-")){
        ifstream infile(argv[i]);
        if (infile.fail()) {
           cerr << "keyvalue: "<<argv[i]
           <<": No such file or directory"<<endl;
           continue;
           status = 1;
        }else {
          parsing(list1, infile, argv[i]);
          infile.close();
        }
      }else{
        char cin_name[]=" "; 
        parsing(list1, cin, cin_name);
      }
     }
   }
  return status;
}
