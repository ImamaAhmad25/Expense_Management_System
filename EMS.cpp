#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
using namespace std;

//Expense Structure
struct Expense {
    double amount;
    string category;
    string description;
    int day,month,year;
};

//Global Variables
vector<Expense> expenses;
map<string, double> categoryTotal;
double monthlyIncome;

//Read expenses file
void loadExpenses() {
    ifstream file("expenses.txt", ios::in); //read mode
    if (!file) 
    {
        cout<<"File not found. Starting fresh.\n";
        return;
    }

    Expense record;
    while (file >> record.amount
                >> record.category
                >> record.description
                >> record.day
                >> record.month
                >> record.year) {

        expenses.push_back(record);
        categoryTotal[record.category] += record.amount;
    }
    file.close();
}

//Save expenses to file
void saveExpenses() {
    ofstream file("expenses.txt", ios::out);  //write mode
    for (auto record : expenses) {
        file << record.amount << " "
             << record.category << " "
             << record.description << " "
             << record.day << " "
             << record.month << " "
             << record.year << endl;
    }
    file.close();
}

//Adding Expense
void addExpense() {
    Expense record;

    cout << "Amount: ";
    cin >> record.amount;
    cin.ignore();

    cout << "Category: ";
    getline(cin, record.category);

    cout << "Description: ";
    getline(cin, record.description);

    cout << "Date (DD MM YYYY): ";
    cin >> record.day >> record.month >> record.year;

    expenses.push_back(record);
    categoryTotal[record.category] += record.amount;

    cout << "Expense Added!\n";
}

// Expense History
void expenseHistory() {
    cout << "\nAmount  Category  Description  Date\n";
    for (auto record : expenses) {
        cout << record.amount << "\t" << record.category << "\t"
             << record.description << "\t"
             << record.day << "-" << record.month << "-" << record.year << endl;
    }
}

// Last Days Report
void lastDaysReport(int days)
{
    int currentDay, currentMonth, currentYear;
    cout << "Enter Current Date (DD MM YYYY): ";
    cin >> currentDay >> currentMonth >> currentYear;

    double total = 0;

    cout << "\nExpenses in last " << days << " days:\n";
    cout << "Amount   Category   Description   Date\n";

    bool found = false; // check if any expense exists

    // Convert current date to total days
    int currentTotalDays = currentYear * 365 + currentMonth * 30 + currentDay;

    for (auto record : expenses)
    {
        // Convert record date to total days
        int recordTotalDays = record.year * 365 + record.month * 30 + record.day;

        if (recordTotalDays <= currentTotalDays &&
            recordTotalDays >= currentTotalDays - days)
        {
            cout << record.amount << "   "
                 << record.category << "   "
                 << record.description << "   "
                 << record.day << "-" << record.month << "-" << record.year
                 << endl;

            // Add to total
            total += record.amount;
            found = true;
        }
    }

    if (found == false)
        cout << "No expenses found in the last " << days << " days.\n";

    cout << "\nTotal Spending: " << total << endl;
}

//This Month Report
void thisMonthReport()
{
    int month;
    cout << "Enter Month (MM): ";
    cin >> month;

    double total = 0;
    map<int, double> daily;

    cout << "\nExpenses of this month:\n";
    cout << "Amount   Category   Description   Date\n";

    for (auto record : expenses)
    {
        if (record.month == month)
        {
            // show each expense
            cout << record.amount << "   "
                 << record.category << "   "
                 << record.description << "   "
                 << record.day << "-" << record.month << "-" << record.year
                 << endl;

            // calculations
            total += record.amount;
            daily[record.day] += record.amount;
        }
    }

    int maxDay = 0;
    double maxAmount = 0;

    for (auto day : daily)
    {
        if (day.second > maxAmount)
        {
            maxAmount = day.second;
            maxDay = day.first;
        }
    }

    cout << "\nMonthly Total: " << total << endl;
    cout << "Highest Spending Day: " << maxDay
         << " (Amount: " << maxAmount << ")" << endl;
    cout << "Savings: " << (monthlyIncome - total) << endl;
}

