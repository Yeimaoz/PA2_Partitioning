#ifndef NET_H
#define NET_H
#include <iostream>
#include <string>
#include <vector>
#include "block.h"
using namespace std;
class Block;
class Net{
    friend ostream &operator<<(ostream &output, const Net &in);
    public:
        string name;
        vector<Block*> blocks;

        Net();
        Net(string);
        Net(const Net&);

        void add_block(Block*);
};
#endif