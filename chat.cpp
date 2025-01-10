#include "chat.h"
#include "person.h"
#include "netizen.h"
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <limits>
#include <ctime>
#include <pqxx/pqxx>

// #include <algorithm>
// using std::transform;
// #include <cctype>
// using std::tolower;

extern std::vector<Person> chat;
extern std::vector<Netizen> netizens;
extern std::vector<Group> groups;
//连接数据库
pqxx::connection openDB()
{
    pqxx::connection c("dbname=chat user=ch password=123456 hostaddr=127.0.0.1 port=5432");
    if (c.is_open()) {
        cout << "Opened database successfully:" << c.dbname() << endl;
    } else {
        cerr << "Can't open database\n" << endl;
        exit(0);
    }
    return c;
}

//————————————————————————————————————————————————————————————————————————————————————

void selectGroupsToChat(std::string personname, std::vector<Netizen> netizens, pqxx::connection &A)
{
    Netizen *n = nullptr;
    for (auto &a : netizens) {
        if (a.sameName(personname)) {
            n = &a;
            break;
        }
    } //对网民的一个指针赋值
    std::cout << "---------------------------------" << std::endl;
    std::cout << "- Welcome to the  group chat room:     " << std::endl;
    std::cout << "- 1.创建群聊             " << '\n'
              << "- 2.加入群聊                      " << '\n'
              << "- 3.选择群聊进行聊天                      " << '\n'
              << "- 4.退出                    " << std::endl;
    std::cout << "---------------------------------" << std::endl;

    int flag;
    while (true) {
        std::cin >> flag;
        switch (flag) {
        case 1:
            n->createGroup(A);
            std::cout << "已退出创建群聊" << std::endl;
            break;
        case 2:
            n->joinGroup(A);
            std::cout << "已退出加入群聊" << std::endl;
            break;
        case 3:
            n->chooseGroupToChat(A);
            std::cout << "已退出聊天" << std::endl;
            break;
        default:
            std::cout << "已成功退出" << std::endl;
            break;
        }
    }
}

//————————————————————————————————————————————————————————————————————————————————————

//添加时间戳
string TimeAdd()
{
    std::time_t current = std::time(nullptr);
    std::tm *localTime = std::localtime(&current);
    string str = "-" + std::to_string(1900 + localTime->tm_year) + "."
                 + std::to_string(1 + localTime->tm_mon) + "." + std::to_string(localTime->tm_mday)
                 + "." + std::to_string(localTime->tm_hour) + ":"
                 + std::to_string(localTime->tm_min) + ":" + std::to_string(localTime->tm_sec);
    return str;
}
//————————————————————————————————————————————————————————————————————————————————————

//查看登陆用户的好友信息，从chat数组中操作
void outputFriend(string name)
{
    if (chat.size() == 0) { //用不了？
        cout << "You don't have any firends yet." << endl;
    }
    for (auto &c : chat) {
        if (c.sameName(name)) {
            c.outputFriends();
        }
    }
}

//判断名字是否相同，从chat数组中操作，用于addFriend()，Judgelogin函数
bool nameS(string personname)
{
    for (auto &d : chat) {
        if (d.sameName(personname))
            return true;
    }
    return false;
}

//添加好友，从chat数组中判断是否有该人的名字，然后从类引用addFriend函数
//a_name 发送加好友申请的用户，b_name 想加的好友的名字
void addFriend(string a_name, string b_name)
{
    if (nameS(b_name)) {
        for (auto &a : chat) {
            if (a.sameName(a_name)) {
                a.addFriend(b_name);
                break;
            }
        }
    } else {
        std::cout << "Your friends currently has no registered user!" << std::endl;
    }
}

//删好友,在类中操作
void deleteFriend(string friendname, string personname)
{
    for (auto &c : chat) {
        if (c.sameName(personname)) {
            c.deleteFriends(friendname);
        }
    }
}

//————————————————————————————————————————————————————————————————————————————————————

//判断是否登陆成功，通过name判断
bool Judgelogin(string personname)
{
    if (!nameS(personname)) {
        return false;
    }
    return true;
}

