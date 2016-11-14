//
//  main.cpp
//  bplus_tree
//
//  Created by Shubham Chaudhary on 26/10/16.
//  Copyright © 2016 Shubham Chaudhary. All rights reserved.
//

#include <iostream>

#include "bplus_tree.hpp"

int main(void) {
    bplus_tree tree;
    size_t n;
    int temp;
    
    std::cout << "Input Num : "; std::cin >> n;
    for (size_t i = 0; i < n; i++) {
        std::cout << "Input : "; std::cin >> temp;

        tree.insert(temp);
        std::cout << "Tree after insertion: \n"; tree.print();
    }
    
    for (size_t i = 0; i < n; i++) {
        std::cout << "Input : "; std::cin >> temp;
        
        tree.erase(temp);
        std::cout << "Tree after erase: \n"; tree.print();
    }

    
    return 0;
}
