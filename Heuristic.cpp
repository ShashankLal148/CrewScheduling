#include <bits/stdc++.h>
using namespace std;

struct edge {
    int u; //Start node
    int v; //End node
    int d; //Departure time
    int w; //Weight

    bool operator < (const edge n) const
    {
        return d < n.d;
    }
};

int64_t gen() 
{
    std::random_device rd;  
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(0, 1e18);
    return dist(gen);
}

//Convert to minutes, where monday 00:00 is 0
int get_minutes(string dayTime) {
    unordered_map<string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };

    istringstream ss(dayTime);
    string day, time;
    ss >> day >> time;
    //cout << day << " " << time << endl;
    int hours, minutes;
    if(time.length() == 5)
    {
        hours = stoi(time.substr(0, 2));
        minutes = stoi(time.substr(3, 2));
    }
    else
    {
        hours = stoi(time.substr(0, 1));
        minutes = stoi(time.substr(2, 2));
    }
    return dayToMinutes[day] + hours * 60 + minutes;
}

int main() 
{
    map<string, int> ind; //Map station names to indices for easier access
    int cnt = 0; //Number of nodes
    vector<edge> edges; //Edge list
    set<string> homeStations = {
        "TVC", "KCVL", "NCJ", "CAPE", "ERS", 
        "QLN", "ERN", "MDU", "TEN", "SRR", 
        "PGT", "CLT", "ED"
    }; //Home stations

    ifstream file("train_edges_repeated_days.csv");
    string line;

    getline(file, line); //Ignore header

    while (getline(file, line)) {
        istringstream ss(line);
        //trainNo, startStation, departureTime, endStation, arrivalTime, weight
        string tstr, ustr, dstr, vstr, astr, wstr;

        getline(ss, tstr, ',');
        getline(ss, ustr, ',');
        getline(ss, dstr, ',');
        getline(ss, vstr, ',');
        getline(ss, astr, ',');
        getline(ss, wstr, ',');

        //Assign indices to nodes
        if(ind.find(ustr) == ind.end())
            ind[ustr] = cnt++;

        if(ind.find(vstr) == ind.end()) 
            ind[vstr] = cnt++;


        int u = ind[ustr];
        int v = ind[vstr];
        int d = get_minutes(dstr);
        int a = get_minutes(astr);

        //Start on a Sunady, end on a Monday case
        if(a < d)
            a += 10080;
        int w = a - d;
        if(w<480)
        edges.push_back({u, v, d, w});
    }

    set<int> homeCodes;
    for(string s: homeStations)
        homeCodes.insert(ind[s]);

    set<int> available = homeCodes; //We can start from any of these nodes
    set<int> blocked; // These edges have already been used

    vector<vector<int>> adj(cnt);

    for(int i = 0; i < edges.size(); i++)
    {
        auto [u, v, d, w] = edges[i];
        adj[u].push_back(i);
    }

    int week = 10080; //Minutes in a week
    int rest = 480; //Minutes for a rest
    vector<int> used; //We will use this vector to store paths
    
    //For simplicity we assume all paths go from home station i to external station j, and then back to i in the reverse order
    //Can I go back along "nodes" if started from home at "start", already travelled for "time" minutes, and have "rem" minutes before resting again?

    function<bool(int, stack<int>, int, int, int)> reach = [&](int start, stack<int> nodes, int time, int rem, int home)
    {
        int i = nodes.top();
        int t = time;
        int r = rem;
        nodes.pop();
        used.clear();
        set<int> extra;
        while(!nodes.empty())
        {
            int j = nodes.top();
            //Best edge to take to go from i to j
            int best = -1;
            int bestd = INT_MAX;
            for(int k: adj[i])
            {
                //Must not visit this edge twice!
                if(blocked.find(k) != blocked.end() || extra.find(k) != extra.end())
                    continue;
                auto [u, v, d, w] = edges[k];
                //We must go to j
                if(v != j)
                    continue;
                //Take this train next week
                if(d < start)
                    d += week;
                if(d >= t)
                {
                    if(d < bestd)
                    {
                        //Take earliest train 
                        best = k;
                        bestd = d;
                    }
                }
            }
            //No trains available?
            if(bestd == INT_MAX)
                return false;
            
            t = bestd + edges[best].w;
            //Take rest if needed
            r += edges[best].w;
            if(r > rest)
            {
                r = 0;
                t += rest;
            }
            used.push_back(best);
            extra.insert(best);
            nodes.pop();
            i = j;
        }
        //Can I reach home in less than a week?
        return t < (start + week);
    };

    int people = 0;

    while(!available.empty())
    {
        //Choose a random home station which still has potential for routes
        int homeInd = gen()%(int)available.size();
        int home;
        for(int i: available)
        {
            if(homeInd)
                homeInd--;
            else
            {
                home = i;
                break;
            }
        }

        int start = -1; //When do we start from the home station
        int time = 0; //How much time has passed
        int rem = 0; //Remaining time for rest

        vector<int> save; //Store the last valid reverse path
        stack<int> path; path.push(home); //Current path
        int i = home;

        while(true)
        {
            //Have a found a valid edge to traverse?
            bool found = false;
            for(int j: adj[i])
            {
                if(blocked.find(j) != blocked.end())
                    continue;
                
                auto [u, v, d, w] = edges[j];
                
                //New time, rem, and start upon traversing this edge
                int nt, nr, ns;
                if(start == -1)
                {
                    //First edge, its optimal to start at departure time
                    ns = d;
                    nr = w;
                    nt = d+w;
                }
                else 
                {
                    ns = start;
                    //Take this train next week
                    if(d < ns)
                        d += week;
                    //We arrived after departure time
                    if(d < time)
                        continue;
                    nt = d + w;
                    nr = rem + w;
                    if(nr > rest)
                    {
                        nr = 0;
                        nt += rest;
                    }
                }

                //Block this edge, and push this vertex onto stack
                blocked.insert(j);
                path.push(v);
                //Can we now go back to home?
                if(!reach(ns, path, nt, nr, home))
                {
                    //If we cannot reach, then continue to next edge option
                    blocked.erase(j);
                    path.pop();
                    continue;
                }
                //Otherwise traverse this edge and update the variables
                found = true;
                i = v;
                time = nt;
                rem = nr;
                start = ns;
                break;
            }
            //If found keep traversing
            if(found)
            {
                save = used;
                continue;
            }
            //Else go along reverse path back to home
            //If only one node, no point in sending a person here
            if(path.size() == 1)
                available.erase(home);
            else
            {
                //Else add a person here and block all the reverse edges
                while(!path.empty())
                {
                    // cout << path.top() << " ";
                    path.pop();
                }
                cout << endl;

                for(int i: save)
                {
                    blocked.insert(i);
                    // cout << i << " ";
                }
                cout << endl;
                people++;
            }
            break;
        }
    }

    cout << people << endl;
    cout << (int)edges.size() - (int)blocked.size() << endl;

    return 0;
}