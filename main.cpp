#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <queue>
#include <set>

using namespace std;


set<char> all_char;

class Fa_Node;

class Edge {
public:
    char c;
    Fa_Node *v;

    Edge(Fa_Node *v, char c = '\0') {
        this->c = c;
        this->v = v;
    }
};

class Fa_Node {
public:
    int n;
    bool start;
    bool end;
    vector<Edge> edge;
    bool temp;

    Fa_Node(int n, bool s = false, bool e = false) {
        this->n = n;
        start = s;
        end = e;
        temp = false;
    }
};

class Nfa {
public:
    Fa_Node *begin, *end;
    int n;


    Nfa() {
        begin = end = new Fa_Node(1, true, true);
        n = 1;
    }

    Nfa(const Nfa &a) {
        begin = a.begin;
        end = a.end;
        n = a.n;
    }

    ~Nfa() {
    }

    void Re_to_Nfa(string &str);

    void renumber();

    void print();

private:
    void and_merge(Nfa &a, char type);

    void or_merge(Nfa &a);

    void m_merge();
};

void Nfa::and_merge(Nfa &a, char type = '\0') {
    end->end = false;
    end->edge.emplace_back(a.begin, type);
    end = a.end;
    a.begin->start = false;
    n += a.n;
}

void Nfa::or_merge(Nfa &a) {
    begin->start = false;
    end->end = false;
    a.begin->start = false;
    a.end->end = false;
    Fa_Node *ts, *te;
    ts = new Fa_Node(1, true, false);
    te = new Fa_Node(1, false, true);

    ts->edge.emplace_back(begin);
    ts->edge.emplace_back(a.begin);
    end->edge.emplace_back(te);
    a.end->edge.emplace_back(te);

    begin = ts;
    end = te;
}

void Nfa::m_merge() {
    begin->start = false;
    end->end = false;

    Fa_Node *ts, *te;
    ts = new Fa_Node(1, true, false);
    te = new Fa_Node(1, false, true);

    ts->edge.emplace_back(begin);
    ts->edge.emplace_back(te);
    end->edge.emplace_back(te);
    end->edge.emplace_back(begin);

    begin = ts;
    end = te;
}

bool check_re(char c) {
    return c == '*' || c == '.' || c == '|' || c == '(' || c == ')' || c == '#';
}

void Nfa::Re_to_Nfa(string &str) {
    map<char, map<char, bool>> mp;//set operator priority
    {
        mp['.']['.'] = true;
        mp['.']['|'] = true;
        mp['.']['*'] = false;
        mp['.']['('] = false;
        mp['.'][')'] = true;
        mp['.']['#'] = true;

        mp['|']['.'] = false;
        mp['|']['|'] = true;
        mp['|']['*'] = false;
        mp['|']['('] = false;
        mp['|'][')'] = true;
        mp['|']['#'] = true;

        mp['*']['.'] = true;
        mp['*']['|'] = true;
        mp['*']['*'] = true;
        mp['*']['('] = true;
        mp['*'][')'] = true;
        mp['*']['#'] = true;

        mp['(']['.'] = false;
        mp['(']['|'] = false;
        mp['(']['*'] = false;
        mp['(']['('] = false;
        mp['('][')'] = true;
        mp['(']['#'] = true;

        mp[')']['.'] = true;
        mp[')']['|'] = true;
        mp[')']['*'] = true;
        mp[')']['('] = true;
        mp[')'][')'] = true;
        mp[')']['#'] = true;

        mp['#']['.'] = false;
        mp['#']['|'] = false;
        mp['#']['*'] = false;
        mp['#']['('] = false;
        mp['#'][')'] = false;
        mp['#']['#'] = true;
    }
    string s;
    for (int i = 0; i < str.size() - 1; i++) {
        s.push_back(str[i]);
        if (str[i] == '(' || str[i] == '|') {
            continue;
        }
        if ((str[i + 1] == '(') || (!check_re(str[i + 1]))) {
            s.push_back('.');
        }
    }
    s.push_back(str[str.size() - 1]);
    s.push_back('#');
    //insert and operation
    stack<Nfa> Nfa_s;
    stack<char> char_s;
    int i = 0;
    char_s.push('#');
    while (!char_s.empty() && i < s.size()) {
        if (check_re(s[i])) {//if it is operator
            char type = char_s.top();
            while (mp[type][s[i]]) {//until last operator's operation priority is less than it's
                char_s.pop();
                if (type == '(' || type == '#') {// ( or # don't push
                    break;
                }
                if (type == '.') {
                    Nfa b = Nfa_s.top();
                    Nfa_s.pop();
                    Nfa a = Nfa_s.top();
                    Nfa_s.pop();

                    a.and_merge(b);
                    Nfa_s.push(a);
                } else if (type == '|') {
                    Nfa b = Nfa_s.top();
                    Nfa_s.pop();
                    Nfa a = Nfa_s.top();
                    Nfa_s.pop();

                    a.or_merge(b);
                    Nfa_s.push(a);
                } else if (type == '*') {
                    Nfa a = Nfa_s.top();
                    Nfa_s.pop();

                    a.m_merge();
                    Nfa_s.push(a);
                }
                type = char_s.top();
            }
            if (!(s[i] == ')' || s[i] == '#')) {
                char_s.push(s[i]);
            }
            i++;
        } else {
            Nfa tp1, tp2;
            tp1.and_merge(tp2, s[i]);
            Nfa_s.push(tp1);
            all_char.insert(s[i]);
            i++;
        }
    }
    *this = Nfa_s.top();
    Nfa_s.pop();

    all_char.insert('\0');
}

void Nfa::renumber() {
    //Fa_Node temp must be false
    queue<Fa_Node *> p;
    int cnt = 1;
    p.push(begin);

    while (!p.empty()) {
        Fa_Node *now = p.front();
        p.pop();
        if (now->temp) {
            continue;
        }
        now->temp = true;
        now->n = cnt;
        cnt++;
        for (auto i:now->edge) {
            p.push(i.v);
        }
    }
    n = cnt - 1;
}

void Nfa::print() {
    //out Fa_Node temp must be true
    this->renumber();
    queue<Fa_Node *> p;
    p.push(begin);
    set<char> sc;
    map<char, set<int>> trans;

    cout << "S,E,N,";
    for (auto i:all_char) {
        if (i != '\0') {
            cout << i << ",";
        } else {
            cout << "\\0,";
        }
    }
    cout << endl;

    while (!p.empty()) {
        Fa_Node *now = p.front();
        p.pop();
        if (!now->temp) {
            continue;
        }
        now->temp = false;
        for (auto i:now->edge) {
            p.push(i.v);
            trans[i.c].insert(i.v->n);
        }

        if (now->start) {
            cout << "+,";
        } else {
            cout << ",";
        }

        if (now->end) {
            cout << "-,";
        } else {
            cout << ",";
        }

        cout << now->n << ',';
        for (auto i:all_char) {
            if (trans.find(i) == trans.end()) {
                cout << ",";
            } else {
                for (int j:trans[i]) {
                    cout << j << ' ';
                }
                cout << ',';
            }
        }
        cout << endl;
        trans.clear();
    }
}

int main() {
    freopen("E:\\just ice\\Compiling principle\\finite-automaton\\out.csv", "w", stdout);
    string s;
    Nfa nfa;
    cin >> s;
    nfa.Re_to_Nfa(s);
    nfa.renumber();
    nfa.print();
    return 0;
}
