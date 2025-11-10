#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

using namespace std;


class stud_group{
    private:
        string uch;
        int kr;
    public:

        stud_group(string u, int k):
        uch(u), kr(k) {}

        void outputTo(){
            cout<<"учебное заведение"<<uch<<endl;
            cout<<"курс"<<kr<<endl;
        }

        string get_uch() const{
            return uch;
        }

        int get_kr() const{
            return kr;
        }

        void set_uch(string ucheb){
            uch=ucheb;
        }

        void set_kr(int kur){
            kr=kur;
        }
            
};

int main(){

    vector<stud_group> groups; 
    int n=0;
    cout<<"введите количество групп:";
    cin>>n;
    for (int i=0; i<n; i++){
        string u;
        int numb;
        cout<<"Введите название университета: ";
        cin>>u;
        cout<<"Введите номер группы:";
        cin>>numb;
        groups.push_back(stud_group(u, numb));
    }

    for(int i=0; i<groups.size();i++){
        groups[i].outputTo();
    }

    vector<stud_group> news;
    for (int j=0; j<n; j++){
        if (groups[j].get_uch() == "Sibsutis")
        news.push_back(groups[j]);

    }

  if (!news.empty()) {
    sort(news.begin(), news.end(), [](const stud_group& a, const stud_group& b) {
        return a.get_kr() < b.get_kr();
    });
}

int n2;
cout<<"Введите сколько вы хотите добавить групп";
cin>>n2;
for (int i=0; i<n2; i++){
    string u;
    int numb;
    cout<<"Введите название университета: ";
    cin>>u;
    cout<<"Введите номер группы:";
    cin>>numb;

    stud_group new_group(u, numb);

    auto it = std::lower_bound(news.begin(), news.end(), new_group, [](const stud_group& a, const stud_group& b) {
    return a.get_kr() < b.get_kr();  
});

    news.insert(it, new_group);}

    for(int i=0; i<news.size();i++){
        news[i].outputTo();
    }

    stud_group X = stud_group("Sibsutis", 433);
    stud_group *IKS= new stud_group("sIbsutis", 432);

    // X.outputTo();
    // IKS->outputTo();

    // X.set_kr(533);
    // IKS->set_kr(532);

    // cout<<"\n";

    // X.set_uch("NSU");
    // IKS->set_uch("NSU");

    // X.outputTo();
    // IKS->outputTo();


}
