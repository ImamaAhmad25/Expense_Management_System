//#include <iostream>
//#include <vector>
//#include <map>
//#include <fstream>
//#include <string>
//using namespace std;
#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <limits>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

// ----------------- User Authentication -----------------
class UserAuth {
public:
    bool login() {
        string inputUser, inputPass;
        cout << "Username: ";
        cin >> inputUser;
        cout << "Password: ";
        cin >> inputPass;

        ifstream file("users.txt");
        if (!file) {
            cout << "users.txt file not found\n";
            return false;
        }

        string fileUser, filePass;
        while (file >> fileUser >> filePass) {
            if (fileUser == inputUser && filePass == inputPass) {
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    void registerUser() {
        string u, p;
        cout << "New Username: ";
        cin >> u;
        cout << "New Password: ";
        cin >> p;

        ofstream file("users.txt", ios::app);
        file << u << " " << p << endl;
        file.close();

        cout << "User Registered Successfully\n";
    }
};

// ----------------- Category Management -----------------
class CategoryManager {
private:
    vector<string> categories;
    string filename;

public:
    CategoryManager() { filename = "categories.txt"; }

    void loadCategories() {
        ifstream file("categories.txt");
        string cat;
        if (file.is_open()) {
            while (getline(file, cat))
                categories.push_back(cat);
            file.close();
        }

        if (categories.empty())
            addPredefinedCategories();
    }

    void addPredefinedCategories() {
        ofstream file("categories.txt", ios::app);
        string predefined[5] = {"Food", "Transport", "Shopping", "Bills", "Entertainment"};
        for (int i = 0; i < 5; i++) {
            categories.push_back(predefined[i]);
            file << predefined[i] << endl;
        }
        file.close();
    }

    void showCategories() {
        cout << "\nAvailable Categories:\n";
        for (int i = 0; i < (int)categories.size(); i++)
            cout << i + 1 << ". " << categories[i] << endl;
    }

    void addCategory() {
        string newCat;
        cin.ignore();
        cout << "Enter New Category Name: ";
        getline(cin, newCat);
        categories.push_back(newCat);

        ofstream file("categories.txt", ios::app);
        file << newCat << endl;
        file.close();

        cout << "Category added successfully!\n";
    }

    string selectCategory(bool showList) {
        if (showList) showCategories();

        int choice;
        while (true) {
            cout << "Select Category (number): ";
            cin >> choice;

            if (cin.fail() || choice < 1 || choice > (int)categories.size()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid category. Please select a valid number.\n";
            } else
                break;
        }
        return categories[choice - 1];
    }
};

// ----------------- Expense/Income -----------------
class Expense {
public:
    string date, category, note, paymentMode;
    float amount;
    bool isIncome;

    void input(CategoryManager &cm) {
        cout << "Enter Date (DD/MM/YYYY): ";
        cin >> date;

        if (!isIncome) {
            cm.showCategories();
            char opt;
            cout << "Do you want to add a new category? (y/n): ";
            cin >> opt;
            if (opt == 'y' || opt == 'Y') {
                cm.addCategory();
                category = cm.selectCategory(true);
            } else {
                category = cm.selectCategory(false);
            }
        } else {
            cin.ignore();
            cout << "Enter Income Category: ";
            getline(cin, category);
        }

        amount = getValidAmount();
        paymentMode = isIncome ? "Income" : getPaymentMode();

        cout << "Enter Note: ";
        cin.ignore();
        getline(cin, note);
    }

    void saveToFile() {
        if (isIncome) {
            ofstream file("income.txt", ios::app);
            file << date << " " << category << " " << amount << " " << paymentMode << " " << note << endl;
            file.close();
        } else {
            ofstream file("expenses.txt", ios::app);
            file << date << " " << category << " " << amount << " " << paymentMode << " " << note << endl;
            file.close();
        }
    }

    void display() {
        cout << date << " | " << category << " | " << amount << " | " << paymentMode << " | " << note << endl;
    }

private:
    float getValidAmount() {
        float amt;
        while (true) {
            cout << "Enter Amount: ";
            cin >> amt;
            if (cin.fail() || amt <= 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid amount. Enter a positive number.\n";
            } else break;
        }
        return amt;
    }

    string getPaymentMode() {
        string mode;
        while (true) {
            cout << "Enter Payment Mode (Cash/Card/UPI): ";
            cin >> mode;
            if (mode == "Cash" || mode == "Card" || mode == "UPI") break;
            else cout << "Invalid payment mode. Enter again.\n";
        }
        return mode;
    }
};

// ----------------- Comparator Functions -----------------
bool comparePair(const pair<string,int> &a, const pair<string,int> &b) { return a.second > b.second; }
bool compareExpenseAsc(const Expense &a, const Expense &b) { return a.amount < b.amount; }
bool compareExpenseDesc(const Expense &a, const Expense &b) { return a.amount > b.amount; }

// ----------------- Expense Manager -----------------
class ExpenseManager {
private:
    vector<Expense> expenses;
    vector<Expense> incomes;
    stack<Expense> undoStack;
    stack<Expense> redoStack;

public:
    float budgetLimit; // Monthly budget limit

    void addTransaction(CategoryManager &cm) {
        int typeChoice;
        cout << "\n1. Add Expense\n2. Add Income\nChoice: ";
        cin >> typeChoice;

        Expense e;
        if (typeChoice == 1) e.isIncome = false;
        else if (typeChoice == 2) e.isIncome = true;
        else {
            cout << "Invalid choice!\n";
            return;
        }

        e.input(cm);

        if (!e.isIncome) {
            undoStack.push(e);
            while (!redoStack.empty()) redoStack.pop();
            expenses.push_back(e);
            e.saveToFile();

            cout << "\nExpense saved successfully!\n";
            showRecentTransaction();
            showRemainingBalance();

            char choice;
            cout << "Do you want to Undo or Redo last expense? (u/r/n): ";
            cin >> choice;
            if (choice == 'u' || choice == 'U') undo();
            else if (choice == 'r' || choice == 'R') redo();

        } else {
            incomes.push_back(e);
            e.saveToFile();
            cout << "\nIncome saved successfully!\n";
        }
    }

    void showRecentTransaction() {
        if (!undoStack.empty()) {
            cout << "\nMost Recent Expense:\n";
            undoStack.top().display();
        } else {
            cout << "No recent expense.\n";
        }
    }

    void showRemainingBalance() {
        float totalIncome = 0, totalExpense = 0;
        map<string,float> dailyExpenses; // For daily average
        string line, date, cat, mode, note;
        float amt;

        // Income
        ifstream fileIncome("income.txt");
        while (getline(fileIncome, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            ss >> date >> cat >> amt >> mode;
            getline(ss, note);
            totalIncome += amt;
        }
        fileIncome.close();

        // Expenses
        ifstream fileExpense("expenses.txt");
        while (getline(fileExpense, line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            ss >> date >> cat >> amt >> mode;
            getline(ss, note);
            totalExpense += amt;
            dailyExpenses[date] += amt;
        }
        fileExpense.close();

        float remaining = totalIncome - totalExpense;
        cout << "\nRemaining Balance: " << remaining << endl;

        // Budget Warning
        if(totalExpense > budgetLimit) {
            cout << "WARNING: You have exceeded your budget limit of " << budgetLimit << "!\n";
        }

        // Daily Average Expense (iterator-based)
        float dailyAvg = 0;
        if(!dailyExpenses.empty()) {
            float sumDaily = 0;
            map<string,float>::iterator it;
            for(it = dailyExpenses.begin(); it != dailyExpenses.end(); ++it)
                sumDaily += it->second;
            dailyAvg = sumDaily / dailyExpenses.size();
        }
        cout << "Daily Average Expense: " << dailyAvg << endl;
    }

    void undo() {
        if (undoStack.empty()) {
            cout << "Nothing to undo.\n";
            return;
        }
        Expense e = undoStack.top();
        undoStack.pop();
        redoStack.push(e);

        if (!expenses.empty()) expenses.pop_back();
        saveExpensesToFile();
        cout << "Last expense undone.\n";
        showRemainingBalance();
    }

    void redo() {
        if (redoStack.empty()) {
            cout << "Nothing to redo.\n";
            return;
        }
        Expense e = redoStack.top();
        redoStack.pop();
        expenses.push_back(e);
        undoStack.push(e);
        saveExpensesToFile();
        cout << "Expense redone.\n";
        showRemainingBalance();
    }

    void saveExpensesToFile() {
        ofstream file("expenses.txt");
        for (size_t i = 0; i < expenses.size(); i++) {
            file << expenses[i].date << " " << expenses[i].category << " "
                 << expenses[i].amount << " " << expenses[i].paymentMode << " "
                 << expenses[i].note << endl;
        }
        file.close();
    }

    void viewAllExpenses() {
        cout << "\nComplete Expense History:\n";
        ifstream file("expenses.txt");
        string line;
        while (getline(file, line)) cout << line << endl;
        file.close();
    }

    void showTopCategories() {
        map<string,int> catCount;
        ifstream file("expenses.txt");
        string date, cat, mode, note;
        float amt;
        while(file >> date >> cat >> amt >> mode) {
            getline(file, note);
            catCount[cat]++;
        }
        file.close();

        vector<pair<string,int> > vec;
        map<string,int>::iterator it;
        for(it = catCount.begin(); it != catCount.end(); ++it)
            vec.push_back(make_pair(it->first,it->second));

        sort(vec.begin(), vec.end(), comparePair);

        cout << "\nTop 3 Expense Categories:\n";
        for(int i = 0; i < (int)vec.size() && i < 3; i++)
            cout << vec[i].first << " - " << vec[i].second << " times\n";
    }

    void showIncomeDetails() {
        float totalIncome = 0;
        map<string,float> incomeByCategory;
        ifstream file("income.txt");
        string line;
        cout << "\n------ Income Transactions ------\n";
        while(getline(file,line)) {
            if(line.empty()) continue;
            stringstream ss(line);
            string date, cat, mode, note;
            float amount;
            ss >> date >> cat >> amount >> mode;
            getline(ss, note);
            totalIncome += amount;
            incomeByCategory[cat] += amount;
            cout << date << " | " << cat << " | " << amount << " | " << mode << " |" << note << endl;
        }
        file.close();

        cout << "\n------ Income Summary ------\n";
        cout << "Total Income: " << totalIncome << endl;
        map<string,float>::iterator it;
        for(it = incomeByCategory.begin(); it != incomeByCategory.end(); ++it)
            cout << it->first << " : " << it->second << endl;
    }

    void viewFilteredExpenses() {
        int choice;
        cout << "\nFilter by:\n1. Category\n2. Date\n3. Amount Range\nChoice: ";
        cin >> choice;

        ifstream file("expenses.txt");
        string date, category, note, mode;
        float amount;
        bool found = false;

        switch(choice) {
            case 1: {
                string catFilter;
                cout << "Enter category: ";
                cin >> catFilter;
                while(file >> date >> category >> amount >> mode) {
                    getline(file, note);
                    if(category == catFilter) {
                        cout << date << " | " << category << " | " << amount << " | " << mode << " | " << note << endl;
                        found = true;
                    }
                }
                break;
            }
            case 2: {
                string dateFilter;
                cout << "Enter date (DD/MM/YYYY): ";
                cin >> dateFilter;
                while(file >> date >> category >> amount >> mode) {
                    getline(file, note);
                    if(date == dateFilter) {
                        cout << date << " | " << category << " | " << amount << " | " << mode << " | " << note << endl;
                        found = true;
                    }
                }
                break;
            }
            case 3: {
                float minAmt, maxAmt;
                cout << "Enter minimum amount: ";
                cin >> minAmt;
                cout << "Enter maximum amount: ";
                cin >> maxAmt;
                while(file >> date >> category >> amount >> mode) {
                    getline(file, note);
                    if(amount >= minAmt && amount <= maxAmt) {
                        cout << date << " | " << category << " | " << amount << " | " << mode << " | " << note << endl;
                        found = true;
                    }
                }
                break;
            }
            default: cout << "Invalid filter choice\n";
        }
        if(!found) cout << "No records found for this filter.\n";
        file.close();
    }

    void viewSortedExpenses() {
        vector<Expense> allExpenses;
        ifstream file("expenses.txt");
        string date, cat, note, mode;
        float amount;

        while(file >> date >> cat >> amount >> mode) {
            getline(file, note);
            Expense e;
            e.date = date;
            e.category = cat;
            e.amount = amount;
            e.paymentMode = mode;
            e.note = note;
            allExpenses.push_back(e);
        }
        file.close();

        int choice;
        cout << "\nSort by Amount:\n1. Ascending\n2. Descending\nChoice: ";
        cin >> choice;

        if(choice == 1)
            sort(allExpenses.begin(), allExpenses.end(), compareExpenseAsc);
        else
            sort(allExpenses.begin(), allExpenses.end(), compareExpenseDesc);

        cout << "\nSorted Expenses:\n";
        for(size_t i=0;i<allExpenses.size();i++)
            allExpenses[i].display();
    }
};

// ----------------- Main -----------------
int main() {
    UserAuth auth;
    int choice;

    cout << "1. Register\n2. Login\nChoice: ";
    cin >> choice;

    if(choice==1) auth.registerUser();

    if(!auth.login()) {
        cout << "Login Failed\n";
        return 0;
    }

    cout << "\nLogin Successful!\n";

    CategoryManager cm;
    cm.loadCategories();

    ExpenseManager em;

    cout << "Enter your monthly budget limit: ";
    cin >> em.budgetLimit;

    int option;
    do {
        cout << "\n1. Add Expense/Income\n2. Undo Last Expense\n3. Redo Last Expense\n4. Show Recent Expense\n"
             << "5. View Complete Expense History\n6. Top 3 Expense Categories\n7. Show Income Details\n"
             << "8. Filter Expenses\n9. Sort Expenses by Amount\n10. Exit\nChoice: ";
        cin >> option;

        switch(option) {
            case 1: em.addTransaction(cm); break;
            case 2: em.undo(); break;
            case 3: em.redo(); break;
            case 4: em.showRecentTransaction(); break;
            case 5: em.viewAllExpenses(); break;
            case 6: em.showTopCategories(); break;
            case 7: em.showIncomeDetails(); break;
            case 8: em.viewFilteredExpenses(); break;
            case 9: em.viewSortedExpenses(); break;
            case 10: cout << "Exiting...\n"; break;
            default: cout << "Invalid option. Try again.\n";
        }
    } while(option!=10);

    return 0;
}
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
