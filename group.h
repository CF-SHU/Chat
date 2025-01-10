#pragma once
#include "netizen.h"

class Netizen;

class Group
{
public:
    Group(std::string name);                            //构造函数
    bool addNetizen(std::string groupname, Netizen *n); //添加某个特定群聊的群成员
    bool Judge(std::string groupname, Netizen *n); //判断是否存在该群聊,存在则添加该用户为群成员
    bool hasGroup(std::string groupname);               //判断是否有这个群聊名
    void checkName();                                   //输出群聊的群成员名

private:
    std::string m_name;               //群聊名
    std::vector<Netizen *> _netizens; //用于储存一个群聊的群成员
};
