#include "backend.h"
#include "utils.h"
double FineCalculator::calcFine(const string& dueDate, const string& returnDate) {
    int overdue = daysBetween(dueDate, returnDate);
    if (overdue <= 0) return 0.0;
    double fine = 0.0;
    // Staged rate
    if (overdue <= 7) fine = overdue * 0.5;
    else if (overdue <= 30) fine = 7 * 0.5 + (overdue - 7) * 1.0;
    else fine = 7 * 0.5 + 23 * 1.0 + (overdue - 30) * 2.0;
    return min(fine, MAX_FINE);
}
