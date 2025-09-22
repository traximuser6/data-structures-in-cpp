#include <iostream>
using namespace std;

class BankAccount {
private:
    double balance;

public:
    explicit BankAccount(double openingBalance)
        : balance(openingBalance) {
    }

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        } else {
            cout << "Invalid deposit amount!\n";
        }
    }

    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
        } else {
            cout << "Invalid or insufficient funds!\n";
        }
    }

    void display() const {
        cout << "Current balance: " << balance << "\n";
    }
};

int main() {
    BankAccount ba1(100.00); // create account

    cout << "Before transactions\n";
    ba1.display();

    ba1.deposit(74.35); // make deposit
    ba1.withdraw(20.00); // make withdrawal

    cout << "After transactions\n";
    ba1.display();

    return 0;
}
