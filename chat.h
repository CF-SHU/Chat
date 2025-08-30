//函数声明
#pragma once
#include "netizen.h"

pqxx::connection openDB(); //连接数据库

//主功能选择
void selectFunction();

//群聊功能选择
void selectGroupsToChat(std::string personname, pqxx::connection &A);

//添加时间戳
std::string TimeAdd();

//查看登陆用户的好友信息，从netizens数组中操作
void outputFriend(std::string name);

//判断名字是否相同，从netizens数组中操作，用于addFriend()，Judgelogin函数
bool nameS(std::string personname);

//添加好友，从netizens数组中判断是否有该人的名字，然后从类引用addFriend函数
//a_name 发送加好友申请的用户，b_name 想加的好友的名字
void addFriend(std::string a_name, std::string b_name);

//删好友,在类中操作
void deleteFriend(std::string friendname, std::string personname);

//判断是否登陆成功，通过name判断
bool Judgelogin(std::string personname);

//用户登陆
std::string Login(std::string personname, std::string personid);

//用户注册，从netizens中操作
void newuser(std::string personname, std::string personid, pqxx::connection &c);

//覆盖原本用户表中的内容，用于dataInsert
void dataCover(pqxx::connection &c, std::string tableName);

//将netizens数组中有关用户的信息全部写入数据库中的data表中，用于程序结束
//将groups数组中有关用户的信息全部写入数据库中的groups表中
void dataInsert(pqxx::connection &c);

//从数据库中读取用户信息到netizens数组，用于程序开始
void dataRead(pqxx::connection &c);

//在用户选择保存退出时将该用户的表设置为不在线
void setNO(std::string personname, pqxx::connection &c);

//登陆时设置用户表为在线状态
void setYes(std::string personname, pqxx::connection &c);

//用户注册，创建用户用于聊天的表
void CreateTableForChat(pqxx::connection &c, std::string name, std::string id);

//判断是否在线
bool isOnline(std::string name, pqxx::connection &c);

//发消息，但只有异步，即只有一方在线的情况
void Send(std::string sender, std::string receiver, std::string message, pqxx::connection &c);

//用于在发消息时显示特定的历史聊天记录
void ShowMessage(std::string sender, std::string receiver, pqxx::connection &c);

//显示未读消息和发送人
void ShowNewMessage(const std::string name, pqxx::connection &c);
