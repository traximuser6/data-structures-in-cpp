#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

class BankAccount {
private:
    double balance;
    std::string accountHolder;
    const std::string accountNumber;

    // Validate amount for deposit/withdrawal
    static bool isValidAmount(double amount) {
        return amount > 0 && amount < std::numeric_limits<double>::max();
    }

public:
    // Constructor with account holder and opening balance
    BankAccount(const std::string &holder, double openingBalance, const std::string &accNum)
        : balance(openingBalance >= 0 ? openingBalance : 0),
          accountHolder(holder),
          accountNumber(accNum) {
        if (openingBalance < 0) {
            std::cout << "Warning: Negative opening balance set to 0\n";
        }
    }

    // Deposit method
    bool deposit(double amount) {
        if (!isValidAmount(amount)) {
            std::cout << "Error: Invalid deposit amount (" << amount << "). Must be positive and reasonable.\n";
            return false;
        }
        balance += amount;
        return true;
    }

    // Withdraw method
    bool withdraw(double amount) {
        if (!isValidAmount(amount)) {
            std::cout << "Error: Invalid withdrawal amount (" << amount << "). Must be positive and reasonable.\n";
            return false;
        }
        if (amount > balance) {
            std::cout << "Error: Insufficient funds! Current balance: " << balance << "\n";
            return false;
        }
        balance -= amount;
        return true;
    }

    // Display account details
    void display() const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Account Holder: " << accountHolder << "\n";
        std::cout << "Account Number: " << accountNumber << "\n";
        std::cout << "Current Balance: $" << balance << "\n";
    }

    // Get current balance
    double getBalance() const {
        return balance;
    }

    // Get account holder
    std::string getAccountHolder() const {
        return accountHolder;
    }

    // Get account number
    std::string getAccountNumber() const {
        return accountNumber;
    }
};

// Function to clear input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    // Initialize account with sample data
    BankAccount ba1("John Doe", 100.00, "ACC123456");

    std::cout << "=== Initial Account State ===\n";
    ba1.display();

    // Interactive transaction loop
    while (true) {
        std::cout << "\n=== Transaction Menu ===\n";
        std::cout << "1. Deposit\n";
        std::cout << "2. Withdraw\n";
        std::cout << "3. Display Balance\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice (1-4): ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "Error: Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }

        if (choice == 4) {
            std::cout << "Exiting program. Final account state:\n";
            ba1.display();
            break;
        }

        switch (choice) {
            case 1: {
                // Deposit
                std::cout << "Enter deposit amount: $";
                double amount;
                if (!(std::cin >> amount)) {
                    std::cout << "Error: Invalid amount entered.\n";
                    clearInputBuffer();
                    continue;
                }
                if (ba1.deposit(amount)) {
                    std::cout << "Successfully deposited $" << std::fixed << std::setprecision(2) << amount << "\n";
                }
                break;
            }
            case 2: {
                // Withdraw
                std::cout << "Enter withdrawal amount: $";
                double amount;
                if (!(std::cin >> amount)) {
                    std::cout << "Error: Invalid amount entered.\n";
                    clearInputBuffer();
                    continue;
                }
                if (ba1.withdraw(amount)) {
                    std::cout << "Successfully withdrew $" << std::fixed << std::setprecision(2) << amount << "\n";
                }
                break;
            }
            case 3: // Display
                ba1.display();
                break;
            default:
                std::cout << "Error: Invalid choice. Please select 1-4.\n";
                break;
        }
        clearInputBuffer();
    }

    return 0;
}
