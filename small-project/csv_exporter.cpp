#include "csv_exporter.h"
#include <fstream>
#include <sstream>

string CsvExporter::escapeCsvField(const string& field) {
    if (field.find(',') == string::npos && field.find('"') == string::npos && field.find('\n') == string::npos)
        return field;
    string escaped = field;
    size_t pos = 0;
    while ((pos = escaped.find('"', pos)) != string::npos) {
        escaped.insert(pos, "\"");
        pos += 2;
    }
    return "\"" + escaped + "\"";
}

bool CsvExporter::exportBooks(const vector<Book>& books, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "\xEF\xBB\xBFID,ISBN,Title,Author,Publisher,CategoryID,Stock,TotalStock,Location\n";
    for (auto& b : books)
        ofs << b.getId() << "," << escapeCsvField(b.getIsbn()) << ","
            << escapeCsvField(b.getTitle()) << "," << escapeCsvField(b.getAuthor()) << ","
            << escapeCsvField(b.getPublisher()) << "," << b.getCategoryId() << ","
            << b.getStock() << "," << b.getTotalStock() << ","
            << escapeCsvField(b.getLocation()) << "\n";
    return true;
}

bool CsvExporter::exportUsers(const vector<User>& users, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "\xEF\xBB\xBFID,Username,RealName,Role,Credit,Status,Phone,Email\n";
    for (auto& u : users)
        ofs << u.getId() << "," << escapeCsvField(u.getUsername()) << ","
            << escapeCsvField(u.getRealName()) << "," << u.getRole() << ","
            << u.getCredit() << "," << u.getStatus() << ","
            << escapeCsvField(u.getPhone()) << "," << escapeCsvField(u.getEmail()) << "\n";
    return true;
}

bool CsvExporter::exportBorrowRecords(const vector<BorrowRecord>& records, const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "\xEF\xBB\xBFID,UserID,BookID,BorrowDate,DueDate,ReturnDate,Status,RenewCount\n";
    for (auto& r : records)
        ofs << r.getId() << "," << r.getUserId() << "," << r.getBookId() << ","
            << r.getBorrowDate() << "," << r.getDueDate() << "," << r.getReturnDate() << ","
            << r.getStatus() << "," << r.getRenewCount() << "\n";
    return true;
}

bool CsvExporter::exportTable(const vector<string>& headers,
                              const vector<vector<string>>& rows,
                              const string& filePath) {
    ofstream ofs(filePath);
    if (!ofs) return false;
    ofs << "\xEF\xBB\xBF";
    for (size_t i = 0; i < headers.size(); i++) {
        if (i > 0) ofs << ",";
        ofs << escapeCsvField(headers[i]);
    }
    ofs << "\n";
    for (auto& row : rows) {
        for (size_t i = 0; i < row.size(); i++) {
            if (i > 0) ofs << ",";
            ofs << escapeCsvField(row[i]);
        }
        ofs << "\n";
    }
    return true;
}
