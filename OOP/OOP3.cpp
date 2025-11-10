#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define N 7 

class Plankton {
public:
    string name;
    string position;
    int salary;
    float nal;

    void writeBinary(ofstream& os) const {

        int len;

        len = name.size();
        os.write((char*)&len, sizeof(len));
        os.write(name.c_str(), len);

        len = position.size();
        os.write((char*)&len, sizeof(len));
        os.write(position.c_str(), len);

        os.write((char*)&salary, sizeof(salary));

        os.write((char*)&nal, sizeof(nal));
    }


};

int main() {
    Plankton arr[N];

    ifstream fin("imen.txt");
    if (!fin.is_open()) {
        cout << "Error opening imen.txt!" << endl;
        return 1;
    }


    for (int i = 0; i < N; i++) {
        string a, b, c;
        if (!getline(fin, a)) break;
        if (!getline(fin, b)) break;
        if (!getline(fin, c)) break;

        if (!a.empty() && a.back() == '\r') a.pop_back();
        if (!b.empty() && b.back() == '\r') b.pop_back();
        if (!c.empty() && c.back() == '\r') c.pop_back();

        arr[i].name = a;
        arr[i].position = b;
        arr[i].salary = stoi(c);
        arr[i].nal = arr[i].salary*0.13;
    }
    fin.close();

    ofstream fout("imen.bin", ios::binary);
    if (!fout.is_open()) {
        cout << "Error creating imen.bin!" << endl;
        return 1;
    }

    for (int i = 0; i < N; i++){
        fout.write(reinterpret_cast<char*>(&arr[i]), sizeof(Plankton));
    }
    fout.close();

    ifstream bin("imen.bin", ios::binary);
    if (!bin.is_open()) {
        cout << "Error opening imen.bin!" << endl;
        return 1;
    }

    cout << "Из imen.bin:\n";
    for (int i = 0; i < N; i++) {
        cout << " " << endl; 
        cout << arr[i].name << " " << arr[i].position << " " << arr[i].salary<< " " <<arr[i].nal << endl;
    }

    bin.close();
    return 0;
}
