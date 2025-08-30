//netizen具体函数实现以及聊天功能
#include "netizen.h"
#include "group.h"
#include "chat.h"
#include <string>
using std::string;
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
#include <ctime>
#include <fstream>
#include <sstream>

extern std::vector<Group> groups;
extern std::vector<Netizen> netizens;

//构造函数
Netizen::Netizen(string name, string id) : m_name{name}, m_id{id} {}

//将数据库中的好友信息写入类，用于data表变动时程序的运行
void Netizen::writefriends(std::string fri)
{
    std::istringstream iss{fri};
    std::string friendname;
    while (iss >> friendname) {
        for (auto &a : netizens) {
            if (a.m_name == friendname) //如果chat数组中有这个好友的名字
            {
                this->_friends.emplace_back(friendname, a.m_id);
            }
        }
    }
}

//返回netizen中好友的名字
std::string Netizen::returnfriendname()
{
    std::string frin{};
    for (auto &k : this->_friends) {
        frin = frin + k.m_name + " ";
    }
    return frin;
}

//输出用户好友信息，从类中操作
void Netizen::outputFriends()
{
    if (_friends.size() != 0) {
        std::cout << this->m_name << "的朋友有:" << '\n';
        for (auto &d : _friends) {
            std::cout << d.m_name << std::endl;
        }
        std::cout << '\n';
    } else {
        std::cout << "您现在还没有朋友！\n" << std::endl;
    }
}

//加好友，在netizen数组中操作
void Netizen::addFriend(std::string b_name)
{
    for (auto &b : netizens) {
        if (b.m_name == b_name) {
            this->_friends.emplace_back(b.m_name, b.m_id);
            b._friends.emplace_back(this->m_name, this->m_id);
            std::cout << "添加朋友成功！" << std::endl;
            break;
        }
    }
}

//添加朋友（read函数中使用，从文件读入时）
void Netizen::addFriends(std::string l_name, std::string l_id)
{
    _friends.emplace_back(l_name, l_id);
}

//删好友,从类中操作
void Netizen::deleteFriends(std::string friendname)
{
    int a = 0;
    for (auto &c : this->_friends) {
        if (c.sameName(friendname))
            _friends.erase(_friends.begin() + a);
        a++;
    }
}

//将用户的id和name连成字符串
std::string Netizen::to_string()
{
    std::string inf = m_id + " " + m_name;
    return inf;
}

//输出用户信息
void Netizen::output()
{
    std::cout << m_name << " " << m_id << std::endl;
}

//getter
std::string Netizen::returnname()
{
    return m_name;
}

//判断是否是相同的名字，用于判断用户是否存在
bool Netizen::sameName(std::string l_name)
{
    return m_name == l_name;
    //return this->m_name == l_name;
}

//返回网民的id,在netizens全局数组中操作
string Netizen::returnId(std::string f_name)
{
    for (auto &a : netizens) {
        if (a.sameName(f_name)) {
            return a.m_id;
        }
    }
}
//——————————————————————————————————————————————————————————————————————————————————————

//创建群聊聊天表
void Netizen::createGroup(pqxx::connection &A)
{
    //在数据库中创建一个表用于保存已经创建的群聊名称
    string sql1 = "CREATE TABLE IF NOT EXISTS groups (name varchar(300) NOT NULL);";
    pqxx::work W(A);
    W.exec(sql1);
    W.commit();
    cout << "请输入要创建的群聊名称:";
    string groupname;
    cin >> groupname;
    groups.push_back(groupname);
    //将群聊添加到netizen对象的_groups中
    for (auto &g : groups) {
        if (g.Judge(groupname, this)) //判断群聊名是否存在，存在就加入_groups
        {
            Group *ptr = new Group(g);
            _groups.push_back(ptr);
            //_groups.push_back(&g);
        }
    }

    // 把群聊添加到数据库中
    pqxx::work W1(A);
    string sql2 = "INSERT INTO groups (name) VALUES ('" + groupname + "')"; //将群聊加入数据库中
    W1.exec(sql2);
    W1.commit();

    //创建聊天表
    string sql3
        = "CREATE TABLE IF NOT EXISTS " + groupname
          + " (" "name varchar(100) NOT NULL,information varchar(50000) ,time " "varchar(50),flag " "varchar(10)" ");";
    pqxx::work W3(A);
    W3.exec(sql3);
    W3.commit();
}

//从数据库中查看已创建的群聊列表, 并将群名加入groups全局数组中
void Netizen::checkGroups(pqxx::connection &A)
{
    { //这一步用于确保groups表存在
        //在数据库中创建一个表用于保存已经创建的群聊名称
        string sql1 = "CREATE TABLE IF NOT EXISTS groups (name varchar(300) NOT NULL);";
        pqxx::work W(A);
        W.exec(sql1);
        W.commit();
    }
    // 查询并打印群聊列表
    string sql = "SELECT * FROM groups;";
    pqxx::work W(A);
    pqxx::result res = W.exec(sql);
    std::cout << "Chat groups list:" << std::endl;
    for (auto row : res) {
        string groupname = row["name"].c_str();
        std::cout << groupname << endl;
        groups.emplace_back(groupname);
    }
}

