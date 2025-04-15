#include <bits/stdc++.h>
using namespace std;

#define INF 1e9

// A set to keep track of indices of "original" edges
set<int> originalEdges;

// Definition of an edge in the flow network
struct Edge {
    int v;      // Target node 
    int rev;    // Index of the reverse edge 
    int cap;    // Capacity 
    int flow;   // Current flow 
};

int superSource;    // A special additional source node used in the binary search checking
int sink;           // The sink (destination) node in the flow network
int source;         // The starting node in the flow network

// Mapping from a pair of node IDs (from, to) to an edge index for tracking original edges
map<pair<int, int>, int> edgeMap;

//Convert given dayTime string to minutes
int convertToMinutes(const std::string& dayTime) {
    // Each day is converted into minutes from Monday using this mapping.
    std::unordered_map<std::string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };
    std::istringstream ss(dayTime);
    std::string day, time;
    ss >> day >> time;
    int hours, minutes;
    // Handle time string in either "HH:MM" or "H:MM" format.
    if (time.length() == 5) {
        hours = std::stoi(time.substr(0, 2));
        minutes = std::stoi(time.substr(3, 2));
    } else {
        hours = std::stoi(time.substr(0, 1));
        minutes = std::stoi(time.substr(2, 2));
    }
    // Return total minutes elapsed from Monday 00:00.
    return dayToMinutes[day] + hours * 60 + minutes;
}

class MaxFlow {
public:
    int n;              // Number of nodes in the flow network
    vector<vector<Edge>> g;   // Adjacency list to represent the graph

    // Constructor: Initialize flow network with n nodes.
    MaxFlow(int n) : n(n), g(n) {}

    // Function: addEdge
    //Adds a directed edge from u to v with the given capacity and also adds the corresponding reverse edge with zero capacity.
    void addEdge(int u, int v, int cap) {
        // Edge from u to v
        Edge a{v, (int)g[v].size(), cap, 0};
        // Reverse edge from v to u (initially with zero capacity)
        Edge b{u, (int)g[u].size(), 0, 0};
        g[u].push_back(a);
        g[v].push_back(b);
    }

    // Function: dfs
    // Purpose:  A depth-first search that finds an augmenting path from node u to target t,
    //           with current available flow f. It marks visited nodes in 'vis'.
    int dfs(int u, int t, int f, vector<bool>& vis) {
        if (u == t) return f;  // Reached sink: return current flow
        vis[u] = true;
        for (auto &e : g[u]) {
            // If the next node is not visited and there is available capacity on edge e
            if (!vis[e.v] && e.flow < e.cap) {
                int pushed = dfs(e.v, t, min(f, e.cap - e.flow), vis);
                if (pushed > 0) {
                    // Augment the path: Increase current flow and decrease reverse edge's flow.
                    e.flow += pushed;
                    g[e.v][e.rev].flow -= pushed;
                    return pushed;
                }
            }
        }
        return 0; // No augmenting path found from this branch.
    }
    // Computes the maximum flow from source s to sink t.
    int fordFulkerson(int s, int t) {
        int flow = 0;
        while (true) {
            vector<bool> vis(n, false);
            int pushed = dfs(s, t, INF, vis);
            if (pushed == 0) break;  // No more augmenting paths
            flow += pushed;
        }
        return flow;
    }
};

// Dynamically assigns a unique integer ID to each pair (station, time) used as a node in the graph.
struct NodeID {
    // Mapping from (station, time) pair to a unique id.
    map<pair<int, int>, int> id;
    // Counter for assigning new ids.
    int idx = 0;

    // Function: Returns an existing id for node (u, t) or assigns a new one.
    int getID(int u, int t) {
        if (id.find({u, t}) == id.end())
            id[{u, t}] = idx++;  // Assign new id if not present.
        return id[{u, t}];
    }
};

// 'adj' represents the time-dependent adjacency list for train connections.
// The key is a pair (station, start time) and the value is a vector of vectors,
// where each inner vector contains details: {destination station, arrival time, capacity (always 1)}.
map<pair<int, int>, vector<vector<int>>> adj;

int n, m; // Global variables: n for number of stations, m for number of edges (trains)


//Checks if, with a given parameter d (number of extra edges from super source), the chosen d can cover the required conditions in the max flow network.
bool check(int d, MaxFlow &mf, NodeID &nodeID) {
    // Clear previous edges from the superSource node and add a new edge to the actual source node with capacity d.
    mf.g[superSource].clear();
    mf.addEdge(superSource, source, d);

    // Reset flow on all edges to zero before computing max flow.
    for (int u = 0; u < mf.n; u++) {
        for (auto &e : mf.g[u]) {
            e.flow = 0;
        }
    }

    // Run the Ford-Fulkerson algorithm from superSource to sink.
    mf.fordFulkerson(superSource, sink);

    int count = 0;
    // Count the number of original edges (as identified by their indices in 'originalEdges')
    // that are not fully used (i.e. their flow is 0).
    for (auto it : adj) {
        int u = nodeID.getID(it.first.first, it.first.second);
        for (auto e : mf.g[u]) {
            // Check if this edge is one of the original edges.
            if (originalEdges.find(edgeMap[{u, e.v}]) != originalEdges.end()) {
                // If the original edge did not get any flow, count it.
                if (e.flow == 0) {
                    count++;
                }
            }
        }
    }
    // Output the count (for debugging or progress tracking).
    cout << count << endl;
    // Return true if count is less than or equal to 100, meeting the requirement.
    return count <= 100;
}

