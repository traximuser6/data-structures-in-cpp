#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <ctime>
#include <iomanip>

class BankAccount {
private:
    double balance;
    std::string accountHolder;
    const std::string accountNumber;
    std::vector<std::string> transactionHistory;

    static bool isValidAmount(double amount) noexcept {
        return amount > 0.0 && amount < 1e9; // Prevent overflow & absurd values
    }

    void logTransaction(const std::string &type, double amount, bool success) {
        std::ostringstream oss;
        std::time_t now = std::time(nullptr);
        oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S")
                << " | " << type << " $" << std::fixed << std::setprecision(2) << amount
                << (success ? "SUCCESS" : "FAILED")
                << " | New balance: $" << balance;
        transactionHistory.push_back(oss.str());
    }

public:
    BankAccount(std::string holder, double openingBalance, std::string accNum)
        : balance(openingBalance >= 0 ? openingBalance : 0.0),
          accountHolder(std::move(holder)),
          accountNumber(std::move(accNum)) {
        if (openingBalance < 0) {
            std::cerr << "Warning: Negative opening balance adjusted to $0.00\n";
        }
        logTransaction("OPEN ACCOUNT", balance, true);
    }

    bool deposit(double amount) {
        if (!isValidAmount(amount)) {
            std::cerr << "Invalid deposit amount: $" << amount << '\n';
            logTransaction("DEPOSIT", amount, false);
            return false;
        }
        balance += amount;
        logTransaction("DEPOSIT", amount, true);
        return true;
    }

    bool withdraw(double amount) {
        if (!isValidAmount(amount)) {
            std::cerr << "Invalid withdrawal amount: $" << amount << '\n';
            logTransaction("WITHDRAW", amount, false);
            return false;
        }
        if (amount > balance) {
            std::cerr << "Insufficient funds! Requested: $" << amount
                    << ", Available: $" << balance << '\n';
            logTransaction("WITHDRAW", amount, false);
            return false;
        }
        balance -= amount;
        logTransaction("WITHDRAW", amount, true);
        return true;
    }

    bool transfer(BankAccount &to, double amount) {
        if (this == &to) {
            std::cerr << "Error: Cannot transfer to the same account.\n";
            return false;
        }
        if (withdraw(amount)) {
            if (to.deposit(amount)) {
                logTransaction("TRANSFER OUT to " + to.accountNumber, amount, true);
                return true;
            } else {
                deposit(amount); // rollback
                logTransaction("TRANSFER FAILED (rollback)", amount, false);
            }
        }
        return false;
    }

    void display() const {
        std::cout << "\n╔════════════════════════════════════╗\n"
                << "║         ACCOUNT SUMMARY            ║\n"
                << "╠════════════════════════════════════╣\n"
                << "║ Holder   : " << std::setw(22) << std::left << accountHolder << " ║\n"
                << "║ Number   : " << accountNumber << std::string(19 - accountNumber.length(), ' ') << " ║\n"
                << "║ Balance  : $" << std::setw(20) << std::fixed << balance << " ║\n"
                << "╚════════════════════════════════════╝\n";
    }

    void showHistory() const {
        std::cout << "\n=== Transaction History (" << transactionHistory.size() << ") ===\n";
        if (transactionHistory.empty()) {
            std::cout << "No transactions yet.\n";
            return;
        }
        for (const auto &t: transactionHistory) {
            std::cout << t << '\n';
        }
    }

    // Getters
    [[nodiscard]] double getBalance() const noexcept { return balance; }
    [[nodiscard]] const std::string &getAccountNumber() const noexcept { return accountNumber; }
};

// Input helpers
double readDouble(const std::string &prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= 0) return value;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Please enter a valid positive number.\n";
    }
}

int readInt(const std::string &prompt, int min, int max) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) return value;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Enter a number between " << min << " and " << max << ".\n";
    }
}

int main() {
    BankAccount acc1("Alice Johnson", 500.00, "ACC1001");
    BankAccount acc2("Bob Smith", 200.00, "ACC1002");

    BankAccount *current = &acc1;

    std::cout << "Bank Account Manager v2.0\n";
    current->display();

    while (true) {
        std::cout << "\n1. Deposit\n2. Withdraw\n3. Transfer\n"
                << "4. Show Balance\n5. Transaction History\n"
                << "6. Switch Account\n7. Exit\n";

        int choice = readInt("Choose (1-7): ", 1, 7);

        if (choice == 7) {
            std::cout << "Thank you for banking with us!\n";
            current->display();
            break;
        }

        switch (choice) {
            case 1: {
                double amt = readDouble("Deposit amount: $");
                if (current->deposit(amt))
                    std::cout << "Successfully deposited $" << amt << '\n';
                break;
            }
            case 2: {
                double amt = readDouble("Withdraw amount: $");
                if (current->withdraw(amt))
                    std::cout << "Successfully withdrew $" << amt << '\n';
                break;
            }
            case 3: {
                double amt = readDouble("Transfer amount: $");
                BankAccount &target = (current == &acc1 ? acc2 : acc1);
                if (current->transfer(target, amt)) {
                    std::cout << "Transferred $" << amt << " to " << target.getAccountNumber() << '\n';
                } else {
                    std::cout << "Transfer failed.\n";
                }
                break;
            }
            case 4: current->display();
                break;
            case 5: current->showHistory();
                break;
            case 6:
                current = (current == &acc1) ? &acc2 : &acc1;
                std::cout << "Switched to account " << current->getAccountNumber() << '\n';
                current->display();
                break;
            default: ;
        }
    }
    return 0;
}
