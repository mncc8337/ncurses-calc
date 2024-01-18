#pragma once
#include <string>
using std::string;
using std::to_string;

inline int toint(char a) {
    return int(a - 48);
}
inline char tochr(int a) {
    return char(a + 48);
}

// remove all '0's
inline void fmt(string& str) {
    while(str.length() > 0 and str[0] == '0') str.erase(0, 1);
}

inline string add(string a, string b) {
    string larger;
    string smaller;

    if(a.length() > b.length()) {
        larger = a; smaller = b;
    }
    else {
        larger = b; smaller = a;
    }

    while(smaller.length() < larger.length())
        smaller = '0' + smaller;
    
    int carry = 0;
    int sum = 0;
    string result = "";

    for(int i = larger.length()-1; i >= 0; i--) {
        sum = toint(larger[i]) + toint(smaller[i]) + carry;
        if(sum >= 10) {
            carry = 1;
            sum -= 10;
        }
        else carry = 0;

        result = tochr(sum) + result;
    }
    if(carry != 0) result = '1' + result;

    return result;
}

inline string sub(string a, string b) {
    string larger;
    string smaller;
    bool neg = false;

    if(a.length() >= b.length()) {
        larger = a; smaller = b;
    }
    else {
        neg = true;
        larger = b; smaller = a;
    }


    while(smaller.length() < larger.length())
        smaller = '0' + smaller;

    if(larger < smaller) {
        neg = true;
        std::swap(larger, smaller);
    }
    
    int carry = 0;
    int sum = 0;
    string result = "";

    for(int i = larger.length()-1; i >= 0; i--) {
        sum = toint(larger[i]) - toint(smaller[i]) - carry;
        if(sum < 0) {
            carry = 1;
            sum += 10;
        }
        else carry = 0;

        result = tochr(sum) + result;
    }
    if(carry != 0) result = '1' + result;

    if(neg) result = '-' + result;

    return result;
}

inline string mul1chr(string a, char b) {
    int carry = 0;
    int sum = 0;
    string result = "";

    for(int i = a.length()-1; i >= 0; i--) {
        sum = toint(a[i]) * toint(b) + carry;

        if(sum >= 10) {
            carry = sum / 10;
            sum -= carry * 10;
        }
        else carry = 0;

        result = tochr(sum) + result;
    }
    if(carry != 0) result = tochr(carry) + result;

    return result;
}

inline string mul(string a, string b) {
    string result = "0";

    string sum = "";
    for(int i = b.length()-1; i >= 0; i--) {
        sum = mul1chr(a, b[i]);
        for(int k = 1; k <= b.length() - i - 1; k++)
            sum += '0';
        result = add(result, sum);
    }

    return result;
}

inline string div1chr(string a, char b) {
    string result = "";
    int carry = 0;
    int sum = 0;
    for(int i = 0; i < a.length(); i++) {
        sum = toint(a[i]) + carry * 10;
        result += tochr(sum / toint(b));
        carry = sum % toint(b);
    }

    return result;
}

inline string div(string a, string b, string& mod) {
    mod = "";

    if(a.length() < b.length()) {
        mod = a;
        return "0";
    }
    if(a == b) {
        if(a != "0") {
            mod = "0";
            return "1";
        }
        else return "math error";
    }

    string result = "";

    string d = a.substr(0, b.length());
    if(a.length() >= b.length())
        a = a.substr(b.length(), a.length());
    else a = "_";

    if(a.length() == 0) a = "_";

    int cnt = 0;
    for(int i = 0; i < a.length(); i++) {
        if(a[i] != '_')
            d += a[i];

        cnt = 0;
        string t = sub(d, b);
        while(t[0] != '-') {
            cnt++;
            d = t;
            t = sub(t, b);
        }
        result += to_string(cnt);
    }

    mod = d; fmt(mod);
    return result;
}