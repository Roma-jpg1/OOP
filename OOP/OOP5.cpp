#include <iostream>
#include <list>
#include <string>
using namespace std;

class Train {
    int number;
    string destination;
    string departure;
public:
    Train() {}
    Train(int n, string d, string t) : number(n), destination(d), departure(t) {}
    void print() {
        cout << "Номер: " << number << endl;
        cout << "Станция: " << destination << endl;
        cout << "Отправление: " << departure << endl << endl;
    }
    int getNumber() { return number; }
    string getDestination() { return destination; }
};

int main() {
    list<Train> trains;
    int n;
    cout << "Сколько поездов: ";
    cin >> n;
    for (int i = 0; i < n; i++) {
        int num;
        string sta;
        string time;
        cout << "Номер: ";
        cin >> num;
        cout << "Станция: ";
        cin>>sta;
        cout << "Отправление: ";
        cin>>time;
        trains.emplace_back(num, sta, time);
        cout<<"\n\n";
    }

    for (auto& t : trains) t.print();

    int num;
    cout << "Найти по номеру: ";
    cin >> num;
    for (auto& t : trains)
        if (t.getNumber() == num) t.print();

    string sta;
    cout << "Найти по станции: ";
    cin>>sta;
    for (auto& t : trains)
        if (t.getDestination() == sta) t.print();
}
