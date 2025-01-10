#include "person.h"
#include "chat.h"
#include "group.h"
#include "netizen.h"
#include <fstream>
#include <iostream>
#include <sstream>

extern std::vector<Person> chat;
extern std::vector<Group> groups;
extern std::vector<Netizen> netizens;

//构造函数
Person::Person(std::string name, std::string id) : m_name{name}, m_id{id} {}

//将数据库中的好友信息写入类，用于data表变动时程序的运行
void Person::writefriends(std::string fri)
{
    std::istringstream iss{fri};
    std::string friendname;
    while (iss >> friendname) {
        for (auto &a : chat) {
            if (a.m_name == friendname) //如果chat数组中有这个好友的名字
            {
                this->_friends.emplace_back(friendname, a.m_id);
            }
        }
    }
}

//返回chat中好友的名字
std::string Person::returnfriendname()
{
    std::string frin{};
    for (auto &k : this->_friends) {
        frin = frin + k.m_name + " ";
    }
    return frin;
}

//输出用户好友信息，从类中操作
void Person::outputFriends()
{
    if (_friends.size() != 0) {
        std::cout << this->m_name << " friends:" << '\n';
        for (auto &d : _friends) {
            std::cout << d.m_name << std::endl;
        }
        std::cout << '\n';
    }
}

//加好友，在chat数组中操作
void Person::addFriend(std::string b_name)
{
    for (auto &b : chat) {
        if (b.m_name == b_name) {
            this->_friends.emplace_back(b.m_name, b.m_id);
            b._friends.emplace_back(this->m_name, this->m_id);
            std::cout << "Add friend successfully." << std::endl;
            break;
        }
    }
}

//删好友,从类中操作
void Person::deleteFriends(std::string friendname)
{
    int a = 0;
    for (auto &c : this->_friends) {
        if (c.sameName(friendname))
            _friends.erase(_friends.begin() + a);
        a++;
    }
}

//——————————————————————————————————————————————————————————————————————————————————————
//将用户的id和name连成字符串
std::string Person::to_string()
{
    std::string inf = m_id + " " + m_name;
    return inf;
}

//输出用户信息
void Person::output()
{
    std::cout << m_name << " " << m_id << std::endl;
}

//getter
std::string Person::returnname()
{
    return m_name;
}
//——————————————————————————————————————————————————————————————————————————————————————

//在chat中找到某个人,判断提供的人名是否在chat中
bool Person::sameName(std::string l_name)
{
    return this->m_name == l_name;
}

//添加朋友（从文件读入时）
void Person::addFriends(std::string l_name, std::string l_id)
{
    _friends.emplace_back(l_name, l_id);
}

//如果名字存在，返回id，在chat中操作
std::string Person::returnId(std::string f_name)
{
    for (auto &a : chat) {
        if (a.sameName(f_name)) {
            return a.m_id;
        }
    }
}

//从已有文件中读取数据到类
void read(pqxx::connection &c)
{
    std::ifstream ifs("../../data.dat");
    //未进行错误处理
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) { break; }
        std::istringstream iss{line};
        std::string l_id;
        iss >> l_id;
        std::string l_name;
        iss >> l_name;
        chat.emplace_back(l_name, l_id);
        netizens.emplace_back(l_name, l_id);
        //用户注册，创建用户用于聊天的表&存储用户信息和朋友的data表
        CreateTableForChat(c, l_name, l_id);
    }
    while (std::getline(ifs, line)) {
        std::istringstream iss{line};
        std::string l_name;
        std::string li;
        iss >> l_name;
        for (auto &n : chat) {
            if (n.sameName(l_name)) {
                while (std::getline(ifs, li)) {
                    if (li.empty()) break;
                    for (auto &a : chat) {
                        if (a.sameName(li)) {
                            std::string l_id = a.returnId(li);
                            n.addFriends(li, l_id);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    ifs.close();
}
