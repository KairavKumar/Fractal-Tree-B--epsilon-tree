#include "be_tree.hpp"
#include <iostream>
#include <string>

int main() {
    BeTree tree;
    std::string command;
    uint32_t key, val;

    std::cout << "B-epsilon Tree Interactive Mode\n";
    std::cout << "Commands: insert, update, delete, query, exit\n";

    while (true) {
        std::cout << "> ";
        std::cin >> command;

        if (command == "insert") {
            std::cout << "key: ";
            std::cin >> key;
            std::cout << "value: ";
            std::cin >> val;
            tree.Insert(key, val);
            std::cout << "Inserted (" << key << ", " << val << ")\n";
        } else if (command == "update") {
            std::cout << "key: ";
            std::cin >> key;
            std::cout << "value: ";
            std::cin >> val;
            uint32_t old_val = tree.Query(key);
            if (old_val == KEY_NOT_FOUND)
                std::cout << "Key " << key << " not found\n";
            else{
                tree.Update(key, val);
                std::cout << "Updated (" << key << ", " << val << ")\n";
            }
        } else if (command == "delete") {
            std::cout << "key: ";
            std::cin >> key;
            uint32_t old_val = tree.Query(key);
            if (old_val == KEY_NOT_FOUND)
                std::cout << "Key " << key << " not found\n";
            else{
                tree.Delete(key);
                std::cout << "Deleted key " << key << "\n";
            }
        } else if (command == "query") {
            std::cout << "key: ";
            std::cin >> key;
            val = tree.Query(key);
            if (val == KEY_NOT_FOUND)
                std::cout << "Key " << key << " not found\n";
            else
                std::cout << "Key " << key << " has value " << val << "\n";
        } else if (command == "exit") {
            break;
        } else {
            std::cout << "Unknown command. Try: insert, update, delete, query, exit\n";
        }
    }
    return 0;
}
