#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <string>
#include <set>
#include "net.h"
using namespace std;

class Block{
    public:
        string name;
        
        Block();
        Block(string);
        Block(const Block&);
};
#endif