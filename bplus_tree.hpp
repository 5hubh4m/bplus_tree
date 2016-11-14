//
//  bplus_tree.hpp
//  bplus_tree
//
//  Created by Shubham Chaudhary on 26/10/16.
//  Copyright © 2016 Shubham Chaudhary. All rights reserved.
//

#ifndef bplus_tree_h
#define bplus_tree_h


#include <vector>
#include <memory>

#define NO_ACTION false
#define ACTION true

const size_t b = 2;

struct bplus_node {
    std::vector<int> keys;
    std::vector<std::shared_ptr<bplus_node>> children;
    
    std::shared_ptr<bplus_node> next;
    
    bool is_leaf() {
        return children.empty();
    }
};

class bplus_tree {
private:
    std::shared_ptr<bplus_node> root;
    
    std::pair<std::shared_ptr<bplus_node>, int> insert_recurse(std::shared_ptr<bplus_node>, int);
    std::pair<bool, size_t> erase_recurse(std::shared_ptr<bplus_node>, int);
    bool search_recurse(std::shared_ptr<bplus_node>, int);
    
public:
    void insert(int);
    void erase(int);
    bool search(int);
    void print();
};


#endif /* bplus_tree_h */
