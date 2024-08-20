#include <bits/stdc++.h>
using namespace std;

class MNTEntry {
    int PP;
    int KP;
    int MDTP;
    int KPDTP;

public:
    MNTEntry() {
        this->PP = 0;
        this->KP = 0;
        this->MDTP = 0;
        this->KPDTP = 0;
    }

    friend class MacroProcessor;
};

class MacroProcessor {
    vector<pair<string, MNTEntry>> MNT;
    vector<pair<string, string>> KPDTAB;
    vector<vector<string>> MDT;
    vector<string> PNTAB;
    fstream input;
    fstream mnt;
    fstream kpdt;
    fstream mdt;
    fstream pt;
    fstream ic;

public:
    MacroProcessor() {
        input.open("Input2.txt", ios::in);
        if (!input.is_open()) {
            cout << "Error opening input file" << endl;
        }
        mnt.open("MacroNameTable.txt", ios::out);
        if (!mnt.is_open()) {
            cout << "Error opening MNT file" << endl;
        }
        kpdt.open("KeywordParameterDefaultTable.txt", ios::out);
        if (!kpdt.is_open()) {
            cout << "Error opening KPDT file" << endl;
        }
        mdt.open("MacroDefinitionTable.txt", ios::out);
        if (!mdt.is_open()) {
            cout << "Error opening MDT file" << endl;
        }
        pt.open("ParameterTable.txt", ios::out);
        if (!pt.is_open()) {
            cout << "Error opening PT file" << endl;
        }
        ic.open("IntermediateCode.txt",ios::out);
        if(!ic.is_open()){
            cout<<"Error opening IC file"<<endl;
        }
    }

    int findParameter(string parameter, string name){
        int reference = -1;
        int index = -1;

        // Find the macro name in PNTAB
        for (int i = 0; i < PNTAB.size(); i++) {
            if (PNTAB[i] == name) {
                reference = i;
                break;
            }
        }

        if (reference != -1) {
            // Find the parameter relative to the macro name
            for (int i = reference + 1; i < PNTAB.size(); i++) {
                if (PNTAB[i] == parameter) {
                    index = i - reference;
                    break;
                }
            }
        }

        return index;
    }

    int count(string param,int start, int end){
        int cnt=0;
        for(int i=start;i<end;i++){
            if(PNTAB[i]==param){
                cnt++;
            }
        }
        return cnt;
    }

    void passOne() {
    vector<string> words;
    vector<string> temp;
    string line;
    string name;
    int kcounter = 0;
    int pcounter = 0;
    int mcounter = 0; // Counter for MDT entries
    bool insideMacro = false;
    bool firstLine = false;
    MNTEntry m;

    if (input.is_open()) {
        while (getline(input, line)) {
            words.clear();
            stringstream ss(line);
            string word;
           
            while (ss >> word) {
                words.push_back(word);
            }

            if (words.empty()) continue; // Skip empty lines

            if (words[0] == "MACRO") {
                insideMacro = true;
                firstLine = true;
                continue; // Move to next line
            }
            else if (words[0] == "MEND") {
                if (insideMacro) {
                    // Add MEND to MDT
                    MDT.push_back({"MEND"});
                    mcounter++;
                    insideMacro = false;
                }
                continue; // Move to next line
            }

            if (insideMacro) {
                if (firstLine) {
                    name = words[0];
                    PNTAB.push_back(name);
                    pcounter = 0;
                    kcounter = 0;

                    for (int i = 1; i < words.size(); i++) {
                        if (words[i].at(0) == '&') {
                            PNTAB.push_back(words[i]);
                            pcounter++;
                        }
                        if (words[i] == "=") {
                            if (i != words.size() - 1 && words[i + 1].at(0) != '&') {
                                KPDTAB.push_back({words[i - 1], words[i + 1]});
                                kcounter++;
                            }
                            else {
                                KPDTAB.push_back({words[i - 1], ""});
                                kcounter++;
                            }
                        }
                    }
                    


                    if (kcounter != 0) {
                        m.KPDTP = KPDTAB.size() - kcounter + 1;
                    }
                    m.KP = kcounter;
                    m.PP = pcounter - kcounter;
                    m.MDTP = mcounter + 1;
                    firstLine = false;

                    for(int i=0;i<MNT.size();i++){
                        if(name==MNT[i].first && m.PP==MNT[i].second.PP){
                            cout<<"Macro with same name and parameters exists ! May lead to ambiguous calls !"<<endl;
                        }
                    }
                    int ref=0;
                    for(int i=0;i<PNTAB.size();i++){
                        if(name==PNTAB[i]){
                            ref=i;
                            break;
                        }
                    }
                    for(int i=ref+1;i<ref+m.KP+m.PP+1;i++){
                        if(count(PNTAB[i],ref+1,ref+m.KP+m.PP+1)>1){
                            cout<<"Error: Parameters cannot be duplicate"<<endl;
                            break;
                        }
                    }

                    MNT.push_back({name, m});
                }
                else {
                    for (const auto& w : words) {
                        if (w.at(0) == '&') {
                            string param = "(P,";
                            param += to_string(findParameter(w, name));
                            param += ")";
                            temp.push_back(param);
                        }
                        else {
                            temp.push_back(w);
                        }
                    }
                    MDT.push_back(temp);
                    temp.clear();
                    mcounter++;
                }
            }
            else {
                // Process non-macro lines here
                for (const auto& w : words) {
                    ic << w << " ";
                }
                ic << endl;
            }
        }
    } else {
        cout << "Error opening input file" << endl;
    }
    input.close();
}



    void writeMNT() {
        for (auto &x : MNT) {
            mnt << x.first << " " << x.second.PP << " " << x.second.KP << " " << x.second.MDTP << " " << x.second.KPDTP << endl;
        }
        mnt.close();
    }

    void writeKPDTAB() {
        for (auto &x : KPDTAB) {
            if(x.second!=""){
                kpdt << x.first << " = " << x.second << endl;
            }
            else{
                kpdt<<x.first<<" = -------"<<endl;
            }
        }
        kpdt.close();
    }

    void writeMDT() {
        for (int i = 0; i < MDT.size(); i++) {
            for (int j = 0; j < MDT[i].size(); j++) {
                mdt << MDT[i][j] << " ";
            }
            mdt << endl;
        }
        mdt.close();
    }

    void writePNTAB() {
        for (auto &x : PNTAB) {
            pt << x << endl;
        }
        pt.close();
    }
};

int main() {
    MacroProcessor mp;
    mp.passOne();
    mp.writeMNT();
    mp.writeKPDTAB();
    mp.writeMDT();
    mp.writePNTAB();

    return 0;
}
