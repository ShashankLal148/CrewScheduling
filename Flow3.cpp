#include<bits/stdc++.h>
using namespace std;
#define INF 1e9
set<int> originalEdges;
struct Edge {
    int v, rev, cap, flow;
};
int convertToMinutes(const std::string& dayTime) {
    std::unordered_map<std::string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };
    std::istringstream ss(dayTime);
    std::string day, time;
    ss >> day >> time;
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
class MaxFlow {
public:
    int n;
    vector<vector<Edge>> g;

    MaxFlow(int n) : n(n), g(n) {}

    void addEdge(int u, int v, int cap) {
        Edge a{v, (int)g[v].size(), cap, 0};
        Edge b{u, (int)g[u].size(), 0, 0};
        g[u].push_back(a);
        g[v].push_back(b);
    }

    int dfs(int u, int t, int f, vector<bool>& vis) {
        if (u == t) return f;
        vis[u] = true;
        for (auto &e : g[u]) {
            if (!vis[e.v] && e.flow < e.cap) {
                int pushed = dfs(e.v, t, min(f, e.cap - e.flow), vis);
                if (pushed > 0) {
                    e.flow += pushed;
                    g[e.v][e.rev].flow -= pushed;
                    return pushed;
                }
            }
        }
        return 0;
    }

    int fordFulkerson(int s, int t) {
        int flow = 0;
        while (true) {
            vector<bool> vis(n, false);
            int pushed = dfs(s, t, INF, vis);
            if (pushed == 0) break;
            flow += pushed;
        }
        return flow;
    }
};

struct NodeID {
    map<pair<int, int>, int> id;
    int idx = 0;

    int getID(int u, int t) {
        if (id.find({u, t}) == id.end())
            id[{u, t}] = idx++;
        return id[{u, t}];
    }
};

map<pair<int, int>, vector<vector<int>>> adj;
int n, m;

bool check(int d) {
    NodeID nodeID;
    map<pair<int, int>, int> edgeMap;
    int edgeCount = 0;
    // freopen("EdgeList.txt","w",stdout);
    for (auto &pr : adj) {
        int u = nodeID.getID(pr.first.first, pr.first.second);
        for (auto &vec : pr.second) {
            if((pr.first.first>=1 and pr.first.first<=19) and (vec[0]>=1 && vec[0]<=19))
                originalEdges.insert(edgeCount);
            int v = nodeID.getID(vec[0], vec[1]);
            edgeMap[{u, v}] = edgeCount++;
        }
    }

    int source = nodeID.getID(0, 0);
    int sink = nodeID.getID(n + 1, INF);
    int superSource = nodeID.idx++;
    int totalNodes = nodeID.idx;

    MaxFlow mf(totalNodes);
    for (auto &pr : adj) {
        int u = nodeID.getID(pr.first.first, pr.first.second);
        for (auto &vec : pr.second) {
            int v = nodeID.getID(vec[0], vec[1]);
            mf.addEdge(u, v, 1);
        }
    }
    mf.addEdge(superSource, source, d);

    int g=mf.fordFulkerson(superSource, sink);
    cout<<g<<endl;
    // cout<<d<<endl;
    cout<<originalEdges.size()<<endl;
    
    for(auto it:adj)
    {
        int u = nodeID.getID(it.first.first, it.first.second);
        // int v=nodeID.getID
        for(auto e:mf.g[u])
        {
            if(originalEdges.find(edgeMap[{u,e.v}])!=originalEdges.end())
            {
                // cout<<u<<" "<<e.v<<endl;
                if(e.flow==0)
                {
                    // cout<<u<<" "<<e.v<<endl;
                    return false;
                }
            }
        }
    }
    return true;
}

int main() {
    ios::sync_with_stdio(0); cin.tie(0);
    freopen("train_edges.txt", "r", stdin);
    // string line;
    map<string, int> mp = {
        {"TVC", 1}, {"KCVL", 2}, {"NCJ", 3}, {"CAPE", 4}, {"ERS", 5},
        {"QLN", 6}, {"ERN", 7}, {"MDU", 8}, {"TEN", 9}, {"SRR", 10},
        {"PGT", 11}, {"CLT", 12}, {"ED", 13}, {"TCR", 14}, {"ALLP", 15},
        {"GUV", 16}, {"KTYM", 17}, {"KYJ", 18}, {"PUU", 19}
    };
    // int stationIdx = 1;
    // m = 0;
  

    string line;
    int m=0;n=19;
    getline(cin,line);
    while (getline(cin, line)) {
        m++;
        // if(m>=5000)
        //     break;
        if(line.empty())
            continue;
        istringstream ss(line);
        string train,us,ds,vs,as,ws;
        getline(ss,train,',');
        getline(ss,us,',');
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
        // originalEdges.insert({u,st});
    }
    cout<<adj.size()<<endl;
    for (auto it : adj) {
        adj[{0, 0}].push_back({it.first.first, it.first.second, 1});
        adj[{n + 1, INF}].push_back({it.second[0][0], it.second[0][1], 1});
    }
    cout<<adj.size()<<endl;
    for (auto it : adj) {
        vector<int> p = it.second[0];
        int u = p[0], start = p[1];
    
        // Check outgoing edges from u starting at start
        if (adj.find({u, start}) == adj.end()) continue;
    
        for (auto nxt : adj[{u, start}]) {
            int v = nxt[0], dep_time = nxt[1];
    
            // Check if outgoing edges from v exist
            if (adj.find({v, dep_time}) == adj.end()) continue;
    
            int rest_time = dep_time + (dep_time - it.first.second);
    
            // Find the next valid edge using lower_bound
            auto it2 = lower_bound(adj[{v, dep_time}].begin(), adj[{v, dep_time}].end(),
                                   vector<int>{0, rest_time, 0},
                                   [](const vector<int>& a, const vector<int>& b) {
                                       return a[1] < b[1];
                                   });
    
            // Add valid connections after rest_time
            while (it2 != adj[{v, dep_time}].end()) {
                adj[{u, start}].push_back({(*it2)[0], (*it2)[1], 1});
                ++it2;
            }
        }
    }
    cout<<adj.size()<<endl;
    int l = 1, r = m, ans = -1;
    while (l <= r) {
        int mid = (l + r) / 2;
        // cout<<mid<<endl;
        if (check(mid)) {
            ans = mid;
            r = mid - 1;
        } else {
            l = mid + 1;
        }
    }
    cout << "Minimum d = " << ans << endl;
    return 0;
}
