#include "block.h"

ostream &operator<<(ostream &output, const Block &in){
    Block buffer = in;
    output << "Block " << buffer.name << " ( " << buffer.belongs2 << " )" << " { ";
    for (auto net : buffer.nets)
        cout << net->name << " ";
    output << "}";
	return output; 
}

Block::Block(){}

Block::Block(string name){
    this->name = name;
    this->belongs2 = -1;
    this->moved = false;
}

Block::Block(const Block& in){
    this->name = in.name;
    this->belongs2 = in.belongs2;
    this->nets = in.nets;
    this->moved = in.moved;
}

void Block::add_net(Net* net){
    this->nets.push_back(net);
}

void Block::update(){
    int cost = 0;
    for(int i = 0; i < this->nets.size(); ++i){
        int belongs[2] = {0, 0};
        for(auto& bk : this->fanouts[i])
            belongs[bk->belongs2] += 1;
        if (!belongs[this->belongs2])
            cost += 1;
        else if (!belongs[!this->belongs2])
            cost -= 1;
    }
    this->cost = cost;
}