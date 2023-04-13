//Shayan Salsabilian
//Dominic Bartoni
// $Id: ubigint.cpp,v 1.8 2020/01/06 21:39:55 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;
#include "ubigint.h"
#include "debug.h"

ubigint::ubigint (unsigned long that) {
    if(that == 0){
     unsigned long zero = 0;
     ubig_value.push_back(zero);
    }
    while(that!=0){
      unsigned long value = that % 10;
      ubig_value.push_back(value);
      that = that/10;
    }
}

ubigint::ubigint (const string& that) { 
   for (auto iter = that.crbegin(); iter != that.crend(); ++iter) {
      char digit = *iter;
      if (not isdigit (digit)){ 
            throw invalid_argument ("ubigint::ubigint(" + that + ")");
         }
      unsigned long value = digit;
      ubig_value.push_back(value-'0');
      }
}

ubigint ubigint::operator+ (const ubigint& that) const { 
   ubigint addition;   
   int carryin = 0;
   size_t idx = 0;
   while(idx < (that.ubig_value.size()) 
   || idx < (this->ubig_value.size())){
      if(idx > (that.ubig_value.size())){
         int value = carryin + this->ubig_value[idx];
         carryin = value / 10;
         value = value % 10;
         addition.ubig_value.push_back(value);
         ++idx;
         continue;
      }      
      if(idx > (this->ubig_value.size())){
         int value = carryin + (that.ubig_value[idx]);
         carryin = value / 10;
         value = value % 10;
         addition.ubig_value.push_back(value);
         ++idx;
         continue;
      }
      int value = carryin + (this->ubig_value[idx]) + 
      (that.ubig_value[idx]);
      carryin = value / 10;
      value = value % 10;
      addition.ubig_value.push_back(value);
      ++idx;
   }
   if(carryin>0){
      addition.ubig_value.push_back(carryin);
   }
   return addition;   
}

ubigint ubigint::operator- (const ubigint& that) const { 
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
      size_t idx = 0;
      int borrow = 0;
      ubigint subtraction;
      while(idx < (that.ubig_value.size())
      || idx < (this->ubig_value.size())){
         if(idx >= (that.ubig_value.size())){
            long value = this->ubig_value[idx] + borrow; 
            if( value < 0){
               value = value +  10;
               borrow = -1;
            }else{
               borrow = 0;
            }
            subtraction.ubig_value.push_back(value);
            ++idx;
            continue;
         }
      int value1 = this->ubig_value[idx];
      int value2 = that.ubig_value[idx];
      if((value1+borrow) < value2){
         value1 = value1 + 10;
         int finalvalue = (borrow + value1) - value2;
         borrow=-1;
         subtraction.ubig_value.push_back(finalvalue); 
      }else{
         int finalvalue = (borrow + value1) - value2;
         subtraction.ubig_value.push_back(finalvalue);
         borrow = 0;
      }
      ++idx;
   } 
   for(long idx1 = ((static_cast<long> 
   (subtraction.ubig_value.size())) - 1);idx1 >= 0; --idx1){
      int value = (static_cast<int> (subtraction.ubig_value[idx1]));
      if(value == 0 && this->ubig_value.size() > 1){
         subtraction.ubig_value.pop_back();
      }else{
         break;
      }
   }
   return subtraction;        
}

ubigint ubigint::operator* (const ubigint& that) const { 
   ubigint product;
   unsigned long value = ubig_value.size();
   unsigned long value1 = that.ubig_value.size();
   product.ubig_value.assign(value+value1, 0);
   unsigned long digit = 0;
   for(unsigned long idx=0; idx<(value); ++idx){
      unsigned long carry = 0;
      for(unsigned long jdx=0; jdx<(value1); ++jdx){
         digit = product.ubig_value[idx+jdx] + 
         (ubig_value[idx] * that.ubig_value[jdx]) + carry;
         unsigned long value2 = digit % 10;
         product.ubig_value[idx+jdx] = value2; 
         carry = digit/10;    
      }
      product.ubig_value[idx+(value1)] = carry;
   }
   for(int idx = (((product.ubig_value.size())) - 1);
   idx >= 0; --idx){
      int value3 = (static_cast<int> (product.ubig_value[idx]));
      if(value3 == 0){
         product.ubig_value.pop_back();
      }else{
         break;
      }
   }   
   return product;
}

void ubigint::multiply_by_2() {
   int carry = 0;
   DEBUGF('~', "multiply begin" << *this << "/");
   for(size_t i=0; i < (this->ubig_value.size()); ++i){
      int value = this->ubig_value[i];
      value = (2*value) + carry;
      carry = value / 10;
      value = value % 10;
      this->ubig_value[i] = value;
   }
   if(carry > 0){
      this->ubig_value.push_back(carry);
   }
   DEBUGF('~', "multiply final " << *this << "/");      
}

void ubigint::divide_by_2() { 
   DEBUGF('~', "divide begin" << *this << "/");
   size_t size = this->ubig_value.size();
   for(size_t idx=0; idx < size; ++idx){
      int value = this->ubig_value[idx];
      value = value/2;
      if(idx < (size - 1) 
      && (this->ubig_value[idx+1] % 2 == 1)){
         value = value + 5;
      }
      this->ubig_value[idx] = value; 
   }
   for (int idx = size-1; idx>=0; --idx){
      if (this->ubig_value[idx]==0 && this->ubig_value.size()!=1) 
         this->ubig_value.pop_back();
         else break;
   }
   DEBUGF('~', "divide final " << *this << "/");
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
// NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
      ubigint power_of_2 {1};
      ubigint quotient {0};
      ubigint remainder {dividend}; 
      DEBUGF('~', "middle divide" << dividend << '/');
      while (divisor < remainder) { 
         divisor.multiply_by_2();
         power_of_2.multiply_by_2();
      }
      while (power_of_2 > zero) {
         DEBUGF('~',  power_of_2 << ">"<< zero<<"?"<<"/");
         if (divisor <= remainder) {
            DEBUGF('~',  divisor << "<="<< remainder<<"?"<<"/");
            remainder = remainder - divisor;
            quotient = quotient + power_of_2;
         }
         divisor.divide_by_2();
         power_of_2.divide_by_2();
       }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const { 
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const { 
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   if(this->ubig_value.size() != that.ubig_value.size()){
      return false;
   }
   size_t size = this->ubig_value.size();
   for(size_t idx = 0; idx<size; ++idx){
     if(this->ubig_value[idx] != that.ubig_value[idx]){
        return false;
     }
   }
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   size_t size1 = (this->ubig_value.size());
   size_t size2 = (that.ubig_value.size()); 
   if(size1 > size2){
      return false;
   }else if(size2 > size1){ 
       return true;
   }else{
      for(int idx=(this->ubig_value.size()-1); idx>=0;--idx){
         int value1 = this->ubig_value[idx];
         int value2 = that.ubig_value[idx];
         if(value1 > value2){
            return false;
         }
         if(value2 > value1){
            return true;
         }   
      }
   }
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   string value;
   int n=0;
   size_t size = that.ubig_value.size();
   for (int idx = size-1; idx>=0; --idx){
      if(n==69){
         value += '\\';
         value += '\n';
         n=0; 
      }  
      value += (static_cast<char> (that.ubig_value[idx]+'0'));
      n++;
   }
   return out << (value);
}


