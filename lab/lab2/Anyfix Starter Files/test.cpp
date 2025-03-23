

#include <stack>
#include <cstdint>
#include <string>
#include <cctype>
#include <iostream>
using namespace std;

int main()
{
    const char *s = "hello";
    char ss [20];
    int length = strlen(s);

    for (int i = 0; i < length; ++i)
    {
        ss[i] = s[length - i]; 
        printf("%s", ss);
    }
    return 0;
}