#include "partitioner.h"

Partitioner::Partitioner(){
    cout << "Creating a partitioner ..." << endl;
}

void Partitioner::read_file(string filename){
    cout << "Parsing " << filename << " ..." << endl;
    fstream infile(filename, ios::in);
    string buffer;
    string line;
    while (getline(infile, line)){
        if (!eon(line)){
            buffer += " " + line; 
            continue;
        }
        Net* net = parse_net(buffer != "" ? buffer + " " + line : line);
        this->nets[net->name] = net;
        buffer = "";
    }
    infile.close();
    find_fanouts();
    evaluate_p_value();
}

void Partitioner::write_file(string filename){
    cout << "Writing " << filename << " ..." << endl;
    fstream outfile(filename, ios::out);
    outfile << "cut_size " << this->cut_size << endl;
    for(int i = 0; i < 2; ++i){
        outfile << char(i+65) << " " << this->cuts[i].size() << endl;
        for(auto block : this->cuts[i])
            outfile << block->name << endl;
    }
    outfile.close();
    print_results();
}

bool Partitioner::eon(string line){
    smatch match;
    regex expr("}");
    return regex_search(line, match, expr);
}

Net* Partitioner::parse_net(string line){
    // net name extraction
    smatch match_net;
    regex expr_net("NET *([^ ]+) *\\{");
    regex_search(line, match_net, expr_net);
    string net_name = match_net[1];
    Net* net = new Net(net_name); 

    // blocks extraction
    smatch match_bks;
    regex expr_bks("\\{(.*)\\}");
    regex_search(line, match_bks, expr_bks);

    // split blocks by \w
    // directly add blocks to net, if it exists
    // if not, construct it, then add it to graph and net
    smatch match_bk;
    regex expr_bk("[^ ]*");
    istringstream iss(match_bks[1]);
    for(std::string block_name; iss >> block_name; ){
        Block* block;
        auto block_iter = this->blocks.find(block_name);
        if (block_iter != this->blocks.end())
            block = block_iter->second;
        else {
            block = new Block(block_name);
            this->blocks[block_name] = block;
        } 
        net->add_block(block);
        block->add_net(net);
    }
    return net;
}

void Partitioner::initialize(){
    construct_initial_solution();
    evaluate_cut_size();
    evaluate_block_cost();
    construct_gain_bucket();
}

void Partitioner::construct_initial_solution(){
    srand(time(NULL));
    for (auto& b : this->blocks){
        // b.second->belongs2 = (b.first[1]-48) % 2;
        b.second->belongs2 = rand() % 2;
        this->cuts[b.second->belongs2].insert(b.second);
    }
}

void Partitioner::find_fanouts(){
    for(auto& block : this->blocks){
        for(auto net : block.second->nets){
            vector<Block*> fanouts;
            for(auto bk : net->blocks){
                if (bk != block.second)
                    fanouts.push_back(bk);
            }
            block.second->fanouts.push_back(fanouts);
        }
    } 
}

void Partitioner::FM_algorithm(){
    cout << "Running FM algorithm ..." << endl;
    while (1){
        Block* candidate = get_candidate();
        if (!candidate)
            break;
        while (candidate){
            // cout << *candidate << endl;
            this->cuts[candidate->belongs2].erase(candidate);
            move(candidate);
            this->cuts[candidate->belongs2].insert(candidate);
            candidate = get_candidate();
        }
        for(auto& block : this->blocks)
            block.second->moved = false;
    }
    evaluate_cut_size();
}

void Partitioner::evaluate_p_value(){
    int p_value = -1;
    for(auto& block : this->blocks)
        p_value = (int(block.second->nets.size()) > p_value ? block.second->nets.size() : p_value);
    this->p_value = p_value;
}

void Partitioner::construct_gain_bucket(){
    this->gain_bucket = vector<set<Block*>>(this->p_value*2+1, set<Block*>());
    for(auto& block : this->blocks){
        if (!block.second->moved)
            this->gain_bucket[block.second->cost+this->p_value].insert(block.second);
    }
}

