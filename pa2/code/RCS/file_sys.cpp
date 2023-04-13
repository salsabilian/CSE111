// $Id: file_sys.cpp,v 1.8 2020-10-22 14:37:26-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
   root = make_shared<inode> (file_type::DIRECTORY_TYPE); //make an inode of type directory
   shared_ptr<directory> initDirectory = make_shared<directory> (); //create a directory to place in the inode
   root->contents = initDirectory; //place the directory into the inode
   initDirectory->setDirent(".", root); //set its . and .. operators to itself since the root has no parent
   initDirectory->setDirent("..", root);
   cwd = root;
}

const string& inode_state::prompt() const { return prompt_; }

void inode_state::prompt (const string& new_prompt){
  prompt_ = new_prompt;
}


ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}



file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::print(){
  throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::find(const string& filename){
  throw file_error ("is a " + error_file_type());  
}


size_t plain_file::size() const {
   int temp_size=this->data.size();
   int size;
   for(int i=0; i<temp_size;++i){
   		string temp_string=this->data[i];
   		temp_string=temp_string;
   		size+=temp_string.length();
   		
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   data=words;
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size = this->dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
    inode_ptr direct=make_shared<inode>(file_type::DIRECTORY_TYPE);//create an inode of type directory
    shared_ptr<directory> foo= make_shared<directory>(); //make the directory call it foo

    inode_ptr b=this->findSelf(); //find the pointer to our current inode

    direct->setContents(foo); //put the directory foo in our inode

    string one="."; 
    string two="..";
    foo->setDirent(one,direct); //set the . of directory inode direct to itself
    foo->setDirent(two,b);   //set the .. of directory inode direct to its parent b
    
    this->setDirent(dirname,direct);  //set our current directorys name to the dirname we specified

   DEBUGF ('i', dirname);
   return direct; //return our new inode pointer with . .. and the directory we created
}

inode_ptr directory::mkfile (const string& filename) {
   inode_ptr fil = make_shared<inode>(file_type::PLAIN_TYPE); //create an inode of type file
   shared_ptr<plain_file> fn = make_shared<plain_file>(); //create the file
   fil->setContents(fn);  //put the file into the file inode
   this->setDirent(filename, fil);
   DEBUGF ('i', filename);
   return fil;
}
