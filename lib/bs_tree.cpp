/**
   ___ _                 _                      
  / __| |__   __ _ _ __ | |_    /\/\   ___  ___ 
 / /  | '_ \ / _` | '_ \| __|  /    \ / _ \/ _ \
/ /___| | | | (_| | | | | |_  / /\/\ |  __|  __/
\____/|_| |_|\__,_|_| |_|\__| \/    \/\___|\___|

Author: Mu Xiangyu, Chant Mee 
*/

#ifndef BS_TREE_CPP
#define BS_TREE_CPP

#include "node_manager.cpp"
#include "logger.cpp"
#include <algorithm>
#include <vector>

struct treeNode {
    enum TYPE {
        FILE = 0, DIR, HEAD_NODE
    };

    TYPE type;
    int cnt;
    unsigned long long link;
    treeNode *next_brother, *first_son;

    treeNode();
    treeNode(TYPE type);
};

class BSTree {
private:
    Logger &logger = Logger::get_logger();
    NodeManager &node_manager = NodeManager::get_node_manager();
protected:
    std::vector<treeNode*> path;

    bool check_path();
    bool check_node(treeNode *p, int line);
    bool is_son();
    bool goto_tail();
    bool goto_head();
    bool name_exist(std::string name);
    bool go_to(std::string name);
    bool goto_last_dir();
    bool list_directory_contents(std::vector<std::string> &content);
    bool get_current_path(std::vector<std::string> &path);
};



                        /* ======= struct treeNode ======= */
treeNode::treeNode() = default;
treeNode::treeNode(TYPE type) {
    this->type = type;
    this->cnt = 1;
    this->next_brother = nullptr;
    this->link = -1;
    if (type == FILE || type == HEAD_NODE) this->first_son = nullptr;
    else if (type == DIR) this->first_son = new treeNode(HEAD_NODE);
}


                        /* ======= class BSTree ======= */

bool BSTree::check_path() {
    if (path.empty()) {
        logger.log("Path is empty. This not normal.", Logger::FATAL, __LINE__);
        return false;
    }
    for (auto &it : path) {
        if (it == nullptr) {
            logger.log("Null pointer exists in path. This not normal.", Logger::FATAL, __LINE__);
            return false;
        }
    }
    return true;
}

bool BSTree::check_node(treeNode *p, int line) {
    if (p == nullptr) {
        logger.log("The pointer is empty, please check whether the program is correct.", Logger::FATAL, line);
        return false;
    }
    if (p->cnt <= 0) {
        logger.log("The node counter is already less than or equal to 0, please check the program!", Logger::FATAL, line);
        return false;
    }
    return true;
}

bool BSTree::is_son() {
    if (!check_path()) return false;
    return path.back()->type == 2;
}

bool BSTree::goto_tail() {
    if (!check_path()) return false;
    while (path.back()->next_brother != nullptr) {
        path.push_back(path.back()->next_brother);
    }
    if (!check_path()) return false;
    return true;
}

bool BSTree::goto_head() {
    if (!check_path()) return false;
    for (; !path.empty() && !is_son(); path.pop_back());
    if (!check_path()) return false;
    return true;
}

bool BSTree::name_exist(std::string name) {
    std::vector<std::string> dir_content;
    if (!list_directory_contents(dir_content)) return false;
    for (auto &nm : dir_content) {
        if (nm == name) return true;
    }
    return false;
}

bool BSTree::go_to(std::string name) {
    if (!name_exist(name)) {
        logger.log("no file or directory named " + name, Logger::WARNING, __LINE__);
        return false;
    }
    if (!goto_head()) return false;
    while (node_manager.get_name(path.back()->link) != name) {
        if (path.back()->next_brother == nullptr) {
            return false;
        }
        path.push_back(path.back()->next_brother);
    }
    return true;
}

bool BSTree::goto_last_dir() {
    if (!goto_head()) return false;
    if (path.size() > 2) {
        path.pop_back();
    }
    if (!check_path()) return false;
    return true;
}

bool BSTree::list_directory_contents(std::vector<std::string> &content) {
    if (!goto_head()) return false;
    if (!check_path()) return false;
    while (path.back()->next_brother != nullptr) {
        content.push_back(node_manager.get_name(path.back()->next_brother->link));
        path.push_back(path.back()->next_brother);
    }
    return true;
}

bool BSTree::get_current_path(std::vector<std::string> &p) {
    auto path_backup = path;
    goto_head();
    while (path.size() > 2) {
        p.push_back(node_manager.get_name(path[path.size() - 2]->link));
        goto_last_dir();
        goto_head();
    }
    p.push_back(node_manager.get_name(path.front()->link));
    path = path_backup;
    std::reverse(p.begin(), p.end());
    return true;
}

#endif