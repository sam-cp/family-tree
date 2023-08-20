#include "familytree.hpp"
#include <stdexcept>
#include <cstdio>
#include <limits>

void FamilyTree::get_ancestors(int id, std::unordered_map<int, int>& distances, int depth) const {
    if (this->members.at(id).member.father) {
        this->get_ancestors(this->members.at(id).member.father, distances, depth + 1);
    }
    if (this->members.at(id).member.mother) {
        this->get_ancestors(this->members.at(id).member.mother, distances, depth + 1);
    }
    distances[id] = depth;
}

FamilyTree::FamilyTree()
    :members(), pq() {
    this->pq.push(1);
}

int FamilyTree::find_member(std::string name) const {
    for (auto [key, value] : this->members) {
        if (value.member.name == name) {
            return key;
        }
    }
    return 0;
}

bool FamilyTree::member_exists(int id) const {
    return this->members.find(id) != this->members.end();
}

FamilyTree::Member FamilyTree::get_member(int id) const {
    auto found = this->members.find(id);
    if (found == this->members.end()) {
        throw std::invalid_argument("The given ID does not match a member of the family tree.");
    }
    return found->second.member;
}

const std::unordered_set<int>& FamilyTree::get_children(int id) const {
    auto found = this->members.find(id);
    if (found == this->members.end()) {
        throw std::invalid_argument("The given ID does not match a member of the family tree.");
    }
    return found->second.children;
}

std::string FamilyTree::get_relationship(int subject, int object) const {
    if (!this->member_exists(subject) || !this->member_exists(object)) {
        throw std::invalid_argument("One of the given IDs does not exist.");
    }

    // Get subject's ancestors
    std::unordered_map<int, int> subject_ancestors, object_ancestors;
    this->get_ancestors(subject, subject_ancestors);
    this->get_ancestors(object, object_ancestors);
    
    int min_ancestor = 0;
    int min_distance = std::numeric_limits<int>::max();
    for (auto [ancestor, distance] : subject_ancestors) {
        auto found = object_ancestors.find(ancestor);
        if (found != object_ancestors.end() && distance < min_distance) {
            min_ancestor = ancestor;
            min_distance = distance;
        }
    }
    if (min_ancestor == 0) {
        throw std::runtime_error("No common ancestor exists.");
    }
    int min = std::min(subject_ancestors.at(min_ancestor), object_ancestors.at(min_ancestor));
    int diff = std::abs(object_ancestors.at(min_ancestor) - subject_ancestors.at(min_ancestor));
    bool obj_lower = object_ancestors.at(min_ancestor) > subject_ancestors.at(min_ancestor);
    std::string result;
    if (min == 0) {
        if (diff == 0) {
            result = "self";
        } else {
            while (diff > 2) {
                result += "great-";
                diff -= 1;
            }
            if (diff == 2) {
                result += "grand";
            }
            switch (this->members.at(object).member.gender) {
                case MALE:
                    result += obj_lower ? "son" : "father";
                    break;
                case FEMALE:
                    result += obj_lower ? "daughter" : "mother";
                    break;
            }
        }
    } else if (min == 1) {
        if (diff == 0) {
            if (!this->members.at(subject).member.father || !this->members.at(object).member.mother
                || this->members.at(subject).member.father != this->members.at(object).member.father
                || this->members.at(subject).member.mother != this->members.at(object).member.mother) {
                result = "half-";
            }
            switch (this->members.at(object).member.gender) {
                case MALE:
                    result += "brother";
                    break;
                case FEMALE:
                    result += "sister";
                    break;
            }
        } else {
            while (diff > 1) {
                result += "great-";
                diff -= 1;
            }
            switch (this->members.at(object).member.gender) {
                case MALE:
                    result += obj_lower ? "nephew" : "uncle";
                    break;
                case FEMALE:
                    result += obj_lower ? "niece" : "aunt";
                    break;
            }
        }
    } else {
        if (min > 2) {
            result = std::to_string(min - 1);
            if (((min - 1) / 10) % 10 == 1) {
                result += "th ";
            } else {
                switch ((min - 1) % 10) {
                    case 1:
                        result += "st ";
                        break;
                    case 2:
                        result += "nd ";
                        break;
                    case 3:
                        result += "rd ";
                        break;
                    default:
                        result += "th ";
                        break;
                }
            }
        }
        result += "cousin";
        if (diff > 0) {
            switch (diff) {
                case 1:
                    result += " once";
                    break;
                case 2:
                    result += " twice";
                    break;
                case 3:
                    result += " thrice";
                    break;
                default:
                    result += " " + std::to_string(diff) + "x";
                    break;
            }
            result += " removed";
        }
    }
    return result;
}

std::vector<std::pair<int, FamilyTree::Member>> FamilyTree::list_members() const {
    std::vector<std::pair<int, FamilyTree::Member>> v;
    std::priority_queue<int, std::vector<int>, std::greater<int>> order_pq;
    for (auto [key, value] : this->members) {
        order_pq.push(key);
    }
    while (!order_pq.empty()) {
        v.push_back({order_pq.top(), this->members.at(order_pq.top()).member});
        order_pq.pop();
    }
    return v;
}

