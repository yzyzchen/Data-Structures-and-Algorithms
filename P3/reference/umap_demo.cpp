#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;
 
int main() {
    unordered_map<string, int> months;
  
    // The simple way to add items
    // [] causes it to exist!
    months["January"] = 31;
    months["February"] = 28;
    months["March"] = 31;
    months["April"] = 30;
    months["May"] = 31;
    months["June"] = 31;
    months["July"] = 31;
    months["August"] = 31;
    months["September"] = 30;
    months["October"] = 31;
    months["November"] = 30;
    months["December"] = 31;

    string month;
    cout << "Enter a month name: ";
    cin >> month;
 
    // The simple (but wrong way) to look up an item.
    // Causes "month" to exist, with 0 days.
    //cout << month << " has " << months[month] << " days\n";
 
    // The right way to look up an item.
    // Note: it -> pair<string, int>
    auto it = months.find(month);
    if (it == months.end())
        cout << month << " not found\n";
    else
        cout << it->first << " has " << it->second << " days\n";
 
    return 0;
} // main()