#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <functional> // for std::hash

// --- Utility Functions ---

/**
 * Robustly reads a non-negative double value from standard input.
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
 * Robustly reads an integer within a specified range.
 */
int readInt(const std::string &prompt, const int lo, const int hi) {
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

    // Security & State
    const std::string hashedPin;
    std::time_t lastActivity;
    std::time_t lastWithdrawalReset;
    bool isFrozen = false;

    // Limits & Features
    double dailyWithdrawalLimit = 5000.0;
    double dailyWithdrawn = 0.0;
    double overdraftLimit = -50.0; // Allow slight overdraft
    double overdraftFee = 35.0; // Fee when going negative
    static constexpr double ANNUAL_INTEREST_RATE = 0.02; // 2%

    // Recurring deposits: {amount, next deposit time, interval in days}
    struct RecurringDeposit {
        double amount;
        std::time_t nextDeposit;
        int intervalDays;
    };

    std::vector<RecurringDeposit> recurringDeposits;

    // Helper: Format currency
    static std::string formatCurrency(const double amount) {
        std::ostringstream oss;
        oss << '$' << std::fixed << std::setprecision(2) << amount;
        return oss.str();
    }

    // Helper: Generate account number
    static std::string generateAccountNumber() {
        static int seq = 1000;
        return "ACC" + std::to_string(++seq);
    }

    // Helper: Hash PIN (basic—use proper KDF in production)
    static std::string hashPin(const std::string &pin) {
        const size_t hash = std::hash<std::string>{}(pin);
        return std::to_string(hash);
    }

    // Reset daily withdrawal limit if new day
    void resetDailyLimitsIfNeeded() {
        const std::time_t now = std::time(nullptr);
        std::tm now_tm = *std::localtime(&now);
        now_tm.tm_hour = 0;
        now_tm.tm_min = 0;
        now_tm.tm_sec = 0;
        const std::time_t todayStart = std::mktime(&now_tm);

        std::tm last_tm = *std::localtime(&lastWithdrawalReset);
        last_tm.tm_hour = 0;
        last_tm.tm_min = 0;
        last_tm.tm_sec = 0;

        if (const std::time_t lastResetDay = std::mktime(&last_tm); todayStart > lastResetDay) {
            dailyWithdrawn = 0.0;
            lastWithdrawalReset = now;
        }
    }

    // Log a transaction
    void log(const std::string &type, const double amount, const bool success, const std::string &note = "") {
        std::ostringstream oss;
        const std::time_t t = std::time(nullptr);
        oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
                << " | " << std::left << std::setw(15) << type
                << " | " << std::setw(12) << formatCurrency(amount)
                << " | " << (success ? "SUCCESS" : "FAILED ")
                << " | Bal: " << std::setw(12) << formatCurrency(balance);
        if (!note.empty()) oss << " | " << note;
        transactionHistory.push_back(oss.str());
        if (success) lastActivity = t;
    }

    // Validate amount
    [[nodiscard]] static bool isValidAmount(double amount) noexcept {
        return amount > 0.005 && amount <= 1e9;
    }

public:
    explicit BankAccount(std::string holder, const std::string &accountPin, const double opening = 0.0, std::string num = "")
        : balance(opening >= 0 ? opening : 0.0),
          accountHolder(std::move(holder)),
          accountNumber(num.empty() ? generateAccountNumber() : std::move(num)),
          hashedPin(hashPin(accountPin)),
          lastActivity(std::time(nullptr)),
          lastWithdrawalReset(std::time(nullptr)) {
        if (opening < 0) std::cerr << "Negative balance corrected to $0.00\n";
        log("OPEN", balance, true);
    }

    // Prevent copying
    BankAccount(const BankAccount &) = delete;

    BankAccount &operator=(const BankAccount &) = delete;

    [[nodiscard]] bool authenticate(const std::string &inputPin) const noexcept {
        return hashedPin == hashPin(inputPin);
    }

    // Apply interest based on time since last activity
    void applyInterest() {
        const std::time_t now = std::time(nullptr);
        const double timeDiff = std::difftime(now, lastActivity);

        if (const int monthsPassed = static_cast<int>(std::floor(timeDiff / (30.0 * 24.0 * 3600.0))); monthsPassed >= 1 && balance > 0) {
            constexpr double monthlyRate = ANNUAL_INTEREST_RATE / 12.0;
            double interestAmount = balance * monthlyRate * monthsPassed;
            interestAmount = std::round(interestAmount * 100.0) / 100.0;
            balance += interestAmount;
            log("INTEREST", interestAmount, true, "For " + std::to_string(monthsPassed) + " month(s)");
            lastActivity = now;
        }
    }

    // Add recurring deposit
    void addRecurringDeposit(const double amount, const int days = 30) {
        if (isValidAmount(amount)) {
            recurringDeposits.push_back({amount, std::time(nullptr) + days * 86400, days});
            log("RECURRING ADD", amount, true, "Every " + std::to_string(days) + " days");
        }
    }

    // Process recurring deposits
    void processRecurringDeposits() {
        const std::time_t now = std::time(nullptr);
        bool applied = false;
        for (auto &[amount, nextDeposit, intervalDays]: recurringDeposits) {
            if (now >= nextDeposit) {
                balance += amount;
                log("AUTO-DEPOSIT", amount, true, "Recurring");
                nextDeposit += intervalDays * 86400;
                applied = true;
            }
        }
        if (applied) lastActivity = now;
    }

    [[nodiscard]] bool canWithdraw(const double amount) const {
        if (isFrozen || !isValidAmount(amount)) return false;
        if (balance - amount < overdraftLimit) return false;
        return (dailyWithdrawn + amount <= dailyWithdrawalLimit);
    }

    [[nodiscard]] bool canReceive(const double amount) const {
        return !isFrozen && isValidAmount(amount);
    }

    bool deposit(const double amount) {
        if (isFrozen || !isValidAmount(amount)) {
            log("DEPOSIT", amount, false, isFrozen ? "Frozen" : "Invalid");
            return false;
        }
        balance += amount;
        log("DEPOSIT", amount, true);
        return true;
    }

    bool withdraw(const double amount) {
        resetDailyLimitsIfNeeded();
        if (!canWithdraw(amount)) {
            if (isFrozen) {
                log("WITHDRAW", amount, false, "Frozen");
            } else if (balance - amount < overdraftLimit) {
                log("WITHDRAW", amount, false, "Exceeds overdraft limit");
            } else {
                std::ostringstream oss;
                oss << "Limit " << formatCurrency(dailyWithdrawalLimit - dailyWithdrawn) << " remaining";
                log("WITHDRAW", amount, false, oss.str());
            }
            return false;
        }

        const double prevBalance = balance;
        balance -= amount;
        dailyWithdrawn += amount;

        // Apply overdraft fee if crossing into negative
        if (prevBalance >= 0 && balance < 0) {
            balance -= overdraftFee;
            log("OVERDRAFT FEE", overdraftFee, true);
        }

        log("WITHDRAW", amount, true);
        return true;
    }

    bool transfer(BankAccount &to, const double amount) {
        if (this == &to) return false;
        if (!canWithdraw(amount) || !to.canReceive(amount)) {
            return false;
        }

        // Perform atomic transfer
        balance -= amount;
        dailyWithdrawn += amount;
        to.balance += amount;

        log("TRANSFER OUT", amount, true, "To " + to.accountNumber);
        to.log("TRANSFER IN", amount, true, "From " + accountNumber);
        return true;
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

        applyInterest();
        processRecurringDeposits();

        std::cout << "\n╔══════════════════════════════════════╗\n"
                << "║            ACCOUNT SUMMARY           ║\n"
                << "╠══════════════════════════════════════╣\n"
                << "║ Holder   : " << std::setw(24) << std::left << accountHolder << " ║\n"
                << "║ Account  : " << accountNumber
                << std::string(std::max(0, 24 - static_cast<int>(accountNumber.size())), ' ') << " ║\n"
                << "║ Balance  : " << std::setw(24) << std::left << formatCurrency(balance)
                << (isFrozen ? "[FROZEN]" : "") << " ║\n"
                << "║ Daily W/D: " << std::setw(24) << std::left
                << formatCurrency(std::max(0.0, dailyWithdrawalLimit - dailyWithdrawn)) + " remaining" << " ║\n"
                << "║ Overdraft: " << std::setw(24) << std::left
                << (overdraftLimit < 0 ? formatCurrency(overdraftLimit) : "None") << " ║\n"
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
        const int start = n <= 0 ? 0 : std::max(0, static_cast<int>(transactionHistory.size()) - n);
        for (int i = start; i < static_cast<int>(transactionHistory.size()); ++i)
            std::cout << transactionHistory[i] << '\n';
    }

    // Getters
    [[nodiscard]] double getBalance() const noexcept { return balance; }
    [[nodiscard]] const std::string &getAccountNumber() const noexcept { return accountNumber; }
    [[nodiscard]] const std::string &getHolder() const noexcept { return accountHolder; }

    // Admin helper
    static void displayAllAccounts(const std::vector<BankAccount *> &accounts) {
        std::cout << "\n=== ADMIN VIEW: ALL ACCOUNTS ===\n";
        for (const auto *acc: accounts) {
            std::cout << "Account: " << acc->accountNumber
                    << " | Holder: " << acc->accountHolder
                    << " | Balance: " << formatCurrency(acc->balance)
                    << (acc->isFrozen ? " [FROZEN]" : "")
                    << '\n';
        }
    }
};

