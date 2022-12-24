
#include"utility.hpp"

#include <cstdio>
#include <string>

namespace gob
{
std::vector<String> split(const String& str, const char delimiter)
{
    std::vector<String> out;
    int start = 0;
    int end = str.indexOf(delimiter);

    while(end >= 0)
    {
        out.emplace_back(str.substring(start, end));
        start = end + 1;
        end = str.indexOf(delimiter, start);
    }
    out.emplace_back(str.substring(start));
    return out;
}




//
}
