/* Minimal JSON support for Arduino */

#include <iostream>

#include "JSON.h"

JSON::JSON()
{
    memset(this, sizeof(JSON), 0);
}

string JSON::stringify(JSON& obj)
{
    return "hello world";
}

JSON * JSON::parse(string src)
{
    return (JSON *)0;
}

void JSON::to_string(string &str)
{
}

void JSON::JsonObj::insert(string &name, JSON& value)
{
}

JSON * JSON::JsonObj::retrieve(string &name)
{
    return (JSON *)0;
}
