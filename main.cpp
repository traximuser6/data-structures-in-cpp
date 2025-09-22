#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

class BankAccount {
    double balance;
    std::string accountHolder;
    const std::string accountNumber;

    // Validate amount for deposit/withdrawal
    static bool isValidAmount(const double amount) noexcept {
        return amount > 0.0 && amount < std::numeric_limits<double>::max();
    }

public:
    // Constructor
    BankAccount(std::string holder, const double openingBalance, std::string accNum)
        : balance(openingBalance >= 0 ? openingBalance : 0.0),
          accountHolder(std::move(holder)),
          accountNumber(std::move(accNum)) {
        if (openingBalance < 0) {
            std::cerr << "Warning: Negative opening balance set to $0.00\n";
        }
    }

    // Deposit method
    [[nodiscard]] bool deposit(const double amount) {
        if (!isValidAmount(amount)) {
            std::cerr << "Error: Invalid deposit amount ($" << amount << ").\n";
            return false;
        }
        balance += amount;
        return true;
    }

    // Withdraw method
    [[nodiscard]] bool withdraw(const double amount) {
        if (!isValidAmount(amount)) {
            std::cerr << "Error: Invalid withdrawal amount ($" << amount << ").\n";
            return false;
        }
        if (amount > balance) {
            std::cerr << "Error: Insufficient funds! Balance: $" << balance << "\n";
            return false;
        }
        balance -= amount;
        return true;
    }

    // Display account details
    void display() const {
        std::cout << std::fixed << std::setprecision(2)
                << "\n=== Account Summary ===\n"
                << "Holder : " << accountHolder << '\n'
                << "Number : " << accountNumber << '\n'
                << "Balance: $" << balance << '\n';
    }

    // Getters
    [[nodiscard]] double getBalance() const noexcept { return balance; }
    [[nodiscard]] const std::string &getAccountHolder() const noexcept { return accountHolder; }
    [[nodiscard]] const std::string &getAccountNumber() const noexcept { return accountNumber; }
};

// Helper: safely read a double value
double readDouble(const std::string &prompt) {
    double value{};
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) return value;

        std::cerr << "Error: Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Helper: safely read an int value
int readInt(const std::string &prompt, int min, int max) {
    int value{};
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= min && value <= max) return value;

        std::cerr << "Error: Enter a number between " << min << " and " << max << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    BankAccount account("John Doe", 100.00, "ACC123456");

    std::cout << "Welcome to the Bank Account Manager\n";
    account.display();

    while (true) {
        std::cout << "\n=== Transaction Menu ===\n"
                << "1. Deposit\n"
                << "2. Withdraw\n"
                << "3. Display Account\n"
                << "4. Exit\n";

        int choice = readInt("Enter choice (1-4): ", 1, 4);

        if (choice == 4) {
            std::cout << "Exiting. Final account state:\n";
            account.display();
            break;
        }

        switch (choice) {
            case 1:
                if (const double amount = readDouble("Enter deposit amount: $");
                    account.deposit(amount)) {
                    std::cout << "Deposited $" << std::fixed << std::setprecision(2) << amount << '\n';
                }
                break;

            case 2:
                if (const double amount = readDouble("Enter withdrawal amount: $");
                    account.withdraw(amount)) {
                    std::cout << "Withdrew $" << std::fixed << std::setprecision(2) << amount << '\n';
                }
                break;

            case 3:
                account.display();
                break;

            default:
                std::cerr << "Unexpected error: invalid menu option.\n";
                break;
        }
    }

    return 0;
}
