#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <utility>

class BankAccount {
private:
    double balance;

private:
    std::string accountHolder;

private:
    const std::string accountNumber;

    // Validate amount for deposit/withdrawal
    static bool isValidAmount(const double amount) {
        return amount > 0.0 && amount < std::numeric_limits<double>::max();
    }

public:
    // Constructor with account holder and opening balance
    BankAccount(std::string holder, const double openingBalance, std::string accNum)
        : balance(openingBalance >= 0 ? openingBalance : 0.0),
          accountHolder(std::move(holder)),
          accountNumber(std::move(accNum)) {
        if (openingBalance < 0) {
            std::cerr << "Warning: Negative opening balance. Set to $0.00\n";
        }
    }

    // Deposit method
    bool deposit(const double amount) {
        if (!isValidAmount(amount)) {
            std::cerr << "Error: Invalid deposit amount (" << amount << ").\n";
            return false;
        }
        balance += amount;
        return true;
    }

    // Withdraw method
    bool withdraw(const double amount) {
        if (!isValidAmount(amount)) {
            std::cerr << "Error: Invalid withdrawal amount (" << amount << ").\n";
            return false;
        }

        if (amount > balance) {
            std::cerr << "Error: Insufficient funds! Current balance: $" << balance << "\n";
            return false;
        }
        balance -= amount;
        return true;
    }

    // Display account details
    void display() const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\n=== Account Summary ===\n";
        std::cout << "Holder  : " << accountHolder << "\n";
        std::cout << "Number  : " << accountNumber << "\n";
        std::cout << "Balance : $" << balance << "\n";
    }

    // Getters
    [[nodiscard]] double getBalance() const { return balance; }
    [[nodiscard]] const std::string &getAccountHolder() const { return accountHolder; }
    [[nodiscard]] const std::string &getAccountNumber() const { return accountNumber; }
};

// ------------------------------------------------------------
// Helper: safely read a double value from input
double readDouble(const std::string &prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        }
        std::cerr << "Error: Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Helper: safely read an int value from input
int readInt(const std::string &prompt, int min, int max) {
    int choice;
    while (true) {
        std::cout << prompt;
        if (std::cin >> choice && choice >= min && choice <= max) {
            return choice;
        }
        std::cerr << "Error: Enter a number between " << min << " and " << max << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// ------------------------------------------------------------
int main() {
    // Initialize account with sample data
    BankAccount account("John Doe", 100.00, "ACC123456");

    std::cout << "Welcome to the Bank Account Simulator\n";
    account.display();

    while (true) {
        std::cout << "\n=== Transaction Menu ===\n"
                << "1. Deposit\n"
                << "2. Withdraw\n"
                << "3. Display Account\n"
                << "4. Exit\n";

        int choice = readInt("Enter choice (1-4): ", 1, 4);

        if (choice == 4) {
            std::cout << "Exiting program. Final account state:\n";
            account.display();
            break;
        }

        switch (choice) {
            case 1: {
                // Deposit
                double amount = readDouble("Enter deposit amount: $");
                if (account.deposit(amount)) {
                    std::cout << "Successfully deposited $" << std::fixed << std::setprecision(2) << amount << "\n";
                }
                break;
            }
            case 2: {
                // Withdraw
                double amount = readDouble("Enter withdrawal amount: $");
                if (account.withdraw(amount)) {
                    std::cout << "Successfully withdrew $" << std::fixed << std::setprecision(2) << amount << "\n";
                }
                break;
            }
            case 3: // Display
                account.display();
                break;
        }
    }

    return 0;
}