// Date Range Report
void dateRangeReport()
{
    int startDay, startMonth, startYear;
    int endDay, endMonth, endYear;

    cout << "Start Date (DD MM YYYY): ";
    cin >> startDay >> startMonth >> startYear;

    cout << "End Date (DD MM YYYY): ";
    cin >> endDay >> endMonth >> endYear;

    // Convert dates to comparable numbers
    int startTotalDays = startYear * 365 + startMonth * 30 + startDay;
    int endTotalDays   = endYear * 365 + endMonth * 30 + endDay;

    double total = 0;

    cout << "\nExpenses in the selected date range:\n";
    cout << "Amount   Category   Description   Date\n";

    for (auto record : expenses)
    {
        int recordTotalDays = record.year * 365 + record.month * 30 + record.day;

        if (recordTotalDays >= startTotalDays && recordTotalDays <= endTotalDays)
        {
            // Show each expense
            cout << record.amount << "   "
                 << record.category << "   "
                 << record.description << "   "
                 << record.day << "-" << record.month << "-" << record.year
                 << endl;

            // Add to total
            total += record.amount;
        }
    }

    cout << "\nDate Range Total: " << total << endl;

    // Optional: if no expense found
    if (total == 0)
        cout << "No expenses found in this date range.\n";
}


// Category Report
void categoryReport() {
    cout << "\nCategory Wise Spending:\n";
    for (auto category : categoryTotal) {
        cout << category.first << " : " << category.second << endl;
    }
}

//Daily Report
void dailyReport() {
    int day, month, year;
    cout << "Enter Date (DD MM YYYY): ";
    cin >> day >> month >> year;

    double total = 0;

    cout << "\nExpenses on " << day << "-" << month << "-" << year << ":\n";
    cout << "Amount   Category   Description\n";

    bool found = false; // check if any expense exists

    for (auto record : expenses) {
        if (record.day == day && record.month == month && record.year == year) {
            cout << record.amount << "   "
                 << record.category << "   "
                 << record.description << endl;

            // Add to total
            total += record.amount;
            found = true;
        }
    }

    if (found==false)
        cout << "No expenses found on this date.\n";

    cout << "\nDaily Total: " << total << endl;
}

// Weekly Report
void weeklyReport() {
    lastDaysReport(7);
}

//Compare Months
void compareMonths() {
    int m1, m2;
    double total1 = 0, total2 = 0;

    cout << "Month 1 (MM): ";
    cin >> m1;
    cout << "Month 2 (MM): ";
    cin >> m2;

    for (auto record : expenses) {
        if (record.month == m1) total1 += record.amount;
        if (record.month == m2) total2 += record.amount;
    }

    cout << "Month " << m1 << ": " << total1 << endl;
    cout << "Month " << m2 << ": " << total2 << endl;

    // Compare totals
    if (total1 > total2)
        cout << "Month " << m1 << " has more expenses." << endl;
    else if (total2 > total1)
        cout << "Month " << m2 << " has more expenses." << endl;
    else
        cout << "Both months have equal expenses." << endl;
}


// Main Function
int main() {
    
    loadExpenses();

    cout << "Enter Monthly Income: ";
    cin >> monthlyIncome;

    int choice;
    do {
        cout << "\n===== EXPENSE SYSTEM =====\n";
        cout << "1 Add Expense\n";
        cout << "2 Expense History\n";
        cout << "3 Last 7 Days\n";
        cout << "4 Last 30 Days\n";
        cout << "5 Month Report\n";
        cout << "6 Date Range Report\n";
        cout << "7 Category Report\n";
        cout << "8 Daily Report\n";
        cout << "9 Weekly Report\n";
        cout << "10 Compare Months\n";
        cout << "11 Custom Days Report\n";  // New option
        cout << "0 Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
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
            case 11: {
                int days;
                cout << "Enter number of days: ";
                cin >> days;
                lastDaysReport(days);  // Call with user input
                break;
            }
        }
    } while (choice != 0);

    saveExpenses();
    cout << "Bye Bye!\n";
    return 0;
}
