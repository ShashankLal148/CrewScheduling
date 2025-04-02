#include <bits/stdc++.h>
using namespace std;

const int MAX_V = 5000;  // Maximum number of vertices
const int INF = 1e9;     // Infinite value
map<pair<int, int>, vector<vector<int>>> adj;
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
class FordFulkerson {
private:
    struct Edge {
        int to, capacity, flow;
        Edge* reverse;
        bool isUsed;  // New flag to track edge usage

        Edge(int t, int c) : to(t), capacity(c), flow(0), reverse(nullptr), isUsed(false) {}
    };

    vector<vector<Edge*>> graph;
    vector<bool> visited;
    int vertices;
    set<pair<int,int>> usedEdges;  // Track used edges

    int dfs(int source, int sink, int minFlow) {
        // Base case: reached sink
        if (source == sink) return minFlow;

        visited[source] = true;

        // Try to find augmenting path
        for (Edge* edge : graph[source]) {
            // Check if edge has remaining capacity and destination not visited
            if (!visited[edge->to] && edge->flow < edge->capacity) {
                int bottleneck = dfs(edge->to, sink, 
                    min(minFlow, edge->capacity - edge->flow));

                // If path found
                if (bottleneck > 0) {
                    // Mark edge as used
                    edge->isUsed = true;
                    usedEdges.insert({source, edge->to});

                    // Augment flow
                    edge->flow += bottleneck;
                    edge->reverse->flow -= bottleneck;
                    return bottleneck;
                }
            }
        }

        return 0;
    }

public:
    // Constructor
    FordFulkerson(int v) : vertices(v), graph(v), visited(v) {}

    // Add edge to graph
    void addEdge(int from, int to, int capacity) {
        Edge* forward = new Edge(to, capacity);
        Edge* backward = new Edge(from, 0);

        // Cross-link edges
        forward->reverse = backward;
        backward->reverse = forward;

        graph[from].push_back(forward);
        graph[to].push_back(backward);
    }

    // Calculate max flow
    int computeMaxFlow(int source, int sink) {
        int maxFlow = 0;
        usedEdges.clear();  // Clear previous used edges

        // Repeatedly find augmenting paths
        while (true) {
            // Reset visited array
            fill(visited.begin(), visited.end(), false);

            // Find augmenting path
            int flowAddition = dfs(source, sink, INF);

            // If no path found, terminate
            if (flowAddition == 0) break;

            // Add flow found
            maxFlow += flowAddition;
        }

        return maxFlow;
    }

    // Print used edges
    void printUsedEdges() {
        cout << "Edges Used in Maximum Flow:" << endl;
        for (auto& edge : usedEdges) {
            cout << "Edge: " << edge.first << " -> " << edge.second << endl;
        }
    }

    // Get used edges
    set<pair<int,int>> getUsedEdges() {
        return usedEdges;
    }

    // Optional: Get final flow of a specific edge
    int getEdgeFlow(int from, int to) {
        for (Edge* edge : graph[from]) {
            if (edge->to == to) {
                return edge->flow;
            }
        }
        return -1;  // Edge not found
    }

    // Clean up dynamically allocated memory
    ~FordFulkerson() {
        for (auto& edges : graph) {
            for (auto edge : edges) {
                delete edge;
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    freopen("train_edges.txt","r",stdin);
    map<string, int> mp = {
        {"TVC", 1}, {"KCVL", 2}, {"NCJ", 3}, {"CAPE", 4}, {"ERS", 5},
        {"QLN", 6}, {"ERN", 7}, {"MDU", 8}, {"TEN", 9}, {"SRR", 10},
        {"PGT", 11}, {"CLT", 12}, {"ED", 13}, {"TCR", 14}, {"ALLP", 15},
        {"GUV", 16}, {"KTYM", 17}, {"KYJ", 18}, {"PUU", 19}
    };
    set<pair<pair<int,int>,pair<int,int>>> myst;
    string line;
    int m=0;int n=19;
    getline(cin,line);
    while(getline(cin,line))
    {
        m++;
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
            myst.insert({{u,st},{v,en}});
    }
    for (auto it : adj) {
        adj[{0, 0}].push_back({it.first.first, it.first.second, 1});
        adj[{n + 1, INF}].push_back({it.second[0][0], it.second[0][1], 1});
    }
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
    NodeID nodeID;
    
    int edgeCount = 0;
    
    for (auto &pr : adj) {
        int u = nodeID.getID(pr.first.first, pr.first.second);
        for (auto &vec : pr.second) {
            if(myst.find({{pr.first.first,pr.first.second},{vec[0],vec[1]}})!=myst.end())
            {
                originalEdges.insert(edgeCount);
            }
            // if((pr.first.first>=1 and pr.first.first<=19) and (vec[0]>=1 && vec[0]<=19))
            //     originalEdges.insert(edgeCount);
            int v = nodeID.getID(vec[0], vec[1]);
            edgeMap[{u, v}] = edgeCount++;
        }
    }
    // Example usage
    // FordFulkerson mf(6);  // Create graph with 6 vertices

    // // Add edges
    // mf.addEdge(0, 1, 16);
    // mf.addEdge(0, 2, 13);
    // mf.addEdge(1, 2, 10);
    // mf.addEdge(1, 3, 12);
    // mf.addEdge(2, 1, 4);
    // mf.addEdge(2, 4, 14);
    // mf.addEdge(3, 2, 9);
    // mf.addEdge(3, 5, 20);
    // mf.addEdge(4, 3, 7);
    // mf.addEdge(4, 5, 4);

    // // Compute max flow from source (0) to sink (5)
    // int maxFlow = mf.computeMaxFlow(0, 5);
    
    // // Print results
    // cout << "Maximum Flow: " << maxFlow << endl;
    
    // // Print used edges
    // mf.printUsedEdges();

    return 0;
}