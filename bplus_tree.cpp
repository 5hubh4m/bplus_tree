//
//  bplus_tree.cpp
//  bplus_tree
//
//  Created by Shubham Chaudhary on 26/10/16.
//  Copyright © 2016 Shubham Chaudhary. All rights reserved.
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <memory>

#include "bplus_tree.hpp"

void bplus_tree::insert(int key) {
    if (root == nullptr) {
        root = std::make_shared<bplus_node>();
        root->keys.push_back(key);
    }
    else {
        auto p = insert_recurse(root, key);
        
        if (!(p.first == nullptr)) {
            auto new_root = std::make_shared<bplus_node>();
            
            new_root->keys.push_back(p.second);
            new_root->children.push_back(root);
            new_root->children.push_back(p.first);
            
            root = new_root;
        }
    }
}

std::pair<std::shared_ptr<bplus_node>, int>
bplus_tree::insert_recurse(std::shared_ptr<bplus_node> temp, int key) {
    size_t i;
    for (i = 0; i < temp->keys.size(); i++)
        if (key <= temp->keys[i])
            break;
    
#ifdef DEBUG
    std::cout << "temp: ";
    for (auto a : temp->keys) {
        std::cout << a << " ";
    } std::cout << "\n";
#endif
    
    
    if (temp->is_leaf()) {
        temp->keys.insert(temp->keys.begin() + i, key);
        
        // If no split
        if (temp->keys.size() <= b)
            return std::make_pair(nullptr, 0);
        
        // Else if a split is performed
        else {
            auto new_leaf = std::make_shared<bplus_node>();
            
            size_t m = ((b + 1) / 2);
            m += (b % 2 == 0 ? 1 : 0);
            
            new_leaf->keys.insert(new_leaf->keys.begin(),
                                  temp->keys.begin() + m,
                                  temp->keys.end());
            
            temp->next = new_leaf;
            temp->keys.erase(temp->keys.begin() + m, temp->keys.end());
            
#ifdef DEBUG
            std::cout << "temp: ";
            for (auto a : temp->keys) {
                std::cout << a << " ";
            } std::cout << "\n";
            
            std::cout << "new_leaf: ";
            for (auto a : new_leaf->keys) {
                std::cout << a << " ";
            } std::cout << "\n";
            
            std::cout << "end: " << temp->keys[temp->keys.size() - 1] << "\n";
#endif
            
            return std::make_pair(new_leaf, temp->keys[temp->keys.size() - 1]);
        }
    }
    else {
        auto p = insert_recurse(temp->children[i], key);
        if (p.first == nullptr)
            return std::make_pair(nullptr, 0);
        
        else {
            size_t j;
            for (j = 0; j < temp->keys.size(); j++)
                if (p.second <= temp->keys[j])
                    break;
            
            temp->keys.insert(temp->keys.begin() + j, p.second);
            temp->children.insert(temp->children.begin() + j + 1, p.first);
            
            // If no split
            if (temp->keys.size() <= b)
                return std::make_pair(nullptr, 0);
            
            // Else if a split is performed
            else {
                auto new_leaf = std::make_shared<bplus_node>();
                
                size_t m = ((b + 1) / 2) + 1;
                
                new_leaf->keys.insert(new_leaf->keys.begin(),
                                      temp->keys.begin() + m,
                                      temp->keys.end());
                new_leaf->children.insert(new_leaf->children.begin(),
                                          temp->children.begin() + m,
                                          temp->children.end());
                
                temp->keys.erase(temp->keys.begin() + m, temp->keys.end());
                temp->children.erase(temp->children.begin() + m, temp->children.end());
                
                int up_key = temp->keys[temp->keys.size() - 1];
                
                temp->keys.erase(temp->keys.begin () + temp->keys.size() - 1);
                
                return std::make_pair(new_leaf, up_key);
            }
        }
    }
    return std::make_pair(nullptr, 0);
}

