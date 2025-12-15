#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <ctime>
#include <algorithm>
#include <cmath> // For std::round

// --- Utility Functions ---

/**
 * Robustly reads a non-negative double value from standard input.
 * @param prompt The prompt string.
 * @return The valid double value.
 */
double readDouble(const std::string &prompt) {
    double v;
    std::cout << prompt;
    while (!(std::cin >> v) || v < 0.0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid amount (must be non-negative) → try again: ";
    }
    return v;
}

/**
 * Robustly reads an integer within a specified range from standard input.
 * @param prompt The prompt string.
 * @param lo The minimum allowed value.
 * @param hi The maximum allowed value.
 * @return The valid integer value.
 */
int readInt(const std::string &prompt, int lo, const int hi) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v) || v < lo || v > hi) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Enter " << lo << "-" << hi << ": ";
    }
    return v;
}

// --- BankAccount Class ---

class BankAccount {
private:
    double balance;
    std::string accountHolder;
    const std::string accountNumber;
    std::vector<std::string> transactionHistory;

    // New Features & Optimizations
    const std::string pin; // Added PIN for security
    std::time_t lastActivity; // For interest calculation

    bool isFrozen = false;
    double dailyWithdrawalLimit = 5000.0;
    double dailyWithdrawn = 0.0;
    static constexpr double ANNUAL_INTEREST_RATE = 0.02; // 2% annual rate

    void resetDailyLimitsIfNeeded() {
        static std::time_t lastReset = 0;
        std::time_t now = std::time(nullptr);
        struct tm *lt = std::localtime(&now);

        // Optimization: Check for reset only once per day
        if (now - lastReset > 86400) {
            // Check if more than 24 hours passed
            lt->tm_hour = lt->tm_min = lt->tm_sec = 0;
            std::time_t midnight = std::mktime(lt);

            if (lastReset < midnight) {
                dailyWithdrawn = 0.0;
                lastReset = midnight;
                // Since this function is called on withdraw, it's a good place to update lastActivity
                lastActivity = now;
            }
        }
    }

    [[nodiscard]] static bool isValidAmount(double amount) noexcept {
        // Optimization: Use a reasonable float tolerance check (optional, but good practice)
        return amount > 0.005 && amount <= 1e9;
    }

    void log(const std::string &type, double amount, bool success, const std::string &note = "") {
        std::ostringstream oss;
        std::time_t t = std::time(nullptr);
        // Optimization: Use the system's timezone/locale settings for time format
        oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
                << " | " << std::left << std::setw(12) << type
                << " | $" << std::fixed << std::setprecision(2) << std::setw(10) << amount
                << " | " << (success ? "SUCCESS " : "FAILED  ")
                << " | Bal: $" << std::setw(10) << std::fixed << std::setprecision(2) << balance;
        if (!note.empty()) oss << " | " << note;
        transactionHistory.push_back(oss.str());

        // Update activity time on any successful transaction
        if (success) lastActivity = t;
    }

    static std::string generateAccountNumber() {
        static int seq = 1000;
        return "ACC" + std::to_string(++seq);
    }

public:
    // Added PIN to constructor
    explicit BankAccount(std::string holder, const std::string &accountPin, double opening = 0.0, std::string num = "")
        : balance(opening >= 0 ? opening : 0.0),
          accountHolder(std::move(holder)),
          accountNumber(num.empty() ? generateAccountNumber() : std::move(num)),
          pin(accountPin), // Initialize PIN
          lastActivity(std::time(nullptr)) {
        // Initialize last activity time

        if (opening < 0) std::cerr << "Negative balance corrected to $0.00\n";
        log("OPEN", balance, true);
    }

    // Disallow copy construction and assignment for security and uniqueness
    BankAccount(const BankAccount &) = delete;

    BankAccount &operator=(const BankAccount &) = delete;

    // New Feature: Simple authentication check
    [[nodiscard]] bool authenticate(const std::string &inputPin) const noexcept {
        return pin == inputPin;
    }

    // New Feature: Calculate and apply monthly interest
    void applyInterest() {
        std::time_t now = std::time(nullptr);
        double timeDiff = std::difftime(now, lastActivity);

        // Calculate the number of full months passed (approx)
        int monthsPassed = static_cast<int>(std::floor(timeDiff / (30.0 * 24.0 * 3600.0)));

        if (monthsPassed >= 1 && balance > 0) {
            double monthlyRate = ANNUAL_INTEREST_RATE / 12.0;
            double interestAmount = balance * monthlyRate * monthsPassed;

            // Round to 2 decimal places for accurate banking math
            interestAmount = std::round(interestAmount * 100.0) / 100.0;

            balance += interestAmount;
            log("INTEREST", interestAmount, true, "For " + std::to_string(monthsPassed) + " month(s)");
        }
    }

    bool deposit(double amount) {
        if (isFrozen) {
            log("DEPOSIT", amount, false, "Frozen");
            return false;
        }

        if (!isValidAmount(amount)) {
            log("DEPOSIT", amount, false, "Invalid");
            return false;
        }

        applyInterest(); // Apply interest before transaction
        balance += amount;
        log("DEPOSIT", amount, true);
        return true;
    }

    bool withdraw(double amount) {
        resetDailyLimitsIfNeeded();
        if (isFrozen) {
            log("WITHDRAW", amount, false, "Frozen");
            return false;
        }

        if (!isValidAmount(amount)) {
            log("WITHDRAW", amount, false, "Invalid");
            return false;
        }

        if (dailyWithdrawn + amount > dailyWithdrawalLimit) {
            std::ostringstream oss;
            oss << "Limit $" << std::fixed << std::setprecision(2) << dailyWithdrawalLimit - dailyWithdrawn <<
                    " remaining";
            log("WITHDRAW", amount, false, oss.str());
            return false;
        }

        if (amount > balance) {
            log("WITHDRAW", amount, false, "No funds");
            return false;
        }

        applyInterest(); // Apply interest before transaction
        balance -= amount;
        dailyWithdrawn += amount;
        log("WITHDRAW", amount, true);
        return true;
    }

