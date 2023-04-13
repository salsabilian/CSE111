// $Id: commands.cpp,v 1.19 2020-10-20 18:23:13-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd); //find the inode where currently at
   if (result == cmd_hash.end()) { //if were at the end return an error
      throw command_error (cmd + ": no such function");
   }
   return result->second; //why are we returning the second result?
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   string out="";
   for(size_t i=1;i<words.size();++i){ //get the directory we want to go into
      out+=words[i];
   }

   inode_ptr hi= state.getCWD();  //get the inode we are currently pointing to 
   base_file_ptr foo = hi->getContents(); //get its content as a base_file_ptr
   inode_ptr next=foo->find(out); //search its contents for the inode_ptr we are trying to enter


   base_file_ptr final=next->getContents();
   wordvec output = final->readfile();

   cout << word_range (output.cbegin(), output.cend()) << endl;

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   if(words.size()==1){
      state.setCWDtoRoot();
      return;
   }

   if(words.size()>3){
      string error = "Error took this many arguments at cd: "+to_string(words.size()-1);
      throw command_error (error);
      return;
   }

   string out=words.at(1);

   inode_ptr hi= state.getCWD();  //get the inode we are currently pointing to 
   base_file_ptr foo = hi->getContents(); //get its content as a base_file_ptr
   inode_ptr next=foo->find(out); //search its contents for the inode_ptr we are trying to enter
   state.setCWD(next); //set our cwd to that new pointer
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   string out=""; 
   inode_ptr hi=state.getCWD(); //get our current working directory pointer
   base_file_ptr foo = hi->getContents(); //get its content and save it in a base_file_ptr
   foo->print(); //then print
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   string out = words[1];
   inode_ptr curr = state.getCWD();
   base_file_ptr cont = curr->getContents();

   inode_ptr n=cont->mkfile(out);
   base_file_ptr dom = n->getContents();

   wordvec new_word;
   for(size_t i=2;i<words.size();++i){
      new_word.push_back(words[i]);
   }

   dom->writefile(new_word);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   string out="";
   for(size_t i=1;i<words.size();++i){
      out+=words[i];
   }
   inode_ptr hi=state.getCWD();

   base_file_ptr foo = hi->getContents();

   foo->mkdir(out);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string out="";
   for(size_t i=1;i<words.size();++i){
   	out+=words[i];
  	}
   out+=' ';
   state.prompt(out);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