//加入群聊，将选择的群聊对象加入到该网民的_groups里，并将该网民加入到群聊对象的_netizens里
void Netizen::joinGroup(pqxx::connection &A)
{
    //展示群聊列表
    checkGroups(A);
    if (groups.size() == 0) {
        cout << "当前没有已创建好的群聊，正在退出加入群聊功能..." << endl;
        return;
    }
    //请选择你要加入聊天的群聊
    cout << "请输入你想加入的群聊：" << endl;
    string groupname;
    cin >> groupname;
    for (auto &g : groups) {
        if (g.addNetizen(groupname, this)) {
            _groups.push_back(&g);
            cout << "加入群聊成功!" << endl;
            return;
        }
    }
    cout << "群聊不存在，加入失败." << endl;
}

//选择群聊聊天
void Netizen::chooseGroupToChat(pqxx::connection &A)
{
    cout << "以下是您已经加入过的群聊：" << endl;
    //展示类里面已经加入的群聊
    for (auto &g : _groups) {
        g->checkName();
    }
    string groupname;
    cout << "请输入要加入聊天的群聊名称：" << endl;
    cin >> groupname;
    cout << "可以开始聊天了！(输入over结束聊天）" << endl;
    //开始聊天
    chat(A, groupname);

    cout << "依然在这个群聊聊天吗？(y/n)" << endl;
    string a;
    cin >> a;
    if (a == "y") {
        chat(A, groupname);
    } else {
        cout << "已退出聊天!" << endl;
    }
}

//聊天，先显示新消息，输入over结束聊天
void Netizen::chat(pqxx::connection &A, string groupname)
{
    watchNotRead(A, groupname);
    string information;
    while (true) {
        cin >> information;
        if (information == "over")
            break;
        this->insertInformation(A, groupname, information);
    }
}

//插入信息，包括发送人的名字，发送的消息，发送时间，以及是否是新消息
void Netizen::insertInformation(pqxx::connection &A, string tablename, string information)
{
    std::time_t currentTime = std::time(nullptr);
    std::tm *localTime = std::localtime(&currentTime);
    //返回一个指向std::tm结构的指针，该结构包含分解的时间信息（如年、月、日、小时、分钟和秒）
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "-%Y.%m.%d %H:%M:%S", localTime); //格式化时间为可读的字符串

    string sql = "INSERT INTO " + tablename + " (name,information,time,flag) VALUES('"
                 + this->m_name + "','" + information + "','" + buffer + "','new');";
    pqxx::work W(A);
    W.exec(sql);
    W.commit();
}

//查看未读消息（刷新）
void Netizen::watchNotRead(pqxx::connection &A, string tablename)
{
    try {
        string sql = "SELECT (name,information) from " + tablename + " WHERE flag = 'new';";
        pqxx::work W(A);
        pqxx::result res = W.exec(sql);
        // 输出查询结果
        for (auto row : res) { // 使用范围 for 循环遍历结果集
            if (!row.empty()) {
                pqxx::field i = row[0];
                if (!i.is_null()) {
                    string info = pqxx::to_string(i);
                    cout << info << endl;
                }
            }
        }
    } catch (const std::exception &e) {
        cerr << "query failed:" << e.what() << endl;
    }
    //将new改为previous
    string updateSql = "UPDATE " + tablename + " SET flag = 'previous' WHERE flag = 'new';";
    try {
        pqxx::work W(A);
        W.exec(updateSql);
        W.commit();
    } catch (const std::exception &e) {
        // 如果出现异常，则更新失败了
        cerr << "Update failed: " << e.what() << endl;
    }
}
//——————————————————————————————————————————————————————————————————————————————————————

//从已有文件中读取数据到类
void read(pqxx::connection &c)
{
    std::ifstream ifs("../../data.dat");
    //未进行错误处理
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty()) {
            break;
        }
        std::istringstream iss{line};
        std::string l_id;
        iss >> l_id;
        std::string l_name;
        iss >> l_name;
        netizens.emplace_back(l_name, l_id);
        //用户注册，创建用户用于聊天的表&存储用户信息和朋友的data表
        CreateTableForChat(c, l_name, l_id);
    }
    while (std::getline(ifs, line)) {
        std::istringstream iss{line};
        std::string l_name;
        std::string li;
        iss >> l_name;
        for (auto &n : netizens) {
            if (n.sameName(l_name)) {
                while (std::getline(ifs, li)) {
                    if (li.empty())
                        break;
                    for (auto &a : netizens) {
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