// --- Main Program ---

int main() {
    BankAccount a1("Alice Johnson", "1234", 1000.0);
    BankAccount a2("Bob Smith", "0000", 300.0);
    BankAccount a3("Charlie Brown", "9999", 5000.0);

    std::vector<BankAccount *> all_accounts = {&a1, &a2, &a3};
    BankAccount *cur = &a1;

    std::cout << "SecureBank Pro v5.0 (Enhanced)\n";

    // Initial authentication
    std::cout << "\n--- Welcome, Alice ---\n";
    std::string pin_attempt;
    std::cout << "Enter PIN for " << cur->getAccountNumber() << ": ";
    std::cin >> pin_attempt;

    if (!cur->authenticate(pin_attempt)) {
        std::cout << "Authentication failed. Exiting.\n";
        return 1;
    }

    cur->display();

    while (true) {
        std::cout << "\n1 Deposit  2 Withdraw  3 Transfer  4 Balance\n"
                << "5 History  6 Last 5    7 Switch    8 Freeze/Unfreeze\n"
                << "9 Interest 10 Recurring 11 Admin View 12 Exit\n> ";
        const int ch = readInt("", 1, 12);
        if (ch == 12) break;

        switch (ch) {
            case 1: {
                double amt = readDouble("Deposit $");
                cur->deposit(amt);
                break;
            }
            case 2: {
                double amt = readDouble("Withdraw $");
                cur->withdraw(amt);
                break;
            }
            case 3: {
                double amt = readDouble("Amount $");
                std::cout << "To: 1.Bob 2.Charlie\n";
                int t = readInt("", 1, 2);

                if (BankAccount * target = (t == 1 ? &a2 : &a3); target->isAccountFrozen()) {
                    std::cout << "Transfer failed: Target account is frozen.\n";
                } else if (!cur->transfer(*target, amt)) {
                    std::cout << "Transfer failed: Insufficient funds or invalid amount.\n";
                }

                break;
            }
            case 4:
                cur->display();
                break;
            case 5:
                cur->showHistory();
                break;
            case 6:
                cur->showHistory(5);
                break;
            case 7: {
                std::cout << "1 Alice 2 Bob 3 Charlie\n";
                int s = readInt("", 1, 3);
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
                if (cur->isAccountFrozen()) {
                    cur->unfreeze();
                    std::cout << "Account unfrozen.\n";
                } else {
                    cur->freeze();
                    std::cout << "Account frozen.\n";
                }
                break;
            case 9:
                cur->applyInterest();
                std::cout << "Interest applied (if eligible).\n";
                break;
            case 10: {
                double amt = readDouble("Auto-deposit amount $: ");
                int days = readInt("Interval (days, default 30): ", 1, 365);
                cur->addRecurringDeposit(amt, days);
                break;
            }
            case 11:
                BankAccount::displayAllAccounts(all_accounts);
                break;
            default:
                break;
        }
    }
    std::cout << "\nGoodbye!\n";
    return 0;
}
