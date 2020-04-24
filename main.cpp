#include <iostream>
#include <vector>
#include <string>

using namespace std;

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

    Fa_Node(int n, bool s = false, bool e = false) {
        this->n = n;
        start = s;
        end = e;
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

private:
    void and_merge(Nfa &a, char c);

    void or_merge(Nfa &a);

    void m_merge();
};

void Nfa::and_merge(Nfa &a, char c) {
    end->end = false;
    end->edge.emplace_back(a.begin, c);
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

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