//用户登陆
string Login(string personname, string personid)
{
    std::cout << "Please enter your name and ID!" << '\n' << "Your name:";
    std::cin >> personname;
    std::cout << "Your ID:";
    std::cin >> personid;
    while (!Judgelogin(personname)) {
        cout << "Your name is incorrect, You want to re-enter?(y/n): ";
        string answer;
        cin >> answer;
        if (answer == "y") {
            personname = Login(personname, personid);
        } else if (answer == "n") {
            cout << "You are exitting the chat..." << endl;
            return "*";
            //'*'用于判断用户是否想要继续登陆
        }
    }
    return personname;
}

//用户注册，从chat中操作
void newuser(std::string personname, std::string personid, pqxx::connection &c)
{
    std::cout << "Please enter your name:";
    std::cin >> personname;
    std::cout << "Please enter your id:";
    std::cin >> personid;
    for (auto &r : chat) {
        if (r.sameName(personname)) {
            std::cout << "You already have an account. Please choose to log in." << std::endl;
            Login(personname, personid);
            return;
        }
    }
    chat.emplace_back(personname, personid);
    CreateTableForChat(c, personname, personid); //创建用户表
    std::cout << "Successfully registered" << std::endl;
}

//————————————————————————————————————————————————————————————————————————————————————

//覆盖原本用户表中的内容，用于dataInsert
void dataCover(pqxx::connection &c, string tableName)
{
    std::string del = "truncate table " + tableName + ";";
    pqxx::work w(c);
    w.exec(del);
    w.commit();
}

//将chat数组中有关用户的信息全部写入数据库中的data表中，用于程序结束
void dataInsert(pqxx::connection &c)
{
    dataCover(c, "data");
    for (auto &a : chat) {
        std::string name = a.returnname();
        std::string id = a.returnId(name);
        std::string friendname = a.returnfriendname();
        std::string sql = "insert into data(id,name,friends) values('" + name + "','" + id + "','"
                          + friendname + "');";
        //实际上为了data表的灵活性，应该使表的名字为变量，然后加入sql语句的
        //不过这里因为用户不需要看到表，那么就假设数据表的名称叫data
        pqxx::work w(c);
        w.exec(sql);
        w.commit();
    }
}

//从数据库中读取用户信息到chat数组，用于程序开始
void dataRead(pqxx::connection &c)
{
    std::string s = "SELECT id,name,friends from data;";
    pqxx::work w(c);
    pqxx::result res = w.exec(s);
    chat.clear(); //清空列表
    for (auto re : res) {
        std::string l_id;
        l_id = re["id"].c_str();
        std::string l_name;
        l_name = re["name"].c_str();
        chat.emplace_back(l_id, l_name);
        //？不是?person类是先name,再id的吗？？？？？？？？？？？？？？？？
        //???????????????????????????????????????????????????????????????????????????????????????
    }
    int i = 0; //计数器
    for (auto re : res) {
        std::string fri = re["friends"].c_str();
        //fri是一大串名字
        chat[i].writefriends(fri);
        //因为是一行一行处理的表值，所以用i来计数，如果fri为空，writefriends就直接跳过
        i++;
    }
    w.commit();
}

//——————————————————————————————————————————————————————————————————————————————————————
//在用户选择保存退出时将该用户的表设置为不在线
void setNO(std::string personname, pqxx::connection &c)
{
    pqxx::work w(c);
    string inbool = "UPDATE " + personname + " SET isOnline = 'NO';";
    w.exec(inbool);
    w.commit();
}

//登陆时设置用户表为在线状态
void setYes(std::string personname, pqxx::connection &c)
{
    pqxx::work W(c);
    string inbool = "UPDATE " + personname + " SET isOnline = 'YES';";
    //update是将isOnline这一列的所有值都设置为YES/NO.
    //如果用户一次都没有上线，那么这一列就为空，如果上线那么就全为YES,如果下线就全为NO,
    //如果一方没有上线，那么这一方的isOnline就为空，但之前的消息是NO（因为下线就全为NO)
    W.exec(inbool);
    W.commit();
}

