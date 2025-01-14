//网民类
#pragma once
#include <string>
#include <vector>
#include <pqxx/pqxx>
#include "group.h"

//"group.h" 有了还要声明？
class Group;

class Netizen
{
public:
    Netizen(std::string name, std::string id);

    void createGroup(pqxx::connection &A); //创建群聊
    void joinGroup(pqxx::connection &A); // 加入群聊
    void checkGroups(pqxx::connection &A); // 查看群聊
    void chooseGroupToChat(pqxx::connection &A);                   //选择群聊去聊天
    void watchNotRead(pqxx::connection &A, std::string tablename); //查看未读信息
    void chat(pqxx::connection &A, std::string groupname);         //聊天
    void insertInformation(pqxx::connection &A, std::string tablename, std::string information);

    bool sameName(std::string l_name); //在netizens中找到某个人,判断提供的人名是否在netizens中
    void output();                     //输出用户信息
    void outputFriends();              //输出用户好友信息，从类中操作

    void deleteFriends(std::string friendname); //删好友,从类中操作

    void addFriends(std::string l_name, std::string l_id); //添加朋友（从文件读入时）
    void addFriend(std::string b_name);                    //加好友，在netizens数组中操作
    void writefriends(std::string fri); //将数据库中的好友信息写入类，用于dataRead函数

    std::string returnname();                 //getter m_name
    std::string returnId(std::string f_name); //如果名字存在，返回id，在netizens中操作
    std::string returnfriendname();           //返回netizens中好友的名字

    std::string to_string();             //将用户的id和name连成字符串
private:
    std::string m_id;
    std::string m_name;
    std::vector<Netizen> _friends; //用于存储用户的朋友信息
    std::vector<Group *> _groups; //用于存储用户加入过的群聊名
};
void read(pqxx::connection &c);
