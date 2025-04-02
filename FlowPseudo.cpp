#include<bits/stdc++.h>
using namespace std;

#ifdef pikachu
#include "../misc/dbg.h"
#else
#define endl "\n"
#define debug(...);
#endif

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
using namespace __gnu_pbds;

template<class T> using ordered_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;


template<class T> using ordered_multiset = tree<T, null_type, less_equal<T>, rb_tree_tag, tree_order_statistics_node_update>;

#define all(v) v.begin(),v.end();

//#define int int64_t
using ll = int64_t;

const int MOD = 1e9 + 7, INF = 1e9;

struct Dinic {
    struct Edge {
        int to, rev, cap;
    };
    vector<vector<Edge>> g;
    vector<int> dist, it;
    
    Dinic(int n) {
        g.assign(n, {});
        dist.assign(n, -1);
        it.assign(n, 0);
    }
    
    void addEdge(int s, int t, int cap) {
        g[s].push_back({t, (int)g[t].size(), cap});
        g[t].push_back({s, (int)g[s].size()-1, 0});
    }
    
    bool bfs(int s, int t) {
        fill(dist.begin(), dist.end(), -1);
        dist[s] = 0;
        queue<int> q;
        q.push(s);
        while(!q.empty()){
            int u = q.front(); q.pop();
            for(auto &e: g[u]){
                if(dist[e.to] < 0 && e.cap > 0){
                    dist[e.to] = dist[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return dist[t] >= 0;
    }
    
    int dfs(int u, int t, int flow) {
        if(u == t)
            return flow;
        for(int &i = it[u]; i < (int)g[u].size(); i++){
            auto &e = g[u][i];
            if(e.cap > 0 && dist[e.to] == dist[u] + 1){
                int pushed = dfs(e.to, t, min(flow, e.cap));
                if(pushed){
                    e.cap -= pushed;
                    g[e.to][e.rev].cap += pushed;
                    return pushed;
                }
            }
        }
        return 0;
    }
    
    int maxFlow(int s, int t) {
        int flow = 0;
        while(bfs(s, t)){
            fill(it.begin(), it.end(), 0);
            while (int pushed = dfs(s, t, INF))
                flow += pushed;
        }
        return flow;
    }
};


struct NodeID {
    map<pair<int,int>, int> mp;
    int idx = 0;
    
    int getID(int station, int time) {
        pair<int,int> p = {station, time};
        if(mp.find(p) == mp.end()){
            mp[p] = idx++;
        }
        return mp[p];
    }
};





signed main()
{
    ios_base::sync_with_stdio(false), cin.tie(NULL);
    int tt;
    cin >> tt;
    while(tt--)
    {
        int n, x, m;
        cin >> n >> x >> m;
        //n is  the nnumber of the station...
        //x is the number of resting hours the person should get affter gettig otu of the station..

        // m are the number of schedules repectuively..


        map<pair<int, int> , vector<vector<int>>> adj;
        for(int i = 1; i <= m; i++)
        {
            int u, v, st, en;
            cin >> u >> v >> st >> en;
            //u denotes the station form whih train is leaving 
            //v denotes the station fomr which train is arriving to..
            //st is time of departre from the station u and en is the arrival time to the station v
            adj[{u, st}].push_back({v, en, 1});
        }

        for(auto it : adj)
        {
            adj[{0, 0}].push_back({it.first.first, it.first.second, 2});
            adj[{n + 1, INF}].push_back({it.second[0][0], it.second[0][1], 2});
        }


        for(auto it : adj)
        {
            vector<int> p = it.second[0];
            for(auto i : adj)
            {
                vector<int> u = i.second[0];
                int time = u[1];
                if(x + p[1] <= time)
                    adj[{p[0], p[1]}].push_back({u[0], u[1], 2}); 
            }
        }
        //add three node first one S which is the starting node connected to the D node that

        int l = 1, r = m;

        int ans = m;

        while(l <= r)
        {
            int mid = (l + r) / 2;

            NodeID nodeID;
    
            for(auto &pr : adj)
            {
                nodeID.getID(pr.first.first, pr.first.second);
                for(auto &vec : pr.second)
                    nodeID.getID(vec[0], vec[1]);
            }
            int source = nodeID.getID(0, 0);
            int sink   = nodeID.getID(n+1, INF);

            int superSource = nodeID.idx++;
            int totalNodes = nodeID.idx;
            Dinic dinic(totalNodes);


            for(auto &pr : adj)
            {
                int u = nodeID.getID(pr.first.first, pr.first.second);
                for(auto &vec : pr.second)
                {
                    int v = nodeID.getID(vec[0], vec[1]);
                    dinic.addEdge(u, v, 1);
                }
            }
            dinic.addEdge(superSource, source, mid);



            unordered_map<int,int> scheduleSupply;
            for(auto &pr : adj){
                int u = nodeID.getID(pr.first.first, pr.first.second);
                for(auto &vec : pr.second){
                    if(vec[2] == 1) {
                        scheduleSupply[u]++;
                    }
                }
            }

            for(auto &p : scheduleSupply)
                dinic.addEdge(superSource, p.first, p.second);
            
            int flow = dinic.maxFlow(superSource, sink);

            if(flow == m) {
                ans = mid;
                r = mid - 1;
            } else {
                l = mid + 1;
            }
        }

        cout << ans << endl;

    }
}