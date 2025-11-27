#include <iostream>

using namespace std;

void line() {
    cout << "-------------------------" << endl;
}

void message() {
    cout << "Enjoy yourself with C++!" << endl;
}

int main() {

    for (int i = 0; i < 10; ++i) {
        line();
        cout << i << " " << endl;
        message();
        line();
    }

    return 0;
}


