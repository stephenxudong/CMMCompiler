#include "type/TableNode.h"

string TableNode::getKind() const
{
    return kind;
}

void TableNode::setKind(const string &value)
{
    kind = value;
}

string TableNode::getIntVal() const
{
    return intVal;
}

void TableNode::setIntVal(const string &value)
{
    intVal = value;
}

string TableNode::getRealVal() const
{
    return realVal;
}

void TableNode::setRealVal(const string &value)
{
    realVal = value;
}

string TableNode::getName() const
{
    return name;
}

void TableNode::setName(const string &value)
{
    name = value;
}

int TableNode::getLevel() const
{
    return level;
}

void TableNode::setLevel(int value)
{
    level = value;
}

int TableNode::getArrayEleCnt() const
{
    return arrayEleCnt;
}

void TableNode::setArrayEleCnt(int value)
{
    arrayEleCnt = value;
}

int TableNode::getLine() const
{
    return line;
}

void TableNode::setLine(int value)
{
    line = value;
}

std::string TableNode::tostr() const
{
    string s = "";
    s.append(this->name).append("_").append(to_string(this->level));
    s.append("_").append(to_string(this->arrayEleCnt));
    return s;
}

string TableNode::getStringVal() const
{
    return stringVal;
}

void TableNode::setStringVal(const string &value)
{
    stringVal = value;
}
