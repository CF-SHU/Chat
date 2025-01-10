#pragma once
#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "group.h"

class Group;
//"group.h" 有了还要声明？

class Netizen
{
public:
    Netizen(std::string id, std::string name);

    void watchNotRead(pqxx::connection &A, std::string tablename);
    void conversation(pqxx::connection &A, std::string friendId);
    void insertInformation(pqxx::connection &A, std::string tablename, std::string information);
    void createGroup(pqxx::connection &A); //创建群聊
    void extracted(Group &g);
    void joinGroup(pqxx::connection &A); // 加入群聊
    void extracted(pqxx::result &res);
    void checkGroups(pqxx::connection &A); // 查看群聊
    void chooseGroupToChat(pqxx::connection &A);
    void chat(pqxx::connection &A, std::string groupname);
    bool sameName(std::string l_name);
    std::string returnId(std::string f_name);
    void addFriends(std::string l_name, std::string l_id);

private:
    std::string m_id;
    std::string m_name;
    std::vector<Group *> _groups; //用于存储用户加入过的群聊名
};
