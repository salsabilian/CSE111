//Shayan Salsabilian
//Dominic Bartoni
// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"


bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}


bigint::bigint (const ubigint& uvalue_, bool is_negative_):
   uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   bigint result;
   if(is_negative == false && that.is_negative == false){
      result = uvalue + that.uvalue;
      result.is_negative = false;
   }else if(is_negative == false && that.is_negative == true){
      if(uvalue > that.uvalue){
         result = uvalue - that.uvalue;
         result.is_negative = false;
      }else if(uvalue < that.uvalue){
         result = that.uvalue - uvalue;
         result.is_negative = true;
      }
   }else if(is_negative == true && that.is_negative == false){
      if(uvalue > that.uvalue){
         result = uvalue - that.uvalue;
         result.is_negative = true;
      }else if(that.uvalue > uvalue){
         result = that.uvalue - uvalue;
         result.is_negative = false;
      }
   }else{
      result = uvalue + that.uvalue;
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   bigint result;
   if(is_negative == false && that.is_negative == false){
      if(uvalue > that.uvalue){
         result = uvalue - that.uvalue;
         result.is_negative = false;
      }else if(uvalue < that.uvalue){
         result = (that.uvalue - uvalue);
         result.is_negative = true;
      }else return bigint{0};
   }else if(is_negative == true && that.is_negative == false){
      result = (uvalue + that.uvalue);    
      result.is_negative = true;
   }else if(is_negative == false && that.is_negative == true){
      result = (uvalue + that.uvalue);
      result.is_negative = false; 
   }else if(is_negative == true && that.is_negative == true){
      if(that.uvalue > uvalue){
         result = (that.uvalue - uvalue);
         result.is_negative = false;
      }else if(uvalue > that.uvalue){
         result = (that.uvalue - uvalue);
         result.is_negative = true;
      }
    
   }
   return result;
}


bigint bigint::operator* (const bigint& that) const {
   bigint result = uvalue * that.uvalue;
   if(is_negative == true && that.is_negative == true){
      result.is_negative = false;
   } else if(is_negative == true || that.is_negative == true){
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   if(is_negative == true && that.is_negative == true){
      result.is_negative = false;
   }else if(is_negative == true || that.is_negative == true){
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   if(is_negative == true && that.is_negative == true){
      result.is_negative = false;
   }else if(is_negative == true || that.is_negative == true){
      result.is_negative = true;
   }
   return result;
}

bool bigint::operator== (const bigint& that) const {
   if(is_negative != that.is_negative){
      return false;
   }else{
      return that.uvalue == uvalue;
   }
   return that.uvalue == uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if(is_negative == true && that.is_negative == false){
      return true;
   }else if(that.is_negative==true && is_negative == false){
      return false;
   }
   return uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   if(that.is_negative == false){ 
      return out << that.uvalue;
   }else{
      return out << "-" << that.uvalue;
   }
}

