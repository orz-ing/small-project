#include "backend.h"
#include "utils.h"
#include <cstring>
#include <vector>
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>

// SHA-256 implementation (FIPS 180-4)
namespace {
    inline uint32_t ror(uint32_t v, int n) { return (v >> n) | (v << (32 - n)); }
    inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
    inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
    inline uint32_t sigma0(uint32_t x) { return ror(x,2)^ror(x,13)^ror(x,22); }
    inline uint32_t sigma1(uint32_t x) { return ror(x,6)^ror(x,11)^ror(x,25); }
    inline uint32_t om0(uint32_t x) { return ror(x,7)^ror(x,18)^(x>>3); }
    inline uint32_t om1(uint32_t x) { return ror(x,17)^ror(x,19)^(x>>10); }
    const uint32_t K[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
}
string sha256(const string& input) {
    uint64_t bitLen = input.size() * 8;
    vector<uint8_t> data(input.begin(), input.end());
    data.push_back(0x80);
    while ((data.size() * 8) % 512 != 448) data.push_back(0x00);
    for (int i = 7; i >= 0; i--) data.push_back((uint8_t)((bitLen >> (i * 8)) & 0xff));
    uint32_t H[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    for (size_t i = 0; i < data.size(); i += 64) {
        uint32_t W[64];
        for (int t = 0; t < 16; t++)
            W[t] = (uint32_t)data[i+t*4]<<24|(uint32_t)data[i+t*4+1]<<16|(uint32_t)data[i+t*4+2]<<8|(uint32_t)data[i+t*4+3];
        for (int t = 16; t < 64; t++) W[t] = om1(W[t-2]) + W[t-7] + om0(W[t-15]) + W[t-16];
        uint32_t a=H[0],b=H[1],c=H[2],d=H[3],e=H[4],f=H[5],g=H[6],h=H[7];
        for (int t = 0; t < 64; t++) {
            uint32_t T1 = h + sigma1(e) + ch(e,f,g) + K[t] + W[t];
            uint32_t T2 = sigma0(a) + maj(a,b,c);
            h=g; g=f; f=e; e=d+T1; d=c; c=b; b=a; a=T1+T2;
        }
        H[0]+=a; H[1]+=b; H[2]+=c; H[3]+=d; H[4]+=e; H[5]+=f; H[6]+=g; H[7]+=h;
    }
    const char hex[]="0123456789abcdef"; string result;
    for (int i=0;i<8;i++) for(int j=3;j>=0;j--){uint8_t b=(uint8_t)((H[i]>>(j*8))&0xff);result+=hex[b>>4];result+=hex[b&0xf];}
    return result;
}

vector<string> split(const string& s, char delim) {
    vector<string> tokens; stringstream ss(s); string token;
    while (getline(ss, token, delim)) if (!token.empty()) tokens.push_back(token);
    return tokens;
}

string dateToString(const tm& date) {
    ostringstream oss; oss<<setfill('0')<<(date.tm_year+1900)<<"-"<<setw(2)<<(date.tm_mon+1)<<"-"<<setw(2)<<date.tm_mday;
    return oss.str();
}

tm stringToDate(const string& s) {
    tm tm={}; istringstream iss(s); iss>>get_time(&tm,"%Y-%m-%d");
    if (iss.fail()){tm.tm_year=124;tm.tm_mon=0;tm.tm_mday=1;}
    return tm;
}

string today() {
    time_t t=time(nullptr); auto* lt=localtime(&t); return dateToString(*lt);
}

string currentDateTime() {
    time_t t=time(nullptr); auto* lt=localtime(&t);
    ostringstream oss; oss<<setfill('0')<<(lt->tm_year+1900)<<"-"<<setw(2)<<(lt->tm_mon+1)<<"-"<<setw(2)<<lt->tm_mday
        <<" "<<setw(2)<<lt->tm_hour<<":"<<setw(2)<<lt->tm_min<<":"<<setw(2)<<lt->tm_sec;
    return oss.str();
}

int daysBetween(const string& d1, const string& d2) {
    auto t1=stringToDate(d1); auto t2=stringToDate(d2);
    return (int)difftime(mktime(&t2),mktime(&t1))/(60*60*24);
}

bool isHoliday(const string& date) {
    auto tm=stringToDate(date); time_t t=mktime(&tm); auto* lt=localtime(&t);
    return lt->tm_wday==0||lt->tm_wday==6;
}

string addDays(const string& date, int days) {
    auto tm=stringToDate(date); time_t t=mktime(&tm); t+=days*24*60*60; auto* lt=localtime(&t);
    return dateToString(*lt);
}

string toLower(const string& s) {
    string r=s; for(auto& c:r)c=(char)tolower(c); return r;
}
