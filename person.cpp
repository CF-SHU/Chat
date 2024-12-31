#include "person.h"
#include "chat.h"
//引用CreateTableForChat(c,naem,id);to read()
#include <fstream>
#include <iostream>
#include <sstream>
#include <pqxx/pqxx>

extern std::vector<Person> chat;

Person::Person(std::string name, std::string id) : m_name{name}, m_id{id} {}

//在chat中找到某个人,判断提供的人名是否在chat中
bool Person::sameName(std::string l_name)
{
    if (this->m_name == l_name) {
        return true;
    } else
        return false;
}
//返回id，在chat中操作
std::string Person::returnId(std::string f_name)
{
    for (auto &a : chat) {
        if (a.sameName(f_name)) { return a.m_id; }
    }
}
//添加朋友（从文件读入时）
void Person::addFriends(std::string l_name, std::string l_id)
{
    _friends.emplace_back(l_name, l_id);
}
//输出用户信息
void Person::output()
{
    std::cout << m_name << " " << m_id << std::endl;
}
//将数据库中的好友信息写入类
void Person::writefriends(std::string fri)
{
    std::istringstream iss{fri};
    std::string friendname;
    while (true) {
        iss >> friendname;
        if (friendname.size() == 0)
            return;
        for (auto &a : chat) {
            if (a.sameName(friendname))
                this->_friends.emplace_back(friendname, a.m_id);
        }
    }
}
//返回类中好友的名字
std::string Person::returnfriendname()
{
    std::string frin;
    for (auto &k : this->_friends) {
        frin = frin + k.m_name;
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
//将用户的id和name连成字符串
std::string Person::to_string()
{
    std::string inf = m_id + " " + m_name;
    return inf;
}
//friends表？
void Person::dataInter(pqxx::connection &c)
{
    for (auto &a : this->_friends) {
        std::string sql = "insert into " + this->m_name + "friends(friend) values('" + a.m_name + "');";
        pqxx::work w(c);
        w.exec(sql);
        w.commit();
    }
}
//覆盖好友表中的内容
void Person::friendcover(pqxx::connection &c)
{
    for (auto &t : this->_friends) {
        std::string del = "truncate table " + t.m_name + ";"; //"friends;";
        pqxx::work w(c);
        w.exec(del);
        w.commit();
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
//getter
std::string Person::returnname()
{
    return m_name;
}
//从已有文件中读取数据到类
void read(pqxx::connection &c)
{
    std::ifstream ifs("../../data.dat");
    std::string line;
    while (getline(ifs, line)) {
        if (line.empty()) { break; }
        std::istringstream iss{line};
        std::string l_id;
        iss >> l_id;
        std::string l_name, f_name;
        iss >> l_name;
        chat.emplace_back(l_name, l_id);
        //用户注册，创建用户用于聊天的表
        CreateTableForChat(c, l_name, l_id);
    }
    while (getline(ifs, line)) {
        std::istringstream iss{line};
        std::string l_name;
        std::string li;
        iss >> l_name;
        for (auto &n : chat) {
            if (n.sameName(l_name)) {
                while (getline(ifs, li)) {
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
// //输出用户的朋友信息到文件
// void Person::tooFriend()
// {
//     if (this->_friends.size() != 0) {
//         std::ofstream ofs("../../l.dat", std::ios_base::app);
//         ofs << " " << std::endl;
//         ofs << this->m_name << " friends" << std::endl;
//         for (auto &t : this->_friends) {
//             ofs << t.m_name << std::endl;
//         }
//         ofs.close();
//     }
// }
// //输出用户信息，依次输出id,name,friends
// void write()
// {
//     std::ofstream ofs("../../l.dat");
//     for (auto &g : chat) {
//         std::string ni = g.to_string();
//         ofs << ni << std::endl;
//     }
//     ofs.close();
//     for (auto &e : chat) {
//         e.tooFriend();
//     }
// }