    bool transfer(BankAccount &to, double amount) {
        // Optimization: Don't allow transfers to frozen accounts
        if (this == &to || to.isFrozen) return false;

        // Optimization: Use a local transaction object or scope guard for better rollback (omitted for minimal bloat)
        if (withdraw(amount)) {
            if (to.deposit(amount)) {
                log("TRANSFER OUT", amount, true, "To " + to.accountNumber);
                to.log("TRANSFER IN", amount, true, "From " + accountNumber);
                return true;
            }
            // Rollback is still necessary if deposit fails for a non-frozen reason
            deposit(amount);
        }
        return false;
    }

    void freeze() {
        isFrozen = true;
        log("FREEZE", 0, true);
    }

    void unfreeze() {
        isFrozen = false;
        log("UNFREEZE", 0, true);
    }

    [[nodiscard]] bool isAccountFrozen() const noexcept { return isFrozen; }

    void display() {
        applyInterest(); // Always check/apply interest before displaying
        std::cout << "\n╔══════════════════════════════════════╗\n"
                << "║            ACCOUNT SUMMARY           ║\n"
                << "╠══════════════════════════════════════╣\n"
                << "║ Holder   : " << std::setw(24) << std::left << accountHolder << " ║\n"
                << "║ Account  : " << accountNumber
                << std::string(std::max(0, 24 - (int) accountNumber.size()), ' ') << " ║\n"
                << "║ Balance  : $" << std::setw(12) << std::fixed << std::setprecision(2) << balance
                << (isFrozen ? " [FROZEN]" : "        ") << std::string(10, ' ') << "║\n"
                << "║ Daily W/D: $" << std::setw(12) << std::fixed << std::setprecision(2) << (
                    dailyWithdrawalLimit - dailyWithdrawn) << " remaining ║\n"
                << "╚══════════════════════════════════════╝\n";
    }

    void showHistory(int n = 0) const {
        std::cout << "\n=== Transaction History";
        if (n > 0) std::cout << " (last " << n << ")";
        std::cout << " ===\n";
        if (transactionHistory.empty()) {
            std::cout << "No transactions.\n";
            return;
        }

        int start = n <= 0 ? 0 : std::max(0, static_cast<int>(transactionHistory.size()) - n);
        for (int i = start; i < static_cast<int>(transactionHistory.size()); ++i)
            std::cout << transactionHistory[i] << '\n';
    }

    // Getters
    [[nodiscard]] double getBalance() const noexcept { return balance; }
    [[nodiscard]] const std::string &getAccountNumber() const noexcept { return accountNumber; }
};

// --- Main Program ---

int main() {
    // Added PINs to the accounts
    BankAccount a1("Alice Johnson", "1234", 1000.0);
    BankAccount a2("Bob Smith", "0000", 300.0);
    BankAccount a3("Charlie Brown", "9999", 5000.0);

    std::vector<BankAccount *> all_accounts = {&a1, &a2, &a3};
    BankAccount *cur = &a1;

    std::cout << "SecureBank Pro v4.0\n";

    // Initial Authentication
    std::cout << "\n--- Welcome, Alice ---\n";
    std::string pin_attempt;
    std::cout << "Enter PIN for ACC" << cur->getAccountNumber() << ": ";
    std::cin >> pin_attempt;

    if (!cur->authenticate(pin_attempt)) {
        std::cout << "Authentication failed. Exiting.\n";
        return 1;
    }

    cur->display();

    while (true) {
        std::cout << "\n1 Deposit  2 Withdraw  3 Transfer  4 Balance\n"
                << "5 History  6 Last 5   7 Switch    8 Freeze/Unfreeze\n"
                << "9 Apply Interest 10 Exit\n> ";
        // Updated range to 10
        const int ch = readInt("", 1, 10);
        if (ch == 10) break;

        switch (ch) {
            case 1: cur->deposit(readDouble("Deposit $"));
                break;
            case 2: cur->withdraw(readDouble("Withdraw $"));
                break;
            case 3: {
                double amt = readDouble("Amount $");
                std::cout << "To: 1.Bob 2.Charlie\n";
                const int t = readInt("", 1, 2);
                BankAccount *target = (t == 1 ? &a2 : &a3);

                // New: Check if the target is frozen before attempting transfer
                if (target->isAccountFrozen()) {
                    std::cout << "Transfer failed: Target account is frozen.\n";
                    break;
                }

                cur->transfer(*target, amt);
                break;
            }
            case 4: cur->display();
                break;
            case 5:
                cur->showHistory();
                break;
            case 6:
                cur->showHistory(5);
                break;
            case 7: {
                std::cout << "1 Alice 2 Bob 3 Charlie\n";
                const int s = readInt("", 1, 3);
                BankAccount *next_account = all_accounts[s - 1];

                std::cout << "Enter PIN for " << next_account->getAccountNumber() << ": ";
                std::cin >> pin_attempt;

                if (next_account->authenticate(pin_attempt)) {
                    cur = next_account;
                    cur->display();
                } else {
                    std::cout << "Authentication failed. Staying on current account.\n";
                }
                break;
            }
            case 8:
                if (cur->isAccountFrozen()) cur->unfreeze();
                else cur->freeze();
                break;
            case 9:
                cur->applyInterest(); // Added manual trigger for interest
                break;
            default: ;
        }
    }
    std::cout << "\nGoodbye!\n";
}
