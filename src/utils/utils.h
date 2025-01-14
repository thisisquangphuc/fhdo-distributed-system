
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>


/**
 * @brief Convert a string to bool
 *
 * @param str
 * @return true
 * @return false
 */
inline bool stob(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    // std::istringstream is(str);
    // bool b;
    // is >> std::boolalpha >> b;
    // return b;

    std::vector<std::string> tab{"true", "t", "yes", "y", "1"};
    return std::find(tab.begin(), tab.end(), str) != tab.end();
}