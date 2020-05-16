#ifndef BLOCK_H
#define BLOCK_H
#include <iostream>
#include <string>
#include <vector>
#include "net.h"
using namespace std;

class Net;
class Block{
    friend ostream &operator<<(ostream &output, const Block &in);
    public:
        string name;
        int belongs2;
        vector<Net*> nets;

        Block();
        Block(string);
        Block(const Block&);

        void add_net(Net*);
};
#endif