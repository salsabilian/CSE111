//Shayan Salsabilian
//Dominic Bartoni
// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   while(begin()!=end()){
    this->erase(begin());
   }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   node* t=nullptr;
   node* tail = this->end().where;
   iterator itor = this->exist(pair.first);
   if(this->empty()){
     t = new node(tail, tail, pair); //link t to the anchor
     tail->prev=t; //link anchor to t
     tail->next=t;
   }
   else if(this->find(pair.first)!=this->end()){
      itor->second=pair.second;
      return itor.where;
   }
   else if(itor==this->begin()){
      t=new node(itor.where,itor.where->prev,pair);
      tail->next=t;
      itor.where->prev=t;
     }
  else if(itor==this->end()){
      node* here=this->end().where->prev;
      t=new node(this->end().where,here,pair);
      here->next=t;
      tail->prev=t;
     }
    else{
      node* bef=itor.where->prev;
      t=new node(itor.where,bef,pair);
      bef->next=t;
      itor.where->prev=t;
    }
   DEBUGF ('l', &pair << "->" << pair);
   return t;
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
  if(this->empty()) return this->end();
  if (begin()==end())return end();
  for(auto start=begin();start!=end();++start) {
    if((less(start->first, that)==false && (less(that,start->first))==false)) {
      return start;
    }
    if (start==end()) break;
  }
  return this->end();
}

////
//// listmap::exist(const key_type&)
////
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::exist (const key_type& that) {
if (empty())return end();
auto start = begin();
    while(less(start->first,that)&&start!=end()){
      ++start;
      if(start==end())break;
    }
    return start;
}


//
// iterator listmap::erase (iterator position)
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {  
  node* loc=position.where;  
  node* tail=end().where;
  iterator out=++position;
  --position;
  if(loc->next==tail && loc->prev==tail){
    tail->prev=tail;
    tail->next=tail;
  }
  else if(loc->prev==tail){
    tail->next=loc->next;
    loc->next->prev=tail;
  }
  else if(loc->next==tail){
    tail->prev=loc->prev;
    loc->prev->next=tail;
  }
  else{
    node* bef=loc->prev;
    node* aft=loc->next;
    bef->next=aft;
    aft->prev=bef;
  }
  loc->next=nullptr;
  loc->prev=nullptr;

   delete loc;
   DEBUGF ('l', &*position);
   return out;  
}


