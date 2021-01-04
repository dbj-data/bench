#pragma once

#include <unordered_set>
#include <string>
#include <utility>

namespace {

 using namespace std;
 
 using node = pair< int, string > ;
 
 struct node_hash final {
         inline static std::hash<string> hash_ ;
     size_t operator()( node const & node_ ) const noexcept
     {  
         return hash_( node_.second ) ;
     };
 };
 
 struct node_equal final {
     bool operator() ( node const & left_ , node const & right_) const noexcept
     {  
         return (left_.first == right_.first) ;
     };
 };
 
 
int main()
{
// simple comparison demo
    unordered_set<node, node_hash, node_equal > example 
      = {{1, "1"}, {2,"2"}, {3,"3"} };
 
    auto search = example.find({2,"2"});
    if (search != example.end()) {
        std::cout << "Found {" << (*search).first << "," << (*search).second << "}\n"
        << "There is:" << example.count({2,"2"})  << " elements matching that key\n" ;
    } else {
        std::cout << "Not found\n";
    }
 
 
}

}