//用户注册，创建用户用于聊天的表
void CreateTableForChat(pqxx::connection &c, string name, string id)
{
    string dataSql
        = "CREATE TABLE IF NOT EXISTS data(id varchar(30) NOT NULL," "name varchar(50) NOT NULL," "friends varchar(50000));";
    string sql
        = "CREATE TABLE IF NOT EXISTS " + name
          + "(info varchar(50000)," "sender varchar(50)," "isOnline char(3)," "isNew char(3));";
    //——————————————对于user table的设计思路解释——————————————
    //改变表的设计为info & sender & isOnline & isNew
    //info存发送的消息，sender存发送这条消息的人，isOnline标记是否在线，isNew标记该条消息是否为新消息
    //发送一条消息时，如果在线，那么存在info中的就只有这条消息；同时sender存入(如果是自己发的消息，则="You"),isNew="NO",isOnline全为"YES"
    //如果不在线，那么存在info中的就有"New messages:"+时间+该条消息，并且是一行一行存的,
    //同时，存时间和信息的那一行的sender=发送方的名字&isNew="YES"，isOnline为空
    //——————————————对于user table的设计思路解释——————————————
    {
        pqxx::work W(c);
        W.exec(sql);
        W.exec(dataSql);
        W.commit();
    } //创建局部作用域的原因是为了不会出现数据库的事务关闭的错误,这里可以不加
}

//发消息———————————————————————————————————————————————————————————————————————————————————
//判断是否在线
bool isOnline(string name, pqxx::connection &c)
{
    pqxx::work W(c);
    string sql = "select isOnline from " + name + ";";
    pqxx::result R = W.exec(sql);
    for (auto r : R) {
        string flag = r[0].c_str();
        if (flag == "YES") {
            W.commit();
            return true;
        }
    }
    W.commit();
    return false;
}

//发消息，但只有异步，即只有一方在线的情况
void Send(string sender, string receiver, string message, pqxx::connection &c)
{
    string isNew; //用于判断该消息是否是新消息的标签
    bool judge = isOnline(receiver, c);
    if (!judge) {
        isNew = "YES"; //如果接收方receiver不在线，那么message对receiver来说就是未读的新消息
    } else {
        isNew = "NO"; //如果receiver在线，那么message对receiver来说就是已读的消息
    }
    string flag = "insert into " + receiver + "(info) values('New massages:');";
    string currentTime = "insert into " + receiver + "(info,sender,isNew) values('" + TimeAdd()
                         + "','" + sender + "','YES');";
    //未读标签与时间显示(时间显示是对receiver不在线的专属,未读那么isNew一定是YES

    string Minesql = "insert into " + sender + "(info,sender,isNew) values('" + message
                     + "','You','NO');"; //发送方sender一定在线，所以是已读消息
    //记录消息message在sender的info列中,使发送方的sender列为'you'整合在一起使得数据库的信息在一行中
    string Othersql = "insert into " + receiver + "(info,sender,isNew) values('" + message + "','"
                      + sender + "','" + isNew + "');";
    //记录消息message在receive的info列中,使接收方receiver的sender列为给他发消息的人的名字，即'sender'

    pqxx::work W(c);
    //异步聊天
    if (!judge) { //对方不在线
        W.exec(Minesql);
        W.exec(flag);
        W.exec(currentTime);
        W.exec(Othersql);
    }
    W.commit();
}

//用于在发消息时显示特定的历史聊天记录
void ShowMessage(string sender, string receiver, pqxx::connection &c)
{
    // transform(receiver.begin(), receiver.end(), receiver.begin(), [](unsigned char c) {
    //     return tolower(c);
    // }); //转化receiver为全小写
    pqxx::work W(c);
    cout << "now: " << TimeAdd() << ": " << endl;
    string sql = "select (info,sender) from " + sender + " where sender = " + "'" + receiver + "';";
    pqxx::result R = W.exec(sql);
    for (auto row : R) {
        if (!row.empty()) {
            pqxx::field m = row[0];
            if (!m.is_null()) {
                string mes = pqxx::to_string(m);
                cout << mes << endl;
            }
        }
    }
    W.commit();
    //将查看的消息设置为已读
    string NewSql = "UPDATE " + sender + " SET isNew = 'NO';";
    pqxx::work w1(c);
    w1.exec(NewSql);
    w1.commit();
}