void FamilyTree::store_to_file(std::string filename) const {
    // Topologically sort the members
    std::unordered_map<int, int> indegrees;
    std::queue<int> q;
    for (auto [key, value] : this->members) {
        indegrees[key] = 0;
        if (value.member.father) {
            ++indegrees[key];
        }
        if (value.member.mother) {
            ++indegrees[key];
        }
        if (indegrees[key] == 0) {
            q.push(key);
        }
    }
    std::vector<int> v;
    std::unordered_map<int, int> map;
    while (!q.empty()) {
        int id = q.front();
        q.pop();
        map[id] = v.size() + 1;
        v.push_back(id);
        for (auto child : this->members.at(id).children) {
            --indegrees[child];
            if (indegrees[child] == 0) {
                q.push(child);
            }
        }
    }
    map[0] = 0;

    // Store them in topological order
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        throw std::invalid_argument("Invalid file: cannot be opened for writing.");
    }
    for (int i : v) {
        const Member& m = this->members.at(i).member;
        fwrite(m.name.c_str(), 1, m.name.size() + 1, file);
        switch(m.gender) {
            case MALE:
                fputc(0, file);
                break;
            case FEMALE:
                fputc(1, file);
                break;
        }
        int parents[2] = {map[m.father], map[m.mother]};
        fwrite(parents, 4, 2, file);
    }
    fclose(file);
}

void FamilyTree::read_from_file(std::string filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        throw std::invalid_argument("The specified file does not exist.");
    }
    while (true) {
        Member member;
        // read name
        char c;
        while ((c = fgetc(file)) && !feof(file)) {
            member.name.push_back(c);
        }
        if (feof(file)) {
            if (member.name.empty()) {
                break;
            } else {
                throw std::invalid_argument("File is in invalid format");
            }
        }

        c = fgetc(file);
        if (feof(file)) {
            throw std::invalid_argument("File is in invalid format");
        }
        switch(c) {
            case 0:
                member.gender = MALE;
                break;
            case 1:
                member.gender = FEMALE;
                break;
        }

        fread(&member.father, 4, 2, file);
        if (feof(file)) {
            throw std::invalid_argument("File is in invalid format");
        }

        this->add_member(member.name, member.gender, member.father, member.mother);
    }
}

int FamilyTree::add_member(std::string name, Gender gender, int father, int mother) {
    if (father && this->get_member(father).gender != MALE) {
        throw std::invalid_argument("The father must be male.");
    }
    if (mother && this->get_member(mother).gender != FEMALE) {
        throw std::invalid_argument("The mother must be female.");
    }

    int id = 0;
    id = this->pq.top();
    this->pq.pop();
    if (this->pq.empty()) {
        this->pq.push(id + 1);
    }
    Member member = {name, gender, father, mother};
    FamilyTree::MapValue pair = {std::move(member), std::unordered_set<int>()};
    this->members[id] = std::move(pair);
    if (father) {
        this->members.at(father).children.insert(id);
    }
    if (mother) {
        this->members.at(mother).children.insert(id);
    }
    return id;
}

void FamilyTree::set_name(int id, std::string name) {
    if (!this->member_exists(id)) {
        throw std::invalid_argument("No member with the given ID exists.");
    }
    this->members[id].member.name = name;
}

void FamilyTree::connect_parent(int child, int parent) {
    if (!this->member_exists(child)) {
        std::invalid_argument("The child does not exist.");
    }
    switch (this->get_member(parent).gender) {
        case MALE:
            this->disconnect_father(child);
            this->members[child].member.father = parent;
            break;
        case FEMALE:
            this->disconnect_mother(child);
            this->members[child].member.mother = parent;
            break;
    }
    this->members[parent].children.insert(child);
}

void FamilyTree::disconnect_father(int id) {
    if (!this->member_exists(id)) {
        std::invalid_argument("The child does not exist.");
    }
    int father = this->members[id].member.father;
    this->members[id].member.father = 0;
    if (father) {
        this->members[father].children.erase(id);
    }
}

void FamilyTree::disconnect_mother(int id) {
    if (!this->member_exists(id)) {
        std::invalid_argument("The child does not exist.");
    }
    int mother = this->members[id].member.mother;
    this->members[id].member.mother = 0;
    if (mother) {
        this->members[mother].children.erase(id);
    }
}

void FamilyTree::disconnect_children(int id) {
    if (!this->member_exists(id)) {
        std::invalid_argument("The parent does not exist.");
    }
    std::unordered_set<int> children_copy = this->members[id].children;
    for (int child : children_copy) {
        switch (this->members[id].member.gender) {
            case MALE:
                disconnect_father(child);
                break;
            case FEMALE:
                disconnect_mother(child);
                break;
        }
    }
}

void FamilyTree::remove_member(int id) {
    this->disconnect_children(id);
    this->disconnect_father(id);
    this->disconnect_mother(id);
    this->members.erase(id);
    pq.push(id);
}

void FamilyTree::clear() {
    this->members.clear();
    this->pq = std::priority_queue<int, std::vector<int>, std::greater<int>>();
}