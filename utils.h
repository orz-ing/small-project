#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <ctime>
using namespace std;
string sha256(const string& input);
vector<string> split(const string& s, char delim);
string dateToString(const tm& date);
tm stringToDate(const string& s);
string today();
string currentDateTime();
int daysBetween(const string& d1, const string& d2);
bool isHoliday(const string& date);
string addDays(const string& date, int days);
string toLower(const string& s);
#endif
