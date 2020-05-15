#include "block.h"

Block::Block(){}

Block::Block(string name){
    this->name = name;
}

Block::Block(const Block& in){
    this->name = in.name;
}