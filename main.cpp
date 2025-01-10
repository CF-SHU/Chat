#include <iostream>
#include <vector>
#include "chat.h"
#include "person.h"
#include "netizen.h"

std::vector<Person> chat;
std::vector<Group> groups;
std::vector<Netizen> netizens;

int main()
{
    selectFunction(chat);
    return 0;
}
