#include<bits/stdc++.h>
using namespace std;

#ifdef pikachu
#include "../misc/dbg.h"
#else
#define endl "\n"
#define debug(...);
#endif

#define all(v) v.begin(),v.end()

using ll = int64_t;
const int INF = 1e9;

struct Edge {
    int v, cap, rev;
};

class FordFulkerson {
    int n;
    vector<vector<Edge>> g;
    vector<bool> visited;

    bool dfs(int u, int t, int &flow) {
        if (u == t) return true;
        visited[u] = true;
        for (auto &e : g[u]) {
            if (!visited[e.v] && e.cap > 0) {
                int pushed = min(flow, e.cap);
                if (dfs(e.v, t, pushed)) {
                    e.cap -= pushed;
                    g[e.v][e.rev].cap += pushed;
                    flow = pushed;
                    return true;
                }
            }
        }
        return false;
    }

public:
    FordFulkerson(int n) {
        this->n = n;
        g.assign(n, {});
    }

    void addEdge(int u, int v, int cap) {
        g[u].push_back({v, cap, (int)g[v].size()});
        g[v].push_back({u, 0, (int)g[u].size() - 1});
    }

    int maxFlow(int s, int t) {
        int flow = 0;
        while (true) {
            visited.assign(n, false);
            int pushed = INF;
            if (!dfs(s, t, pushed)) break;
            flow += pushed;
        }
        return flow;
    }
};

struct NodeID {
    map<pair<int, int>, int> mp;
    int idx = 0;

    int getID(int station, int time) {
        pair<int, int> p = {station, time};
        if (mp.find(p) == mp.end()) {
            mp[p] = idx++;
        }
        return mp[p];
    }
};

vector<vector<string>> readCSV(string filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line, word;
    while (getline(file, line)) {
        vector<string> row;
        stringstream s(line);
        while (getline(s, word, ',')) row.push_back(word);
        data.push_back(row);
    }
    file.close();
    return data;
}
int convertToMinutes(const std::string& dayTime) {
    std::unordered_map<std::string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };
    std::istringstream ss(dayTime);
    std::string day, time;
    ss >> day >> time;
    // cout<<dayTime<<endl;
    int hours, minutes;
    if (time.length() == 5) {
        hours = std::stoi(time.substr(0, 2));
        minutes = std::stoi(time.substr(3, 2));
    } else {
        hours = std::stoi(time.substr(0, 1));
        minutes = std::stoi(time.substr(2, 2));
    }
    return dayToMinutes[day] + hours * 60 + minutes;
}
signed main() {
    ios_base::sync_with_stdio(false), cin.tie(NULL); cout.tie(NULL);

    // Redirect input from file
    freopen("train_edges.txt", "r", stdin);
    
    map<pair<int,int>, vector<vector<int>>> adj;
    map<string, int> mp = {
        {"TVC", 1}, {"KCVL", 2}, {"NCJ", 3}, {"CAPE", 4}, {"ERS", 5},
        {"QLN", 6}, {"ERN", 7}, {"MDU", 8}, {"TEN", 9}, {"SRR", 10},
        {"PGT", 11}, {"CLT", 12}, {"ED", 13}, {"TCR", 14}, {"ALLP", 15},
        {"GUV", 16}, {"KTYM", 17}, {"KYJ", 18}, {"PUU", 19}
    };
    
    string line;
    int m = 0;
    int n = 19;
    
    // Read header line and ignore it
    getline(cin, line);
    
    const int BREAK_LIMIT = 5000;
    while (getline(cin, line)) {
        m++;
        if (m >= BREAK_LIMIT) break;
        if (line.empty()) continue;
    
        istringstream ss(line);
        string train, us, ds, vs, as, ws;
        getline(ss, train, ',');
        getline(ss, us, ',');
        getline(ss, ds, ',');
        getline(ss, vs, ',');
        getline(ss, as, ',');
        getline(ss, ws, ',');
    
        int u = mp[us];
        int v = mp[vs];
    
        int st = convertToMinutes(ds);
        int en = convertToMinutes(as);
        if (en < st) en += 10080;
        
        adj[{u, st}].push_back({v, en, 1});
        // cout<<m<<endl;
    }
    // cout<<m<<endl;
    for (auto it : adj) {
        adj[{0, 0}].push_back({it.first.first, it.first.second, 1});
        adj[{n + 1, INF}].push_back({it.second[0][0], it.second[0][1], 1});
    }

    for (auto it : adj) {
        vector<int> p = it.second[0];
        for (auto i : adj) {
            vector<int> u = i.second[0];
            int rest_time = p[1] + (p[1] - it.first.second);  // Rest = Travel Time
            if (rest_time <= u[1]) {
                adj[{p[0], p[1]}].push_back({u[0], u[1], 1});
            }
        }
    }

    int l = 1, r = m, ans = m;
    while (l <= r) {
        int mid = (l + r) / 2;
        NodeID nodeID;
        // cout<<mid<<endl;
        for (auto &pr : adj) {
            nodeID.getID(pr.first.first, pr.first.second);
            for (auto &vec : pr.second) nodeID.getID(vec[0], vec[1]);
        }

        int source = nodeID.getID(0, 0);
        int sink = nodeID.getID(n + 1, INF);
        int superSource = nodeID.idx++;
        int totalNodes = nodeID.idx;

        FordFulkerson ff(totalNodes);
        for (auto &pr : adj) {
            int u = nodeID.getID(pr.first.first, pr.first.second);
            for (auto &vec : pr.second) {
                int v = nodeID.getID(vec[0], vec[1]);
                ff.addEdge(u, v, 1);
            }
        }
        ff.addEdge(superSource, source, mid);

        unordered_map<int, int> scheduleSupply;
        for (auto &pr : adj) {
            int u = nodeID.getID(pr.first.first, pr.first.second);
            for (auto &vec : pr.second) {
                if (vec[2] == 1) {
                    scheduleSupply[u]++;
                }
            }
        }

        for (auto &p : scheduleSupply) {
            ff.addEdge(superSource, p.first, p.second);
        }

        int flow = ff.maxFlow(superSource, sink);

        if (flow == m) {
            ans = mid;
            r = mid - 1;
        } else {
            l = mid + 1;
        }
    }
    cout << ans << endl;
}