int main() {
    ios::sync_with_stdio(0); 
    cin.tie(0);
    
    // Open the input file that contains train edge data.
    // The file "train_edges.txt" should list train edges with station names and times.
    freopen("train_edges.txt", "r", stdin);

    // Mapping station names (codes) to unique integer IDs.
    map<string, int> mp = {
        {"TVC", 1}, {"KCVL", 2}, {"NCJ", 3}, {"CAPE", 4}, {"ERS", 5},
        {"QLN", 6}, {"ERN", 7}, {"MDU", 8}, {"TEN", 9}, {"SRR", 10},
        {"PGT", 11}, {"CLT", 12}, {"ED", 13}, {"TCR", 14}, {"ALLP", 15},
        {"GUV", 16}, {"KTYM", 17}, {"KYJ", 18}, {"PUU", 19}
    };

    // Set to record original connections (edges) between nodes represented as pairs of (station, time).
    // Each element is a pair: { {departure station, departure time}, {arrival station, arrival time} }.
    set<pair<pair<int,int>,pair<int,int>>> myst;

    string line;
    int m = 0;  // Count of edges/trains read from the input file
    n = 19;     // Total number of stations (as defined in the mapping)

    // Read through all the lines in the file.
    getline(cin, line);  // Read header line if present.
    while (getline(cin, line)) {
        m++;  // Increment train edge count.
        
        if(line.empty())
            continue;
        
        istringstream ss(line);
        // Read tokens separated by commas. The tokens represent:
        // train code, up station, departure time, destination station, arrival time, and a possible extra field.
        string train, us, ds, vs, as, ws;
        getline(ss, train, ',');
        getline(ss, us, ',');
        getline(ss, ds, ',');
        getline(ss, vs, ',');
        getline(ss, as, ',');
        getline(ss, ws, ',');
    
        // Map station names to integer IDs.
        int u = mp[us];
        int v = mp[vs];
    
        // Convert departure and arrival times from string format to minutes.
        int st = convertToMinutes(ds);
        int en = convertToMinutes(as);
        // If arrival time is before departure time, adjust by adding a full week (10080 minutes).
        if (en < st) en += 10080;
    
        // Each node is represented by a (station, time) pair.
        // Store the edge in the adjacency list with capacity 1.
        adj[{u, st}].push_back({v, en, 1});
        // Record this as an original edge from departure to arrival.
        myst.insert({{u, st}, {v, en}});
    }

    for (auto it : adj) {
        // Add an edge from artificial starting node {0,0} to each departure node.
        adj[{0, 0}].push_back({it.first.first, it.first.second, 1});
        // Add an edge from an artificial end node {n+1, INF} to the first edge in the list.
        adj[{n + 1, INF}].push_back({it.second[0][0], it.second[0][1], 1});
    }

//For each node, consider valid transfers to subsequent train edges
    // after observing required rest time (calculated using departure and arrival times).
    for (auto it : adj) {
        // Get the first available connection from the current node.
        vector<int> p = it.second[0];
        int u = p[0], start = p[1];
    
        // If no outgoing connections exist for this node, skip.
        if (adj.find({u, start}) == adj.end()) continue;
    
        // For every subsequent connection from this node.
        for (auto nxt : adj[{u, start}]) {
            int v = nxt[0], dep_time = nxt[1];
    
            // Skip if there are no connections from the arrival node.
            if (adj.find({v, dep_time}) == adj.end()) continue;
    
            // Calculate the minimum required rest time.
            int rest_time = dep_time + (dep_time - it.first.second);
    
            // Use lower_bound to find the first possible connection after the required rest time.
            auto it2 = lower_bound(adj[{v, dep_time}].begin(), adj[{v, dep_time}].end(),
                                   vector<int>{0, rest_time, 0},
                                   [](const vector<int>& a, const vector<int>& b) {
                                       return a[1] < b[1];
                                   });
    
            // Connect all subsequent valid transfers.
            while (it2 != adj[{v, dep_time}].end()) {
                adj[{u, start}].push_back({(*it2)[0], (*it2)[1], 1});
                ++it2;
            }
        }
    }

    // Create a NodeID instance to assign unique identifiers to the (station, time) nodes.
    NodeID nodeID;

    int edgeCount = 0;
    for (auto &pr : adj) {
        int u = nodeID.getID(pr.first.first, pr.first.second);
        for (auto &vec : pr.second) {
            // Check if the current edge is one of the original train edges (based on myst).
            if (myst.find({{pr.first.first, pr.first.second}, {vec[0], vec[1]}}) != myst.end()) {
                originalEdges.insert(edgeCount);
            }
            int v = nodeID.getID(vec[0], vec[1]);
            // Map the edge from u to v with its current edgeCount for future reference.
            edgeMap[{u, v}] = edgeCount++;
        }
    }
    
    // Set the source as the artificial starting node and sink as the artificial destination node.
    source = nodeID.getID(0, 0);
    sink = nodeID.getID(n + 1, INF);
    // Create a super source with a new unique id (used in binary search testing).
    superSource = nodeID.idx++;
    int totalNodes = nodeID.idx;  // Total number of nodes in the graph

    // Initialize the max flow object with the total number of nodes.
    MaxFlow mf(totalNodes);

    // Add all the edges from the adj list to the max flow network.
    for (auto &pr : adj) {
        int u = nodeID.getID(pr.first.first, pr.first.second);
        for (auto &vec : pr.second) {
            int v = nodeID.getID(vec[0], vec[1]);
            mf.addEdge(u, v, 1);  // All edges have capacity 1.
        }
    }
    int l = 1, r = m, ans = m;
    while (l <= r) {
        int mid = (l + r) / 2;
        // Use the check function with the current mid value.
        if (check(mid, mf, nodeID)) {
            ans = mid;
            r = mid - 1;  // Try to find a smaller valid d.
        } else {
            l = mid + 1;  // Increase d if condition is not met.
        }
    }
    // Output the smallest valid value of d.
    cout << ans << endl;

}