//显示未读消息和发送人
void ShowNewMessage(const string name, pqxx::connection &c)
{
    pqxx::work w(c);
    string InfoSql = "SELECT  (info,sender) from " + name + " where isNew = 'YES';";
    pqxx::result RI = w.exec(InfoSql);

    for (auto row : RI) {
        if (!row.empty()) {
            pqxx::field i = row[0];
            if (!i.is_null()) {
                string info = pqxx::to_string(i);
                cout << info << endl;
            }
        }
    }
    w.commit();
    //将查看的消息设置为已读
    string NewSql = "UPDATE " + name + " SET isNew = 'NO';";
    pqxx::work w1(c);
    w1.exec(NewSql);
    w1.commit();
}
//发消息————————————————————————————————————————————————————————————————————————————————————

void selectFunction(std::vector<Person> chat)
{
    pqxx::connection c = openDB();

    //read(c); //从文件中写入数据到类，录入初始化值
    dataRead(c); //从数据库中读入信息,增删好友时的数据变化可以被捕捉
    string personname, personid, friendname;
    cout << "Login input 1; Register as a new user input 2:";
    int q;
    cin >> q;
    if (q == 1) {
        personname = Login(personname, personid);
        if (personname == "*") //用户想要退出
            return;
        setYes(personname, c);
    } else if (q == 2) {
        newuser(personname, personid, c);
        setYes(personname, c);
    } else {
        cout << "Please choose correct number: ";
        selectFunction(chat);
    }
    cout << "---------------------------------" << endl;
    cout << "- Welcome to the chat room:     " << endl;
    cout << "- 1.查看我的好友                  " << '\n'
         << "- 2.加好友                       " << '\n'
         << "- 3.删好友                       " << '\n'
         << "- 4.查看私聊的未读消息                  " << '\n'
         << "- 5.私聊                       " << '\n'
         << "- 6.群聊                         " << '\n'
         << "- 7.保存退出                     " << endl;
    cout << "---------------------------------" << endl;
    while (true) {
        int flag;
        cout << "Please select: ";
        cin >> flag;
        switch (flag) {
        case (1):
            outputFriend(personname);
            break;
        case (2):
            cout << "Please enter the name of the friend you want to add:";
            cin >> friendname;
            addFriend(personname, friendname);
            break;
        case (3):
            cout << "Please enter the name of the friend you want to delect:";
            cin >> friendname;
            deleteFriend(friendname, personname);
            cout << "删除成功，下列是您的好友列表：" << endl;
            outputFriend(personname);
            break;
        case (4): {
            cout << "以下是你未读的消息：" << endl;
            ShowNewMessage(personname, c);
            cout << "你想要选择其他功能吗？（y/n)";
            string x;
            while (cin >> x) {
                if (x == "y") {
                    selectFunction(chat);
                } else if (x == "n") {
                    break;
                } else {
                    cout << "请输入正确的选项-> (y/n)";
                }
            }
        }
        case (5): {
            cout << "请输入你想发送消息的人的名字：";
            string receiver;
            cin >> receiver;
            ShowMessage(personname, receiver, c);
            //显示receiver与personname的历史聊天记录
            cout << "可以开始发消息了！输入over结束聊天" << endl;

            string message;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            //忽略\n,使第一行读入的字符串不为空
            std::getline(cin, message);
            while (message != "over") {
                Send(personname, receiver, message, c);
                std::getline(cin, message);
            }
        } break;
        case (6): {
            selectGroupsToChat(personname, netizens, c);
        }
        case (7):
            setNO(personname, c);
            dataInsert(c);
            return;
        default:
            cout << "Currently, no other features have been developed." << endl;
            dataInsert(c);
            break;
        }
    }
}
