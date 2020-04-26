#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <queue>
#include <set>
#include <stack>

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

bool operator<(const Edge &a, const Edge &b) {
    if (a.c != b.c) {
        return a.c < b.c;
    }
    //return (long long int) a.v < (long long int) b.v;
    return a.v < b.v;
}

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

    void Nfa_to_without_empty_trans();

    void
    dfs_del_loop(int now, int &_index, stack<int> &stk, vector<int> &dfn, vector<int> &low, vector<int> &belong);

private:
    void and_merge(Nfa &a, char type);

    void or_merge(Nfa &a);

    void m_merge();

    vector<Fa_Node *> Node;
    vector<bool> used;
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

    n = n + a.n + 2;
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

    n = n + 2;
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
    this->renumber();
}

void Nfa::renumber() {
    //Fa_Node temp must be same
    queue<Fa_Node *> p;
    int cnt = 1;
    p.push(begin);
    bool usedflag = !begin->temp;
    Node.clear();
    used.clear();
    Node.resize(n + 1);
    used.resize(n + 1);
    while (!p.empty()) {
        Fa_Node *now = p.front();
        p.pop();
        if (now->temp == usedflag) {
            continue;
        }
        now->temp = usedflag;
        now->n = cnt;
        cnt++;
        Node[now->n] = now;
        for (auto i:now->edge) {
            p.push(i.v);
        }
    }
}

void Nfa::print() {
    //output NFA
    this->renumber();
    queue<Fa_Node *> p;
    p.push(begin);
    set<char> sc;
    map<char, set<int>> trans;

    bool usedflag = !begin->temp;

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
        if (now->temp == usedflag) {
            continue;
        }
        now->temp = usedflag;
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

void Nfa::Nfa_to_without_empty_trans() {//delete empty transfer,begin and end fail
    set<Edge> edge_tp;

    vector<int> dfn(n + 1);
    vector<int> low(n + 1);
    vector<int> belong(n + 1);
    for (int i = 0; i <= n; i++) {
        dfn[i] = 0;
        low[i] = 0;
        used[i] = false;
        belong[i] = i;
    }
    int _index = 1;
    //tarjan delete loop and expand start and end
    stack<int> stk;
    for (int i = 1; i <= n; i++) {
        if (!dfn[i]) {
            dfs_del_loop(i, _index, stk, dfn, low, belong);
        }
    }
    //rebuild edges
    for (int i = 1; i <= n; i++) {
        for (auto j:Node[i]->edge) {
            if (belong[i] == belong[j.v->n] && (j.c == '\0')) {
                continue;
            }
            Node[belong[i]]->edge.emplace_back(Node[belong[j.v->n]], j.c);
        }
    }
    //unique edges
    for (int i = 1; i <= n; i++) {
        if (belong[i] != i) {
            delete Node[i];
            continue;
        }
        edge_tp.clear();
        for (auto j:Node[i]->edge) {
            edge_tp.insert(j);
        }
        Node[i]->edge.clear();
        for (auto j:edge_tp) {
            Node[i]->edge.push_back(j);
        }
    }
    this->renumber();
    dfn.clear();
    low.clear();
    belong.clear();

    //reverse graph
    vector<int> in(n + 1);
    vector<int> to[n + 1];
    for (int i = 0; i <= n; i++) {
        in[i] = 0;
        used[i] = false;
    }
    for (int i = 1; i <= n; i++) {
        for (auto j:Node[i]->edge) {
            if (j.c == '\0') {
                to[j.v->n].push_back(i);
                in[j.v->n]++;
            }
        }
    }

    //topological sorting
    queue<int> q;
    int cnt = 1;
    while (cnt != 0) {
        cnt = 0;
        for (int i = 1; i <= n; i++) {
            if (used[i]) {
                continue;
            }
            if (in[i] == 0) {
                cnt++;
                used[i] = true;
                q.push(i);
                for (int j:to[i]) {
                    in[j]--;
                }
            }
        }
    }

    //expand empty transfer
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        edge_tp.clear();
        for (auto i:Node[u]->edge) {
            if (i.c != '\0') {
                continue;
            }
            for (auto j:i.v->edge) {
                edge_tp.insert(j);
            }
        }
        for (auto i:edge_tp) {
            Node[u]->edge.push_back(i);
        }
    }

    //unique edges and delete empty transfer
    for (int i = 1; i <= n; i++) {
        edge_tp.clear();
        for (auto j:Node[i]->edge) {
            if (j.c != '\0') {
                edge_tp.insert(j);
            }
        }
        Node[i]->edge.clear();
        for (auto j:edge_tp) {
            Node[i]->edge.push_back(j);
        }
    }

}

void
Nfa::dfs_del_loop(int now, int &_index, stack<int> &stk, vector<int> &dfn, vector<int> &low,
                  vector<int> &belong) {
    low[now] = dfn[now] = _index;
    _index++;
    stk.push(now);
    used[now] = true;
    for (auto i:Node[now]->edge) {
        if (i.c != '\0') {
            continue;
        }
        if (Node[now]->start) {
            Node[i.v->n]->start = true;
        }
        //start expand
        if (!dfn[i.v->n]) {
            dfs_del_loop(i.v->n, _index, stk, dfn, low, belong);
            low[now] = min(low[now], low[i.v->n]);
        } else if (used[i.v->n]) {
            low[now] = min(low[now], dfn[i.v->n]);
        }
        if (Node[i.v->n]->end) {
            Node[now]->end = true;
        }
        //end expand
    }
    if (dfn[now] == low[now]) {
        while (!stk.empty()) {
            int u = stk.top();
            stk.pop();
            used[u] = false;
            belong[u] = now;

            Node[now]->start = Node[now]->start || Node[u]->start;
            Node[now]->end = Node[now]->end || Node[u]->end;

            if (u == now) {
                break;
            }
        }
    }
}

int main() {
    //freopen("E:\\just ice\\Compiling principle\\finite-automaton\\out.csv", "w", stdout);
    string s;
    Nfa nfa;
    cin >> s;
    nfa.Re_to_Nfa(s);
    nfa.Nfa_to_without_empty_trans();
    nfa.print();
    return 0;
}
