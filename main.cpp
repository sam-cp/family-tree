#include "familytree.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

int main(int argc, char* argv[]) {
    bool changes_made = false;
    std::string overall_filename;

    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [filename]" << std::endl;
        return EXIT_FAILURE;
    }
    FamilyTree ft;
    if (argc == 2) {
        try {
            ft.read_from_file(argv[1]);
        } catch (const std::exception& err) {
            std::cerr << err.what() << std::endl;
            return 1;
        }
        overall_filename = argv[1];
    }

    bool end_loop = false;
    do {
        bool get_remainder = true;
        std::cerr << ">>> " << std::flush;
        std::string cmd;
        std::cin >> cmd;

        if (cmd == "list_members") {
            for (auto [id, member] : ft.list_members()) {
                std::cout << std::setw(10) << id << " ... " << member.name << std::endl;
            }
        } else if (cmd == "member_info") {
            int id;
            std::cin >> id;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            try {
                FamilyTree::Member member = ft.get_member(id);
                std::cout << "    Name: " << member.name << std::endl;
                std::cout << "  Gender: " << ((member.gender == FamilyTree::Gender::MALE) ? "Male" : "Female") << std::endl;
                if (member.father) {
                    std::cout << "  Father: " << ft.get_member(member.father).name << " (" << member.father << ")" << std::endl;
                }
                if (member.mother) {
                    std::cout << "  Mother: " << ft.get_member(member.mother).name << " (" << member.mother << ")" << std::endl;
                }
                const std::unordered_set<int>& children = ft.get_children(id);
                if (!children.empty()) {
                    std::cout << "Children:" << std::endl;
                    for (int child : children) {
                        std::cout << "\t" << ft.get_member(child).name << " (" << child << ")" << std::endl;
                    }
                }
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "find_member") {
            while (isspace(std::cin.peek())) {
                std::cin.get();
            }
            std::string name;
            getline(std::cin, name);
            get_remainder = false;
            int id = ft.find_member(name);
            if (id) {
                std::cout << "The ID of " << name << " is " << id << "." << std::endl;
            } else {
                std::cout << "No member of the name \"" << name << "\" was found." << std::endl;
            }
        } else if (cmd == "add_member") {
            char genderchar;
            FamilyTree::Gender gender;
            std::cin >> genderchar;
            switch(genderchar) {
                case 'M':
                case 'm':
                    gender = FamilyTree::Gender::MALE;
                    break;
                case 'F':
                case 'f':
                    gender = FamilyTree::Gender::FEMALE;
                    break;
                default:
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cerr << "Invalid gender: must be 'M' or 'F'." << std::endl;
                    continue;
                    
            }
            int father, mother;
            std::cin >> father >> mother;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max());
                std::cerr << "Invalid ID for parents." << std::endl;
                continue;
            }
            while (isspace(std::cin.peek())) {
                std::cin.get();
            }
            std::string name;
            getline(std::cin, name);
            get_remainder = false;

            try {
                int id = ft.add_member(name, gender, father, mother);
                changes_made = true;
                std::cerr << "\"" << name << "\" added, with ID " << id << "." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "set_name") {
            int member;
            std::cin >> member;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            while (isspace(std::cin.peek())) {
                std::cin.get();
            }
            std::string name;
            getline(std::cin, name);
            get_remainder = false;

            try {
                ft.set_name(member, name);
                changes_made = true;
                std::cerr << "The name of member " << member << " was changed to \"" << name << "\"." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "connect_parent") {
            int child, parent;
            std::cin >> child >> parent;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            try {
                ft.connect_parent(child, parent);
                changes_made = true;
                FamilyTree::Member parent_member = ft.get_member(parent);
                std::cerr << "The " << (parent_member.gender == FamilyTree::Gender::MALE ? "father" : "mother") << " of " << ft.get_member(child).name << " is now " << parent_member.name << "." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "disconnect_father") {
            int child;
            std::cin >> child;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            try {
                ft.disconnect_father(child);
                changes_made = true;
                std::cerr << "The father of " << ft.get_member(child).name << " is no longer listed." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "update_mother") {
            int child;
            std::cin >> child;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            try {
                ft.disconnect_mother(child);
                changes_made = true;
                std::cerr << "The mother of " << ft.get_member(child).name << " is no longer listed." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "remove_member") {
            int member;
            std::cin >> member;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            try {
                std::string name = ft.get_member(member).name;
                ft.remove_member(member);
                changes_made = true;
                std::cerr << name << " has been removed." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "get_relationship") {
            int subject, object;
            std::cin >> subject >> object;
            if (!std::cin.good()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cerr << "Invalid ID." << std::endl;
                continue;
            }
            try {
                std::string relationship = ft.get_relationship(subject, object);
                std::cout << ft.get_member(object).name << " is the " << relationship << " of " << ft.get_member(subject).name << "." << std::endl;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "read_from_file") {
            while (isspace(std::cin.peek())) {
                std::cin.get();
            }
            std::string filename;
            getline(std::cin, filename);

            if (changes_made) {
                std::cout << "You have made changes. Are you sure you want to discard them? (y/N) >> " << std::flush;
                char choice;
                std::cin >> choice;
                if (choice != 'Y' && choice != 'y') {
                    std::cerr << "Cancelling..." << std::endl;
                    continue;
                }
            }

            try {
                ft.read_from_file(filename);
                changes_made = false;
                overall_filename = filename;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "store_to_file") {
            while (isspace(std::cin.peek())) {
                std::cin.get();
            }
            std::string filename;
            getline(std::cin, filename);
            try {
                ft.store_to_file(filename);
                overall_filename = filename;
                changes_made = false;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "save") {
            if (overall_filename.empty()) {
                std::cerr << "No filename given. Cancelling..." << std::endl;
                continue;
            }
            try {
                ft.store_to_file(overall_filename);
                changes_made = false;
            } catch (const std::exception& err) {
                std::cerr << err.what() << std::endl;
            }
        } else if (cmd == "exit") {
            if (changes_made) {
                std::cout << "You have made changes. Are you sure you want to discard them? (y/N) >> " << std::flush;
                char choice;
                std::cin >> choice;
                if (choice != 'Y' && choice != 'y') {
                    std::cerr << "Cancelling..." << std::endl;
                    continue;
                }
            }

            end_loop = true;
        } else {
            std::cout << "This function is not supported." << std::endl;
        }
        if (get_remainder) {
            std::string remainder;
            getline(std::cin, remainder);
        }
    } while (!end_loop);

    return EXIT_SUCCESS;
}