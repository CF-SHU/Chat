#include "netizen.h"
#include "group.h"
#include <string>
using std::string;
#include <iostream>
#include <vector>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;

extern std::vector<Netizen> netizens;
extern std::vector<Group> groups;

//构造函数
Group::Group(string name) : m_name{name} {}

//添加某个特定群聊的群成员
bool Group::addNetizen(string groupname, Netizen *n)
{
    if (m_name == groupname) {
        _netizens.emplace_back(n);
        return true;
    }
    return false;
}

//判断是否有这个群聊名
bool Group::hasGroup(string groupname)
{
    return groupname == m_name;
}

//输出群聊的群成员名
void Group::checkName()
{
    cout << this->m_name << endl;
}
//判断是否存在这个群聊,如果存在就返回true并添加该群成员
bool Group::Judge(string groupname, Netizen *n)
{
    for (auto &g : groups) {
        if (g.hasGroup(groupname)) {
            _netizens.push_back(n);
            return true;
        }
    }
    return false;
}
