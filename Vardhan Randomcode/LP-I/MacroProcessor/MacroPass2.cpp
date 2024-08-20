#include <bits/stdc++.h>
using namespace std;

class MacroProcessor {
    fstream ic;
    fstream aptab;
    fstream mdt;
    fstream mnt;
    fstream kpdt;
    fstream pt;
    fstream expandedLOC;
    vector<vector<string>> MNT;
    vector<vector<string>> MDT;
    vector<pair<string,string>> KPDT;
    vector<pair<string,string>> APTAB;
    vector<string> PNTAB;
public:
    vector<string> NAMES;

    MacroProcessor() {
        ic.open("IntermediateCode.txt", ios::in);
        if (!ic.is_open()) {
            cout << "Error opening IntermediateCode.txt file" << endl;
        }

        aptab.open("APTAB.txt", ios::out);
        if (!aptab.is_open()) {
            cout << "Error opening APTAB.txt file" << endl;
        }

        mdt.open("MacroDefinitionTable.txt", ios::in);
        if (!mdt.is_open()) {
            cout << "Error opening MacroDefinitionTable.txt file" << endl;
        }

        mnt.open("MacroNameTable.txt", ios::in);
        if (!mnt.is_open()) {
            cout << "Error opening MacroNameTable.txt file" << endl;
        }

        kpdt.open("KeywordParameterDefaultTable.txt", ios::in);
        if (!kpdt.is_open()) {
            cout << "Error opening KeywordParameterDefaultTable.txt file" << endl;
        }

        expandedLOC.open("ExpandedLOC.txt", ios::out);
        if (!expandedLOC.is_open()) {
            cout << "Error opening ExpandedLOC.txt file" << endl;
        }
        pt.open("ParameterTable.txt",ios::in);
        if(!pt.is_open()){
            cout<<"Error opening ParameterTable.txt"<<endl;
        }
        
    }

    void readFiles(){
        vector<string> words;
        string line;
        while(getline(mdt,line)){
            words.clear();
            stringstream ss(line);
            string word;
           
            while (ss >> word) {
                words.push_back(word);
            }
            MDT.push_back(words);

        }
        while(getline(mnt,line)){
            words.clear();
            stringstream ss(line);
            string word;
           
            while (ss >> word) {
                words.push_back(word);
            }
            MNT.push_back(words);
            NAMES.push_back(words[0]);
        }
        while(getline(kpdt,line)){
            words.clear();
            stringstream ss(line);
            string word;
           
            while (ss >> word) {
                words.push_back(word);
            }
            KPDT.push_back({words[0],words[2]});
        }
        
        while(getline(pt,line)){
            words.clear();
            stringstream ss(line);
            string word;

            while(ss>>word){
                words.push_back(word);
            }
            PNTAB.push_back(words[0]);
        }
    }

    vector<string> findMNT(string name){
        for(int i=0;i<MNT.size();i++){
            if(MNT[i][0]==name){
                return MNT[i];
            }
        }
        return {"Invalid"};
    }

    string findParameter(string name,int ref) {
        int reference = -1;
        //int index = -1;

        // Find the macro name in PNTAB
        for (int i = 0; i < PNTAB.size(); i++) {
            if (PNTAB[i] == name) {
                reference=i;
                break;
            }
        }


        return PNTAB[reference+ref];
    }

    void passTwo() {
    vector<string> words;
    string line;

    if (ic.is_open()) {
        while (getline(ic, line)) {
            string word;
            words.clear();
            stringstream ss(line);

            while (ss >> word) {
                words.push_back(word);
            }

            if (words.empty()) {
                continue;
            }

            auto it = find(NAMES.begin(), NAMES.end(), words[0]);
            if (it == NAMES.end()) {  // Not a macro, just copy the line
                for (const auto& w : words) {
                    expandedLOC << w << " ";
                }
                expandedLOC << endl;
            } else {  // Macro found, expand it
                string name = words[0];
                vector<string> mntEntry = findMNT(name);
                if (mntEntry[0] == "Invalid") {
                    cout << "Invalid macro call" << endl;
                    continue;
                }

                int mdtp = stoi(mntEntry[3]) - 1;
                int kpdtp = stoi(mntEntry[4]) - 1;
                APTAB.clear();

                int numPos = stoi(mntEntry[1]);
                int numKey = stoi(mntEntry[2]);
                int actualKey = 0;

                // Fill APTAB with positional parameters
                for (int i = 1; i <= numPos; i++) {
                    APTAB.push_back({words[i], ""});
                    aptab << words[i] << endl;
                }

                // Fill APTAB with keyword parameters from the macro call
                for (int i = numPos + 1; i < words.size(); i++) {
                    if (words[i].at(0) == '&') {
                        APTAB.push_back({words[i], words[i + 2]});
                        actualKey++;
                        aptab << words[i] << "=" << words[i + 2] << endl;
                        i += 2;
                    }
                }

                // Handle default values for missing keyword parameters
                if (actualKey < numKey) {
                    for (int i = kpdtp; i < kpdtp + numKey; i++) {
                        if (actualKey == numKey) break;
                        bool found = false;
                        for (const auto& entry : APTAB) {
                            if (KPDT[i].first == entry.first) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            // Only add if default value is not "-------"
                            if (KPDT[i].second != "-------") {
                                APTAB.push_back({KPDT[i].first, KPDT[i].second});
                                aptab << KPDT[i].first << "=" << KPDT[i].second << endl;
                                actualKey++;
                            }
                        }
                    }
                }

                // Expand the macro definition
                while (MDT[mdtp][0] != "MEND") {
                    for (const auto& token : MDT[mdtp]) {
                        if (token.at(0) == '(' && token.at(1) == 'P') {
                            int ref = token.at(3) - '0'; // Convert char to int
                            if (ref <= numPos) {
                                expandedLOC << APTAB[ref - 1].first << " ";
                            } else {
                                string param = findParameter(name, ref);
                                for (const auto& entry : APTAB) {
                                    if (entry.first == param) {
                                        expandedLOC << entry.second << " ";
                                        break;
                                    }
                                }
                            }
                        } else {
                            expandedLOC << token << " ";
                        }
                    }
                    expandedLOC << endl;
                    mdtp++;
                }
            }
        }
    }
}


};

int main(){
    MacroProcessor p1;
    p1.readFiles();
    p1.passTwo();
    
    
    return 0;
}
