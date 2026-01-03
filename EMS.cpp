#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
using namespace std;

// ================= STRUCT =================
struct Expense {
    double amount;
    string category;
    string description;
    int day, month, year;
};

// ================= GLOBAL =================
vector<Expense> expenses;
map<string, double> categoryTotal;
double monthlyIncome;

// ================= FILE LOAD =================
void loadCategory() {
    ifstream file("category.txt", ios::in); // added ios::in
    if (!file) return;

    string cat;
    double amt;
    while (file >> cat >> amt) {
        categoryTotal[cat] = amt;
    }
    file.close();
}

// ================= FILE SAVE =================
void saveCategory() {
    ofstream file("category.txt", ios::out); // added ios::out
    for (auto c : categoryTotal) {
        file << c.first << " " << c.second << endl;
    }
    file.close();
}

// ================= ADD EXPENSE =================
void addExpense() {
    Expense e;

    cout << "Amount: ";
    cin >> e.amount;
    cin.ignore();

    cout << "Category: ";
    getline(cin, e.category);

    cout << "Description: ";
    getline(cin, e.description);

    cout << "Date (DD MM YYYY): ";
    cin >> e.day >> e.month >> e.year;

    expenses.push_back(e);  // vector push_back fixed
    categoryTotal[e.category] += e.amount;

    cout << "Expense Added!\n";
}

// ================= HISTORY =================
void expenseHistory() {
    cout << "\nAmount  Category  Description  Date\n";
    for (auto e : expenses) {
        cout << e.amount << "  " << e.category << "  "
             << e.description << "  "
             << e.day << "-" << e.month << "-" << e.year << endl;
    }
}

// ================= LAST DAYS =================
void lastDaysReport(int days) {
    int cd, cm, cy;
    cout << "Enter Current Date (DD MM YYYY): ";
    cin >> cd >> cm >> cy;

    double total = 0;
    for (auto e : expenses) {
        if (e.month == cm && (cd - e.day) <= days && (cd - e.day) >= 0) {
            total += e.amount;
        }
    }
    cout << "Total Spending (Last " << days << " days): " << total << endl;
}

// ================= THIS MONTH =================
void thisMonthReport() {
    int m;
    cout << "Enter Month: ";
    cin >> m;

    double total = 0;
    map<int, double> daily;

    for (auto e : expenses) {
        if (e.month == m) {
            total += e.amount;
            daily[e.day] += e.amount;
        }
    }

    int maxDay = 0;
    double maxAmt = 0;
    for (auto d : daily) {
        if (d.second > maxAmt) {
            maxAmt = d.second;
            maxDay = d.first;
        }
    }

    cout << "Monthly Total: " << total << endl;
    cout << "Highest Spending Day: " << maxDay << " Amount: " << maxAmt << endl;
    cout << "Savings: " << (monthlyIncome - total) << endl;
}

// ================= DATE RANGE =================
void dateRangeReport() {
    int d1, d2, m;
    cout << "Enter Month: ";
    cin >> m;
    cout << "Start Day: ";
    cin >> d1;
    cout << "End Day: ";
    cin >> d2;

    double total = 0;
    for (auto e : expenses) {
        if (e.month == m && e.day >= d1 && e.day <= d2) {
            total += e.amount;
        }
    }
    cout << "Date Range Total: " << total << endl;
}

// ================= CATEGORY =================
void categoryReport() {
    cout << "\nCategory Wise Spending:\n";
    for (auto c : categoryTotal) {
        cout << c.first << " : " << c.second << endl;
    }
}

// ================= DAILY =================
void dailyReport() {
    int d, m, y;
    cout << "Date (DD MM YYYY): ";
    cin >> d >> m >> y;

    double total = 0;
    for (auto e : expenses) {
        if (e.day == d && e.month == m && e.year == y)
            total += e.amount;
    }
    cout << "Daily Total: " << total << endl;
}

// ================= WEEKLY =================
void weeklyReport() {
    lastDaysReport(7);
}

// ================= COMPARE MONTHS =================
void compareMonths() {
    int m1, m2;
    double t1 = 0, t2 = 0;

    cout << "Month 1: ";
    cin >> m1;
    cout << "Month 2: ";
    cin >> m2;

    for (auto e : expenses) {
        if (e.month == m1) t1 += e.amount;
        if (e.month == m2) t2 += e.amount;
    }

    cout << "Month " << m1 << ": " << t1 << endl;
    cout << "Month " << m2 << ": " << t2 << endl;
}

// ================= MAIN =================
int main() {
    loadCategory();

    cout << "Enter Monthly Income: ";
    cin >> monthlyIncome;

    int ch;
    do {
        cout << "\n===== EXPENSE SYSTEM =====\n";
        cout << "1 Add Expense\n";
        cout << "2 Expense History\n";
        cout << "3 Last 7 Days\n";
        cout << "4 Last 30 Days\n";
        cout << "5 This Month Report\n";
        cout << "6 Date Range Report\n";
        cout << "7 Category Report\n";
        cout << "8 Daily Report\n";
        cout << "9 Weekly Report\n";
        cout << "10 Compare Months\n";
        cout << "0 Exit\n";
        cout << "Choice: ";
        cin >> ch;

        switch (ch) {
            case 1: addExpense(); break;
            case 2: expenseHistory(); break;
            case 3: lastDaysReport(7); break;
            case 4: lastDaysReport(30); break;
            case 5: thisMonthReport(); break;
            case 6: dateRangeReport(); break;
            case 7: categoryReport(); break;
            case 8: dailyReport(); break;
            case 9: weeklyReport(); break;
            case 10: compareMonths(); break;
        }
    } while (ch != 0);

    saveCategory();
    cout << "Data Saved. Bye!\n";
    return 0;
}
