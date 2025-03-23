#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

class Customer
{
private:
    int id;
    string name, address;

public:
    Customer(int id, string name, string address) : id(id), name(name), address(address) {}
    void showInfo() const;
};

class Account
{
private:
    double balance;
    int accNum;
    string accountTitle, password;

public:
    Account(int bal, int ac_id, string acc_title, string pass) : balance(bal), accNum(ac_id), accountTitle(acc_title), password(pass) {}

    void deposit(double amount);
    bool withdraw(double amount);
    void showBalance() const;
    bool authenticate(const string &enteredPass) const { return enteredPass == password; }
    double getBalance() const { return balance; }
    friend class Bank;
    friend class Transaction;
};

class Transaction
{
public:
    static void recordTransaction(int acNum, const string &type, double amount);
};

class Bank
{
private:
    vector<Customer> customers;
    vector<Account> accounts;

public:
    void addCustomer();
    void createAccount();
    void makeTransaction();
    void checkBalance() const;
    void checkTransactionHistory() const;
};

void Transaction::recordTransaction(int acNum, const string &type, double amount)
{
    ofstream file("Transaction.txt", ios::app);
    if (file.is_open())
    {
        file << "Account Number: " << acNum << " Type: " << type << " Amount: " << amount << endl;
        file.close();
    }
    else
    {
        cout << "Error: Could not open transaction file!" << endl;
    }
}

void Account::deposit(double amount)
{
    balance += amount;
}

bool Account::withdraw(double amount)
{
    if (balance >= amount)
    {
        balance -= amount;
        return true;
    }
    else
    {
        cout << "Insufficient Balance..." << endl;
        return false;
    }
}

void Customer::showInfo() const
{
    cout << "Customer ID: " << id << "\nName: " << name << "\nAddress: " << address << endl;
}

void Account::showBalance() const
{
    cout << "Balance: " << balance << endl;
}

void Bank::addCustomer()
{
    int id;
    string name, address;

    cout << "Enter Customer ID: ";
    while (!(cin >> id) || id < 0)
    {
        cout << "Invalid ID! Enter a valid numeric ID...\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore();
    cout << "Enter Name: ";
    getline(cin, name);

    cout << "Enter City: ";
    getline(cin, address);

    ofstream file("Customers.txt", ios::app);
    file << "Customer ID: " << id << " Name: " << name << " Address: " << address << endl;
    file.close();

    customers.emplace_back(id, name, address);
    cout << "Customer added successfully..." << endl;
}

void Bank::createAccount()
{
    int bal, acId;
    string accTitle, password;

    cout << "Enter Account ID: ";
    while (!(cin >> acId) || acId <= 0)
    {
        cout << "Invalid Account ID! Enter Again..." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Enter Account Balance: ";
    while (!(cin >> bal) || bal <= 0)
    {
        cout << "Invalid Input! Enter Again..." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore();
    cout << "Enter Account Title: ";
    getline(cin, accTitle);

    cout << "Set Account Password: ";
    getline(cin, password);

    ofstream file("Accounts.txt", ios::app);
    file << "Account Number: " << acId << " Account Title: " << accTitle << " Balance: " << bal << endl;
    file.close();

    ofstream passFile("Passwords.txt", ios::app);
    passFile << acId << " " << password << endl;
    passFile.close();

    accounts.emplace_back(bal, acId, accTitle, password);
    cout << "Account Created Successfully..." << endl;
}

void Bank::makeTransaction()
{
    int acNum;
    string enteredPass;
    double amount;
    char type;

    cout << "Enter Account Number: ";
    cin >> acNum;
    cin.ignore();
    cout << "Enter Account Password: ";
    getline(cin, enteredPass);

    for (auto &acc : accounts)
    {
        if (acc.accNum == acNum && acc.authenticate(enteredPass))
        {
            cout << "Enter transaction type (D for deposit, W for withdraw): ";
            cin >> type;
            cout << "Enter amount: ";
            cin >> amount;

            if (type == 'D' || type == 'd')
            {
                acc.deposit(amount);
                Transaction::recordTransaction(acNum, "Deposit", amount);
                cout << "Amount Deposited Successfully." << endl;
            }
            else if (type == 'W' || type == 'w')
            {
                if (acc.withdraw(amount))
                {
                    Transaction::recordTransaction(acNum, "Withdraw", amount);
                    cout << "Amount Withdrawn Successfully." << endl;
                }
                else
                {
                    cout << "Transaction failed due to insufficient balance." << endl;
                }
            }
            else
            {
                cout << "Invalid transaction type!" << endl;
            }
            return;
        }
    }
    cout << "Invalid account number or password!" << endl;
}

void Bank::checkBalance() const
{
    int acNum;
    string enteredPass;

    cout << "Enter Account Number: ";
    cin >> acNum;
    cin.ignore();
    cout << "Enter Account Password: ";
    getline(cin, enteredPass);

    for (const auto &acc : accounts)
    {
        if (acc.accNum == acNum && acc.authenticate(enteredPass))
        {
            cout << "Your current balance is: " << acc.getBalance() << endl;
            return;
        }
    }
    cout << "Invalid account number or password!" << endl;
}

void Bank::checkTransactionHistory() const
{
    int acNum;
    cout << "Enter Account Number: ";
    cin >> acNum;

    ifstream file("Transaction.txt");
    if (!file.is_open())
    {
        cout << "Error: Could not open transaction file!" << endl;
        return;
    }

    string line;
    bool found = false;
    cout << "\nTransaction History for Account " << acNum << ":\n";
    while (getline(file, line))
    {
        if (line.find("Account Number: " + to_string(acNum)) != string::npos)
        {
            cout << line << endl;
            found = true;
        }
    }
    file.close();

    if (!found)
    {
        cout << "No transaction history found for this account.\n";
    }
}

int main()
{
    Bank myBank;
    int choice;

    while (true)
    {
        cout << "\nBank System Menu:\n";
        cout << "1. Add Customer\n";
        cout << "2. Create Account\n";
        cout << "3. Make Transaction\n";
        cout << "4. Check Balance\n";
        cout << "5. Show Transaction History\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            myBank.addCustomer();
            break;
        case 2:
            myBank.createAccount();
            break;
        case 3:
            myBank.makeTransaction();
            break;
        case 4:
            myBank.checkBalance();
            break;
        case 5:
            myBank.checkTransactionHistory();
            break;
        case 6:
            cout << "Exiting program...\n";
            return 0;
        default:
            cout << "Invalid choice! Try again.\n";
        }
    }
    return 0;
}