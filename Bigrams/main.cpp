#include <iostream>
#include "fstream"
#include "vector"
#include "map"

using namespace std;

vector<vector<string>> parse_text(string text){
    vector<vector<string>> parsed_text;
    vector<string> sentence;
    string word;
    for (int i = 0; i < text.length(); ++i) {
        if((text[i] >= 'A') && (text[i] <= 'Z')) text[i] = (char)(text[i] + 32);
        else if(text[i] == '\'' || text[i] == '\"' || text[i] == '\n') {text.erase(i, 1); i--;}
        else if((text[i] == '!') || (text[i] == '?') || (text[i] == ',') || (text[i] == '(') ||
                (text[i] == ')') || (text[i] == '[') || (text[i] == ']') || (text[i] == ';') || (text[i] == ':')) text[i] = '.';
        if((text[i] == '.') && (text[i-1] == '.')) {text.erase(i, 1); i--;}
        if((text[i] == ' ') && (text[i-1] == '.')) {text.erase(i, 1); i--;}
        if((text[i] == '.') && (text[i-1] == ' ')) {text.erase(i, 1); i--;}
    }
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] == '.'){
            sentence.push_back(word);
            parsed_text.push_back(sentence);
            word.clear();
            sentence.clear();
        } else if (text[i] == ' '){
            sentence.push_back(word);
            word.clear();
        } else{
            word += text[i];
        }
    }
    return parsed_text;
}

map<string, map<string, int>> frequencies(const vector<vector<string>>& parsed){
    map<string, map<string, int>> dict;
    int m, n = parsed.size();
    for (int i = 0; i < n; ++i) {
        m = parsed[i].size() - 1;
        for (int j = 0; j < m; ++j) {
            if (!dict.contains(parsed[i][j])){
                dict[parsed[i][j]][parsed[i][j+1]] = 1;
            } else{
                dict[parsed[i][j]][parsed[i][j+1]]++;
            }
        }
    }
    return dict;
}

map<string, string> bigrams(const map<string, map<string, int>>& dict){
    map<string, string> result;
    for(auto word{dict.begin()}; word != dict.end(); word++){
        int max = -1;
        for(auto freq{word->second.begin()}; freq != word->second.end(); freq++){
            if (max < freq->second) {
                result[word->first] = freq->first;
                max = freq->second;
            };
        }
    }
    return result;
}

int main(){
    int n;
    string word;
    ifstream fin;
    string text, tmp;
    fin.open(R"(C:\things\CLion\Small Tasks\Bigrams\text.txt)");
    while(!fin.eof()){
        getline(fin, tmp);
        text += tmp;
    }
    vector<vector<string>> s = parse_text(text);
    map<string, map<string, int>> f = frequencies(s);
    map<string, string> b = bigrams(f);
    input:
    cout << "Start word:";
    cin >> word;
    if (!b.contains(word)){
        cout << "No such bigram" << endl;
        goto input;
    }
    cout << "How much words to add:";
    cin >> n;
    cout << word + " ";
    for (int i = 0; i < n; ++i) {
        cout << b[word] + " ";
        word = b[word];
        if(!b.contains(word)){
            cout << endl << "No more bigrams" << endl;
            goto input;
        }
    }
}