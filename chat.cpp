#include "chat.h"
#include "person.h"
#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <ctime>
#include <pqxx/pqxx>

extern std::vector<Person> chat;

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

//添加时间戳
string TimeAdd()
{
    std::time_t current = std::time(nullptr);
    std::tm *localTime = std::localtime(&current);
    string str = "-" + std::to_string(1900 + localTime->tm_year) + "-"
                 + std::to_string(1 + localTime->tm_mon) + "-" + std::to_string(localTime->tm_mday)
                 + "-" + std::to_string(localTime->tm_hour) + ":"
                 + std::to_string(localTime->tm_min) + ":" + std::to_string(localTime->tm_sec);
    return str;
}
//————————————————————————————————————————————————————————————————————————————————————

//查看登陆用户的好友信息，从chat数组中操作
void outputFriend(std::string name)
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
//判断名字是否相同，从chat数组中操作
bool nameS(std::string personname)
{
    for (auto &d : chat) {
        if (d.sameName(personname))
            return true;
    }
    return false;
}
//添加好友，从chat数组中判断是否有该人的名字，然后从类引用addFriend函数
//a_name 发送加好友申请的用户，b_name 想加的好友的名字
void addFriend(std::string a_name, std::string b_name)
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
void deleteFriend(std::string friendname, std::string personname)
{
    for (auto &c : chat) {
        if (c.sameName(personname)) {
            c.deleteFriends(friendname);
        }
    }
}

