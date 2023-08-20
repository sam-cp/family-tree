#ifndef FAMILYTREE_HPP
#define FAMILYTREE_HPP

#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>

class FamilyTree {
    public:
        enum Gender {
            MALE, FEMALE
        };
        struct Member {
            std::string name = "";
            Gender gender = MALE;
            int father = 0;
            int mother = 0;
        };
    private:
        struct MapValue {
            Member member = Member();
            std::unordered_set<int> children = std::unordered_set<int>();
        };
        std::unordered_map<int, MapValue> members;
        std::priority_queue<int, std::vector<int>, std::greater<int>> pq;

        void get_ancestors(int id, std::unordered_map<int, int>& distances, int depth = 0) const;
    public:
        FamilyTree();

        [[nodiscard]] int find_member(std::string name) const;
        [[nodiscard]] bool member_exists(int id) const;
        [[nodiscard]] FamilyTree::Member get_member(int id) const;
        [[nodiscard]] const std::unordered_set<int>& get_children(int id) const;
        [[nodiscard]] std::string get_relationship(int subject, int object) const;
        [[nodiscard]] std::vector<std::pair<int, FamilyTree::Member>> list_members() const;

        void store_to_file(std::string filename) const;

        void read_from_file(std::string filename);
        int add_member(std::string name, Gender gender, int father = 0, int mother = 0);
        void set_name(int id, std::string name);
        void connect_parent(int child, int parent);
        void disconnect_father(int id);
        void disconnect_mother(int id);
        void disconnect_children(int id);
        void remove_member(int id);
        void clear();
        
};

#endif  // defined(FAMILYTREE_HPP)