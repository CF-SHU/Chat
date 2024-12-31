#pragma once
#include "person.h"

void selectFunction(std::vector<Person> chat);
pqxx::connection openDB();           //连接数据库
void outputFriend(std::string name); //查看登陆用户的好友信息，从chat数组中操作
void addFriend(std::string a_name, std::string b_name);
//添加好友，从chat数组操作，引用Person::addFriend

bool Judgelogin(std::string personname); //判断是否登陆成功，通过name判断
//void dataInsert(pqxx::connection &c);//将chat数组中有关用户的信息全部写入数据库中的data表中
//void dataRead(pqxx::connection &c);  //从数据库中读取用户信息到chat数组
void deleteFriend(std::string friendname, std::string personname);

//判断名字是否相同，从chat数组中操作
bool nameS(std::string personname);
//用户登陆
std::string Login(std::string personname, std::string personid);
//用户注册，从chat中操作
void newuser(std::string personname, std::string personid, pqxx::connection &c);
//发消息
void send(std::string receiver, std::string message, pqxx::connection &c);
//用户注册，创建用户用于聊天的表
void CreateTableForChat(pqxx::connection &c, std::string name, std::string id);
//从数据库中读取用户信息到chat数组
void dataRead(pqxx::connection &c);