//————————————————————————————————————————————————————————————————————————————————————
//判断是否登陆成功，通过name判断
bool Judgelogin(std::string personname)
{
    if (!nameS(personname)) {
        return false;
    }
    return true;
}
//用户登陆
std::string Login(std::string personname, std::string personid)
{
    std::cout << "Please enter your name and ID!" << '\n' << "Your name:";
    std::cin >> personname;
    std::cout << "Your ID:";
    std::cin >> personid;
    while (!Judgelogin(personname)) {
        cout << "Your name is incorrect, You want to re-enter?(Y/N): ";
        string answer;
        cin >> answer;
        if (answer == "Y") {
            personname = Login(personname, personid);
        } else if (answer == "N") {
            cout << "You are exitting the chat..." << endl;
            return "*";
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
void datacover(pqxx::connection &c, string tableName)
{
    std::string del = "truncate table " + tableName + ";";
    pqxx::work w(c);
    w.exec(del);
    w.commit();
}
//将chat数组中有关用户的信息全部写入数据库中的data表中
void dataInsert(pqxx::connection &c)
{
    //datacover(c, "donald");
    //datacover(c, "mickey");
    datacover(c, "data");
    for (auto &a : chat) {
        std::string name = a.returnname();
        std::string id = a.returnId(name);
        std::string friendname = a.returnfriendname();
        std::string sql = "insert into data(id,name,friends) values('" + name + "','" + id + "','"
                          + friendname + "');";
        //实际上为了data表的灵活性，应该使表的名字为变量，然后加入sql语句的
        pqxx::work w(c);
        w.exec(sql);
        w.commit();
    }
}
//从数据库中读取用户信息到chat数组
void dataRead(pqxx::connection &c)
{
    std::string sql = "select id,name,friends from data;";
    pqxx::work w(c);
    pqxx::result res = w.exec(sql);
    for (auto re : res) {
        std::string l_id;
        l_id = re["id"].c_str();
        std::string l_name;
        l_name = re["name"].c_str();
        chat.emplace_back(l_name, l_id);
        std::string fri = re["friends"].c_str();
        //fri是一大串名字
        for (auto &y : chat) {
            y.writefriends(fri);
            //用类函数处理
        }
    }
    w.commit();
}

//————————————————————————————————————————————————————————————————————————————————————
//在用户选择5保存退出时将该用户的表设置为不在线
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
    W.exec(inbool);
    W.commit();
}
//————————————————————————————————————————————————————————————————————————————————————
//用户注册，创建用户用于聊天的表
void CreateTableForChat(pqxx::connection &c, string name, string id)
{
    //string sql = "CREATE TABLE IF NOT EXISTS " + name
    //+"(" "name   varchar(50)," "id    varchar(30)," "info   " "varch" "ar(" "50000" ")," "sender   " "  " "varchar(" "50)," "is" "On" "li" "ne" "  " " c" "ha" "r(" "3)" ")" ";";
    string sql
        = "CREATE TABLE IF NOT EXISTS " + name
          + "(info varchar(50000)," "sender varchar(50)," "isOnline char(3));"; //改变表的设计为info & isOnline & sender
    {
        pqxx::work W(c);
        W.exec(sql);
        W.commit();
        //cout<<"create user table successfully.\n";
    } //创建局部作用域的原因是为了不会出现数据库的事务关闭的错误
    {
        pqxx::work W(c);
        //string inname = "insert into " + name + "(name) values('" + name + "');";
        //string inid = "insert into " + name + "(id) values('" + id + "');";
        string inbool = "insert into " + name + "(isOnline) values('NO');"; //初始化为不在线
        //W.exec(inname);
        //W.exec(inid);
        W.exec(inbool);
        W.commit();
        //cout<<"insert name,id,isonline successfully.\n";
    }
}
//————————————————————————————————————————————————————————————————————————————————————
//发消息
bool isOnline(string name, pqxx::connection &c)
{
    pqxx::work W(c);
    string sql = "select isOnline from " + name + ";";
    pqxx::result R = W.exec(sql);
    for (auto r : R) {
        string flag = r[0].c_str();
        if (flag == "YES") {
            //W.rollback(); //如果发生错误或你决定不保存更改,回滚，并结束事务
            W.commit();
            return true;
        }
    }
    W.commit();
    return false;
}
void ShowMessage(string name, pqxx::connection &c);
void Send(string sender, string receiver, string message, pqxx::connection &c)
{
    string flag = "insert into " + receiver + "(info) values('New massages:');";
    string currentTime = "insert into " + receiver + "(info) values('" + TimeAdd() + "');";
    //未读标签与时间显示

    string Minesql = "insert into " + sender + "(info,sender) values('" + message + "','You');";
    //记录消息message在sender的info列中,使发送方的sender列为'you'整合在一起使得数据库的信息在一行中
    string Othersql = "insert into " + receiver + "(info,sender) values('" + message + "','"
                      + sender + "');";
    //记录消息message在receive的info列中,使接收方receiver的sender列为给他发消息的人的名字，即'sender'

    //string inbool = "insert into " + name + "(isOnline) values(true);";

    bool judge = isOnline(receiver, c);
    pqxx::work W(c);
    if (!judge) {
        W.exec(Minesql);
        W.exec(flag);
        W.exec(currentTime);
        W.exec(Othersql);
        //对方不在线
    } else {
        W.exec(Minesql);  //我的
        {
            W.exec(Othersql);
            W.commit();
        } //别人
        ShowMessage(sender, c);
    } //对方在线
    W.commit();
}
//用于对方在线，且没有时间显示
void ShowMessage(string name, pqxx::connection &c)
{
    pqxx::work W(c);
    string sql = "select info from " + name + ";";
    pqxx::result R = W.exec(sql);
    for (auto row : R) {
        string mes = row[2].c_str();
        cout << TimeAdd() << ": ";
        cout << mes << endl;
    }
    W.commit();
}
//发消息————————————————————————————————————————————————————————————————————————————————————

void selectFunction(std::vector<Person> chat)
{
    pqxx::connection c = openDB();

    read(c); //从文件中写入数据到类
    //dataRead(c); //从数据库中读入信息
    std::string personname, personid, friendname;
    std::cout << "Login input 1; register as a new user input 2:";
    int q;
    std::cin >> q;
    if (q == 1) {
        personname = Login(personname, personid);
        setYes(personname, c);
        if (personname == "*") //用户想要退出
            return;
    } else if (q == 2) {
        newuser(personname, personid, c);
        setYes(personname, c);
    } else {
        cout << "Please choose correct number: ";
        selectFunction(chat);
    }
    int flag;
    std::cout << "---------------------------------" << std::endl;
    std::cout << "- Welcome to the chat room:     -" << std::endl;
    std::cout << "- 1.查看我的好友                -" << '\n'
              << "- 2.加好友                      -" << '\n'
              << "- 3.删好友                      -" << '\n'
              << "- 4.发消息                      -" << '\n'
              << "- 5.保存退出                    -" << std::endl;
    std::cout << "---------------------------------" << std::endl;
    while (true) {
        std::cout << "Please select: ";
        std::cin >> flag;
        switch (flag) {
        case (1):
            outputFriend(personname);
            break;
        case (2):
            std::cout << "Please enter the name of the friend you want to add:";
            std::cin >> friendname;
            addFriend(personname, friendname);
            break;
        case (3):
            std::cout << "Please enter the name of the friend you want to delect:";
            std::cin >> friendname;
            deleteFriend(friendname, personname);
            cout << "删除成功，下列是您的好友列表：" << endl;
            outputFriend(personname);
            break;
        case (4): {
            std::cout << "请输入你想发送消息的人的名字：";
            std::string receiver;
            std::cin >> receiver;
            std::cout << "可以开始发消息了！输入over结束聊天" << std::endl;
            string message;
            std::getline(cin, message);
            while (message != "over") {
                Send(personname, receiver, message, c);
                std::getline(cin, message);
            }
        } break;
        case (5):
            setNO(personname, c);
            dataInsert(c);
            return;
        default:
            std::cout << "Currently, no other features have been developed." << std::endl;
            break;
        }
    }
}