std::pair<bool, size_t>
bplus_tree::erase_recurse(std::shared_ptr<bplus_node> temp, int key) {
    size_t i;
    size_t m = (b + 1) / 2;
    
    for (i = 0; i < temp->keys.size(); i++)
        if (key <= temp->keys[i])
            break;
    if (temp->is_leaf()) {
        if (temp->keys[i] == key) {
            if (temp->keys.size() >= m + 1) {
                temp->keys.erase(temp->keys.begin() + i);
            }
            else
                return std::make_pair(ACTION, i);
        }
    }
    else {
        auto p = erase_recurse(temp->children[i], key);
        
        if (p.first == ACTION) {
            if (temp->children[i]->keys.size() >= m + 1)
                temp->children[i]->keys.erase(temp->children[i]->keys.begin() + p.second);

            else if (temp->children[i]->is_leaf()) {
                auto idx = p.second;
                
                temp->children[i]->keys.erase(temp->children[i]->keys.begin() + idx);
                
                size_t s = temp->children[i]->keys.size(),
                l_s = (i == 0 ? 0 : temp->children[i - 1]->keys.size()),
                r_s = (i == temp->children.size() - 1 ? 0 : temp->children[i + 1]->keys.size());
                
                if (l_s >= m + 1) {
                    auto t = temp->children[i],
                         t1 = temp->children[i - 1];
                    
                    t->keys.insert(t->keys.begin(), t1->keys[t1->keys.size() - 1]);
                    t1->keys.erase(t1->keys.begin() + t1->keys.size() - 1);
                }
                else if (r_s >= m + 1) {
                    auto t = temp->children[i],
                         t1 = temp->children[i + 1];
                    
                    t->keys.insert(t->keys.end(), t1->keys[0]);
                    t1->keys.erase(t1->keys.begin());
                }
                else if (s + l_s >= m && s + l_s <= b) {
                    auto t = temp->children[i],
                         t1 = temp->children[i - 1];
                    
                    t1->keys.insert(t1->keys.end(), t->keys.begin(), t->keys.end());
                    
                    temp->children.erase(temp->children.begin() + i);
                    
                    return std::make_pair(ACTION, i);
                }
                else {
                    auto t = temp->children[i],
                         t1 = temp->children[i + 1];
                    
                    t1->keys.insert(t1->keys.begin(), t->keys.begin(), t->keys.end());
                    
                    temp->children.erase(temp->children.begin() + i);
                    
                    return std::make_pair(ACTION, i);
                }
            }
            else {
                auto idx = p.second;
                
                temp->children[i]->keys.erase(temp->children[i]->keys.begin() + idx);
                
                size_t s = temp->children[i]->keys.size(),
                l_s = (i == 0 ? 0 : temp->children[i - 1]->keys.size()),
                r_s = (i == temp->children.size() - 1 ? 0 : temp->children[i + 1]->keys.size());
                
                if (l_s >= m + 1) {
                    auto t = temp->children[i],
                    t1 = temp->children[i - 1];
                    
                    t->keys.insert(t->keys.begin(), t1->keys[t1->keys.size() - 1]);
                    t->children.insert(t->children.begin(), t1->children[t1->children.size() - 1]);
                    
                    t1->keys.erase(t1->keys.begin() + t1->keys.size() - 1);
                    t1->children.erase(t1->children.begin() + t1->children.size() - 1);
                }
                else if (r_s >= m + 1) {
                    auto t = temp->children[i],
                    t1 = temp->children[i + 1];
                    
                    t->keys.insert(t->keys.end(), t1->keys[0]);
                    t->children.insert(t->children.end(), t1->children[0]);
                    
                    t1->keys.erase(t1->keys.begin());
                    t1->children.erase(t1->children.begin());
                }
                else if (s + l_s >= m && s + l_s <= b) {
                    auto t = temp->children[i],
                    t1 = temp->children[i - 1];
                    
                    if (s == 0) {
                        temp->children[i]->keys.push_back(temp->keys[i]);
                    }
                    
                    t1->keys.insert(t1->keys.end(), t->keys.begin(), t->keys.end());
                    t1->children.insert(t1->children.end(), t->children.begin(), t->children.end());
                    
                    temp->children.erase(temp->children.begin() + i);
                    
                    return std::make_pair(ACTION, i);
                }
                else {
                    auto t = temp->children[i],
                    t1 = temp->children[i + 1];
                    
                    if (s == 0) {
                        temp->children[i]->keys.push_back(temp->keys[i]);
                    }
                    
                    t1->keys.insert(t1->keys.begin(), t->keys.begin(), t->keys.end());
                    t1->children.insert(t1->children.begin(), t->children.begin(), t->children.end());
                    
                    temp->children.erase(temp->children.begin() + i);
                    
                    return std::make_pair(ACTION, i);
                }
            }
        }
    }

    return std::make_pair(NO_ACTION, 0);
}

void bplus_tree::erase(int key) {
    if (root == nullptr)
        return;
    
    auto p = erase_recurse(root, key);
    
    if (p.first == ACTION) {
        if (root->keys.size() <= 1) {
            root = *(root -> children.begin());
        }
    }
}

void bplus_tree::print(void) {
    if (root == nullptr) {
        std::cout << "Empty tree.\n";
        return;
    }
    
    size_t size = 1;
    std::queue<std::shared_ptr<bplus_node>> nodes;
    nodes.push(root);
    
    while (!nodes.empty()) {
        size_t s = size;
        size = 0;
        for (size_t i = 0; i < s; i++) {
            auto temp = nodes.front();
            nodes.pop();
            
            for (auto& a : temp->children) {
                nodes.push(a);
            }
            size += temp->children.size();
            
            for(auto& a : temp->keys) {
                std::cout << std::setw(5) << a << ",";
            } std::cout << "->";
            
        } std::cout << "\n";
    }
}

bool bplus_tree::search_recurse(std::shared_ptr<bplus_node> temp, int key) {
    if (temp == nullptr)
        return false;
    
    size_t i = 0;
    for (i = 0; i < temp->keys.size(); i++)
        if (key <= temp->keys[i])
            break;
    
    if (temp->is_leaf()) {
        if (key == temp->keys[i])
            return true;
        else
            return false;
    }
    else
        return search_recurse(temp->children[i], key);
    
    return false;
}

bool bplus_tree::search(int key) {
    return search_recurse(root, key);
}
