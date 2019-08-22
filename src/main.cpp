#include <iostream>
#include <regex>

#include "Email.h"

using namespace std;

bool matchEmailAddr(char str[]) {
    regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
    string emailAddr(str);
    if (regex_match(emailAddr, pattern)) {
        return true;
    } else {
        return false;
    }
}

int main() {
    char toAddr[BUFSIZ];
    cout << "ÇëÊäÈëÓÊÏä£º";
    cin >> toAddr;
    if (matchEmailAddr(toAddr)) {
        Email e(toAddr);
        e.sendTo();
    } else {
        cout << "Æ¥ÅäÊ§°Ü"  << endl;
    };
    return 0;
}