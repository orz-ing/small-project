#include "json_exporter.h"
#include <fstream>
#include <sstream>

string JsonExporter::serialize(const vector<map<string,string>>& data) {
    string json = "[";
    for (size_t i = 0; i < data.size(); i++) {
        if (i > 0) json += ",";
        json += "{";
        bool first = true;
        for (auto& [key, val] : data[i]) {
            if (!first) json += ",";
            first = false;
            json += "\"" + key + "\":\"" + val + "\"";
        }
        json += "}";
    }
    json += "]";
    return json;
}

static string escapeJson(const string& s) {
    string r;
    for (char c : s) {
        switch (c) {
            case '"': r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\n': r += "\\n"; break;
            case '\r': r += "\\r"; break;
            case '\t': r += "\\t"; break;
            default: r += c;
        }
    }
    return r;
}

bool JsonExporter::exportBooks(const vector<Book>& books, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "[";
    for (size_t i = 0; i < books.size(); i++) {
        if (i > 0) ofs << ",";
        ofs << "{";
        ofs << "\"id\":" << books[i].getId() << ",";
        ofs << "\"isbn\":\"" << escapeJson(books[i].getIsbn()) << "\",";
        ofs << "\"title\":\"" << escapeJson(books[i].getTitle()) << "\",";
        ofs << "\"author\":\"" << escapeJson(books[i].getAuthor()) << "\",";
        ofs << "\"publisher\":\"" << escapeJson(books[i].getPublisher()) << "\",";
        ofs << "\"stock\":" << books[i].getStock() << ",";
        ofs << "\"totalStock\":" << books[i].getTotalStock();
        ofs << "}";
    }
    ofs << "]\n";
    return true;
}

bool JsonExporter::exportUsers(const vector<User>& users, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "[";
    for (size_t i = 0; i < users.size(); i++) {
        if (i > 0) ofs << ",";
        ofs << "{";
        ofs << "\"id\":" << users[i].getId() << ",";
        ofs << "\"username\":\"" << escapeJson(users[i].getUsername()) << "\",";
        ofs << "\"realName\":\"" << escapeJson(users[i].getRealName()) << "\",";
        ofs << "\"role\":\"" << users[i].getRole() << "\",";
        ofs << "\"credit\":" << users[i].getCredit() << ",";
        ofs << "\"status\":\"" << users[i].getStatus() << "\"";
        ofs << "}";
    }
    ofs << "]\n";
    return true;
}

bool JsonExporter::exportBorrowRecords(const vector<BorrowRecord>& records, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "[";
    for (size_t i = 0; i < records.size(); i++) {
        if (i > 0) ofs << ",";
        ofs << "{";
        ofs << "\"id\":" << records[i].getId() << ",";
        ofs << "\"userId\":" << records[i].getUserId() << ",";
        ofs << "\"bookId\":" << records[i].getBookId() << ",";
        ofs << "\"borrowDate\":\"" << records[i].getBorrowDate() << "\",";
        ofs << "\"dueDate\":\"" << records[i].getDueDate() << "\",";
        ofs << "\"status\":\"" << records[i].getStatus() << "\"";
        ofs << "}";
    }
    ofs << "]\n";
    return true;
}

bool JsonExporter::exportJson(const vector<map<string,string>>& data, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << serialize(data) << "\n";
    return true;
}
