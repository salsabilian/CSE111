//Shayan Salsabilian
//Dominic Renzo Bartoni
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
   {"#"     , fn_echo  },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
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
   wordvec out;
   for(size_t i=1;i<words.size();++i){ 
     try{
     inode_ptr hi= state.getCWD();  
     base_file_ptr foo = hi->getContents();
     
     inode_ptr next=foo->find(words[i]); 
     base_file_ptr s = next->getContents();
     wordvec output = s->readfile();
     cout << word_range (output.cbegin(), output.cend()) << endl;
     }catch(file_error&){
      return;
     }

   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

inode_ptr nest(const wordvec& words, const inode_ptr origin){
   if (words[0]=="."){
      return origin;
   }
   inode_ptr source=origin;
   base_file_ptr source_content = source->getContents();
   if(words[0]==".."){
      return source_content->find("..");
   }
   wordvec todo = source_content->list();
   for(size_t i=0;i<words.size();++i){
      source_content= source->getContents();
      source= source_content->find(words[i]);
   }
   return source;
}

void fn_cd (inode_state& state, const wordvec& words){
   if(words.size()>3 ){
      string error = "Error took this many arguments at cd: "
      + to_string(words.size()-1);
      throw command_error (error);
      return;
   }
   if(words.size()==1 or words[1]=="/"){
      state.setCWDtoRoot();
      wordvec temp=state.returnCWD();
      return;
   }
   wordvec vec= split(words.at(1), "/");
   inode_ptr hi= state.getCWD(); 
   try{
      inode_ptr next=nest(vec,hi);
      state.setCWD(next); //set our cwd to that new pointer
   }catch(file_error&){
      return;
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   if(words[0][0]=='#') return;
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   int exit=0;
   if(words.size()>2){
      exit=127;
   }
   else if(words.size()<2){
      exit=0;
   }
   else {
      if (words[1].find_first_not_of("0123456789") 
      != std::string::npos){
         exit=127;
      }
      else{
         exit=stoi(words[1]);
      }
   }
   exec::status(exit);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   string out=""; 
   inode_ptr hi=state.getCWD();
   base_file_ptr foo = hi->getContents();
   foo->print(); //then print
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void recursion(inode_ptr source){ 
   base_file_ptr source_content= source->getContents();
   string self = source_content->findkey(source);   
   string arg="";
   inode_ptr parent=source_content->find("..");
   base_file_ptr parent_content=parent->getContents();

   if(parent!=source_content->find(".")){
      arg=parent_content->findkey(source);
   }
   wordvec todo=source_content->list();
   source_content->print(arg);

   for(size_t i=0;i<todo.size();++i){
      string next_key=todo[i];
      inode_ptr next=source_content->find(next_key);
      recursion(next);
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   if(words.size() == 1){
      fn_ls(state, words);
      return;
   }
   inode_ptr start=state.getRoot();
   for(size_t i=1;i<words.size();++i){
      wordvec parse = split(words[i], "/");
      if (parse.size()==0){ 
         start = state.getRoot();
         recursion(start);
      }else{
         inode_ptr source=nest(parse, state.getCWD());
         recursion(source);
      }
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words){
   string out = words[1];
   wordvec parse = split(words[1], "/");
   string puts= parse.back();
   parse.pop_back();

   inode_ptr curr=state.getCWD();
   try{
       curr= nest(parse, state.getCWD());
   }catch(file_error&){
      return;
   }

   base_file_ptr cont = curr->getContents();
   if(cont->doesExist(puts)){
      inode_ptr check=cont->find(puts);
      base_file_ptr check_content= check->getContents();
      if(check_content->whatType()){
         string error = "Error: "+puts+
         " already exists as a directory\n";
         throw command_error (error);
         return;
      }
   }

   inode_ptr n=cont->mkfile(puts);
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
   if(words.size()==1){
      string error = "Error: took this many arguments at mkdir: "
      + to_string(words.size()-1);
      throw command_error (error);
      return;
   }
   wordvec parse=split(words[1],"/");
   string puts= parse.back();
   parse.pop_back();
   try{
   inode_ptr hi=nest(parse,state.getCWD());
   base_file_ptr foo = hi->getContents();
   foo->mkdir(puts);
   }catch(file_error&){
      throw command_error("invalid directories? \n");
      return;
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string out="";
   for(size_t i=1;i<words.size();++i){
      out+=words[i]+" ";
   }
   state.prompt(out);
}

void fn_pwd (inode_state& state, const wordvec& words){
   if(words.size()!=1){
      string error = "Error: took this many arguments at pwd: "
      + to_string(words.size()-1);
      throw command_error (error);
      return;
   }
   string out="";
   inode_ptr child_ptr = state.getCWD();

   while(child_ptr!=state.getRoot()){
      base_file_ptr child_cont = child_ptr->getContents();
      inode_ptr parent_ptr=child_cont->find("..");
      base_file_ptr parent_cont=parent_ptr->getContents();
      out=parent_cont->findkey(child_ptr)+"/"+out;
      child_ptr=parent_ptr;
   }

   cout<<"/"<<out<<endl;
   return; 
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   if(words.size()!=2){
      string error = "Error: took this many arguments at rm: "
      + to_string(words.size()-1);
      throw command_error (error);
      return;
   }   
   inode_ptr source=state.getCWD();
   base_file_ptr source_content=source->getContents();
   source_content->remove(words[1]);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void recursive_delete(inode_ptr source){ 
   base_file_ptr source_content= source->getContents();
   string self = source_content->findkey(source);   
   string arg="";
   wordvec todo=source_content->list();
   for(size_t i=0;i<todo.size();++i){
      string next_key=todo[i];
      inode_ptr next=source_content->find(next_key);
      recursive_delete(next);
      source_content->remove(next_key);
   }
   //set to nullptr
}

void fn_rmr (inode_state& state, const wordvec& words){
   if(words.size()!=2){
      string error = "Error: took this many arguments at rm: "
      + to_string(words.size()-1);
      throw command_error (error);
      return;
   }   
   inode_ptr source=state.getCWD();
   base_file_ptr source_content=source->getContents();
   inode_ptr rem = source_content->find(words[1]);
   recursive_delete(rem);
   source_content->remove(words[1]);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