void Partitioner::evaluate_cut_size(){
    int cut_size = 0;
    for(auto& net : this->nets){
        int belongs[2] = {0, 0};
        for(auto block : net.second->blocks){
            belongs[block->belongs2] = 1;
            if (belongs[0] && belongs[1]){
                cut_size += 1;
                break;
            }
        }
    }
    this->cut_size = cut_size;
}

void Partitioner::evaluate_block_cost(){
    for(auto& block : this->blocks)
        block.second->update();
}

Block* Partitioner::get_candidate(){
    for(int i = this->gain_bucket.size()-1; i >= this->p_value; --i)
        for(auto block : this->gain_bucket[i]){
            int size[2] = {int(this->cuts[0].size()), int(this->cuts[1].size())};
            int diff1 = abs(size[0]-size[1]);
            size[block->belongs2]-=1;
            size[!block->belongs2]+=1;
            int diff2 = abs(size[0]-size[1]);
            bool under_ratio = (diff2 < diff1) | (diff2 <= float(this->blocks.size())/5);
            if (!block->moved && under_ratio)
                return block;
            }
    return NULL;
}

vector<vector<Block*>> Partitioner::get_distribution(vector<Block*>& blocks){
    vector<vector<Block*>> distribution(2, vector<Block*>());
    for(auto block : blocks)
        distribution[block->belongs2].push_back(block);
    return distribution;
}

void Partitioner::move(Block* candidate){
    candidate->moved = true;
    int move2 = !candidate->belongs2;
    set<Block*> updated;
    for(int i = 0; i < candidate->nets.size(); ++i){
        auto distribution = get_distribution(candidate->fanouts[i]);
        // line 6
        if (distribution[move2].empty()){
            for(auto block : candidate->nets[i]->blocks)
                if (!block->moved){
                    block->cost++;
                    updated.insert(block);
                }
        } else if (distribution[move2].size() == 1){
            for(auto block : distribution[move2])
                if (!block->moved){
                    block->cost--; 
                    updated.insert(block);
                }  
        }
        // line 8
        if (distribution[candidate->belongs2].empty()){
            for(auto block : candidate->nets[i]->blocks)
                if (!block->moved){
                    block->cost--; 
                    updated.insert(block);
                }  
        } else if (distribution[candidate->belongs2].size() == 1){
            for(auto block : distribution[candidate->belongs2])
                if (!block->moved){
                    block->cost++; 
                    updated.insert(block);
                }  
        }
    }
    candidate->belongs2 = move2;
    this->gain_bucket[candidate->cost+this->p_value].erase(candidate);
    candidate->update();
    update_gain_bucket(updated);
}

void Partitioner::update_gain_bucket(set<Block*>& updated){
    for(auto block : updated){
        for(auto& blocks : this->gain_bucket){
            if (blocks.find(block) != blocks.end()){
                blocks.erase(block);
                break;
            }
        }
        this->gain_bucket[block->cost+this->p_value].insert(block);
    }
}

void Partitioner::print_blocks(){
    cout << "#Block: " << this->blocks.size() << endl;
    for (auto block : this->blocks)
        cout << " -> " << *(block.second) << " ( " << block.second->cost << " )"<< endl;
}

void Partitioner::print_nets(){
    cout << "#Net: " << this->nets.size() << endl;
    for (auto nets : this->nets)
        cout << " -> " << *(nets.second) << endl;
    cout << endl;
}

void Partitioner::print(){
    print_blocks();
    print_nets();
}

void Partitioner::print_gain_bucket(){
    cout << "#Bucket: p = " << this->p_value << endl;
    for(int i = this->gain_bucket.size()-1; i >= 0 ; --i){
        cout << " -> Bucket[" << i-this->p_value << "]: ";
        for(auto block : this->gain_bucket[i])
            cout << block->name << " ";
        cout << endl;
    }
}

void Partitioner::print_results(){
    cout << "cut_size " << this->cut_size << endl;
    for (int i = 0; i < this->group_size; ++i){
        cout << "#" << char(i+65)<< ": " << this->cuts[i].size() << endl;
        cout << " -> { ";
        for (auto& b : this->cuts[i])
            cout << b->name << " ";
        cout << "}" << endl;
    }
}