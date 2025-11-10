#include <iostream>
#include <fstream>
#include<vector>
#include<algorithm>
using namespace std;

class Aeroport {
protected:
    string name;
    string residence;
    int count_flights;
    string status;

public:
    Aeroport() {
        name = "";
        residence = "";
        count_flights = 0;
        status = "";
    }

    Aeroport(string n, string r, int cf, string s) {
        name = n;
        residence = r;
        count_flights = cf;
        status = s;
    }

    void setName(string n) { name = n; }
    void setResidence(string r) { residence = r; }
    void setCountFlights(int cf) { count_flights = cf; }
    void setStatus(string s) { status = s; }

    string getName() { return name; }
    string getResidence() { return residence; }
    int getCountFlights() { return count_flights; }
    string getStatus() { return status; }

    virtual void printInfo() {
        cout << "Название: " << name << endl;
        cout << "Место прописки: " << residence << endl;
        cout << "кол-во рейсов: " << count_flights << endl;
        cout << "Статус: " << status << endl;
    }
};

class Flight : public Aeroport {
    private:
        int number_flight;
        int capacity;
    
    public:
        Flight() : Aeroport() {
            number_flight = 0;
            capacity = 0;
        }

        Flight(string n, string r, int cf, string s, int nf, int c) : Aeroport(n, r, cf, s) , 
            number_flight(nf), 
            capacity(c){}
        

        void setNumberFlight(int nf) { number_flight = nf; }
        void setCapacity(int c) { capacity = c; }

        int getNumberFlight() { return number_flight; }
        int getCapacity() { return capacity; }

        void printInfo() override {
            Aeroport::printInfo();
            cout << "Номер рейса: " << number_flight << endl;
            cout << "Вместимость: " << capacity << endl;
        }
};

int main() {
    ifstream fin("aport.txt");

    if (!fin.is_open()) {
        cerr << "Ошибка открытия файлов!" << endl;
        return 1;
    }

    int n;
    cout << "Введите кол-во объектов класса: ";
    if (!(cin >> n) || n < 0) return 0;

    string name;
    string residence;
    int count_flights;
    string status;
    vector<Aeroport> aeorports;

    for (int i = 0; i < n; i++) {
        if (fin) {
        fin >> name >> residence >> count_flights >> status;
        aeorports.push_back(Aeroport(name, residence, count_flights, status));
        }
    }

    int resi=0;
 

    for (int i = 0; i < n; i++) {
        if (aeorports[i].getResidence() == "Новосибирск") 
            resi++;
            aeorports[i].printInfo();
    }
    cout << "Кол-во из новосибирска: " << resi << endl;
        
    

    fin.close();

    return 0;
}
