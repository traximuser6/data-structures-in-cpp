#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <ctime>
#include <algorithm>

class BankAccount {


private:
    double balance;
    std::string accountHolder;
    const std::string accountNumber;
    std::vector<std::string> transactionHistory;
    bool isFrozen = false;
    double dailyWithdrawalLimit = 5000.0;
    double dailyWithdrawn = 0.0;

    void resetDailyLimitsIfNeeded() {
        static std::time_t lastReset = 0;
        std::time_t now = std::time(nullptr);
        struct tm *lt = std::localtime(&now);
        lt->tm_hour = lt->tm_min = lt->tm_sec = 0;
        std::time_t midnight = std::mktime(lt);

        if (lastReset < midnight) {
            dailyWithdrawn = 0.0;
            lastReset = midnight;
        }
    }

    static bool isValidAmount(double amount) noexcept {
        return amount > 0.0 && amount <= 1e9;
    }

    void log(const std::string &type, double amount, bool success, const std::string &note = "") {
        std::ostringstream oss;
        std::time_t t = std::time(nullptr);
        oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
                << " | " << std::left << std::setw(12) << type
                << " | $" << std::fixed << std::setprecision(2) << std::setw(10) << amount
                << " | " << (success ? "SUCCESS " : "FAILED  ")
                << " | Bal: $" << std::setw(10) << balance;
        if (!note.empty()) oss << " | " << note;
        transactionHistory.push_back(oss.str());
    }

    static std::string generateAccountNumber() {
        static int seq = 1000;
        return "ACC" + std::to_string(++seq);
    }

public:
    explicit BankAccount(std::string holder, double opening = 0.0, std::string num = "")
        : balance(opening >= 0 ? opening : 0.0),
          accountHolder(std::move(holder)),
          accountNumber(num.empty() ? generateAccountNumber() : std::move(num)) {
        if (opening < 0) std::cerr << "Negative balance corrected to $0.00\n";
        log("OPEN", balance, true);
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
            log("WITHDRAW", amount, false, "Daily limit");
            return false;
        }
        if (amount > balance) {
            log("WITHDRAW", amount, false, "No funds");
            return false;
        }

        balance -= amount;
        dailyWithdrawn += amount;
        log("WITHDRAW", amount, true);
        return true;
    }

    bool transfer(BankAccount &to, double amount) {
        if (this == &to) return false;
        if (withdraw(amount)) {
            if (to.deposit(amount)) {
                log("TRANSFER OUT", amount, true, "To " + to.accountNumber);
                to.log("TRANSFER IN", amount, true, "From " + accountNumber);
                return true;
            }
            deposit(amount); // rollback
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

    void display() const {
        std::cout << "\n╔══════════════════════════════════════╗\n"
                << "║            ACCOUNT SUMMARY           ║\n"
                << "╠══════════════════════════════════════╣\n"
                << "║ Holder   : " << std::setw(24) << std::left << accountHolder << " ║\n"
                << "║ Account  : " << accountNumber
                << std::string(std::max(0, 24 - (int) accountNumber.size()), ' ') << " ║\n"
                << "║ Balance  : $" << std::setw(12) << std::fixed << std::setprecision(2) << balance
                << (isFrozen ? " [FROZEN] ║\n" : "         ║\n")
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

        int start = n <= 0 ? 0 : std::max(0, (int) transactionHistory.size() - n);
        for (int i = start; i < (int) transactionHistory.size(); ++i)
            std::cout << transactionHistory[i] << '\n';
    }

    // Getters
    [[nodiscard]] double getBalance() const noexcept { return balance; }
    [[nodiscard]] const std::string &getAccountNumber() const noexcept { return accountNumber; }
};

// Input helpers
double readDouble(const std::string &p) {
    double v;
    while (!(std::cin >> v) || v < 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid → try again: ";
    }
    return v;
}

int readInt(const std::string &p, int lo, const int hi) {
    int v;
    while (!(std::cin >> v) || v < lo || v > hi) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Enter " << lo << "-" << hi << ": ";
    }
    return v;
}

int main() {
    BankAccount a1("Alice Johnson", 1000.0);
    BankAccount a2("Bob Smith", 300.0);
    BankAccount a3("Charlie Brown", 5000.0);

    BankAccount *cur = &a1;

    std::cout << "SecureBank Pro v3.0\n";
    cur->display();

    while (true) {
        std::cout << "\n1 Deposit  2 Withdraw  3 Transfer  4 Balance\n"
                << "5 History  6 Last 5   7 Switch    8 Freeze/Unfreeze\n"
                << "9 Exit\n> ";
        int ch = readInt("", 1, 9);
        if (ch == 9) break;

        switch (ch) {
            case 1: cur->deposit(readDouble("Deposit $"));
                break;
            case 2: cur->withdraw(readDouble("Withdraw $"));
                break;
            case 3: {
                double amt = readDouble("Amount $");
                std::cout << "To: 1.Bob 2.Charlie\n";
                int t = readInt("", 1, 2);
                cur->transfer(t == 1 ? a2 : a3, amt);
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
                int s = readInt("", 1, 3);
                cur = (s == 1 ? &a1 : s == 2 ? &a2 : &a3);
                cur->display();
                break;
            }
            case 8:
                if (cur->isAccountFrozen()) cur->unfreeze();
                else cur->freeze();
                break;
            default: ;
        }
    }
    std::cout << "\nGoodbye!\n";
}
