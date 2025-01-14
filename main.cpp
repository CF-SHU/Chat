//sci第六项目小组关于实现异步聊天功能程序的项目
//main函数主要用于设置全局变量与引用selcetFunction函数
#include <iostream>
#include <vector>
#include "chat.h"
#include "netizen.h"
#include "group.h"

std::vector<Group> groups;
std::vector<Netizen> netizens;

int main()
{
    selectFunction();
    return 0;
}
