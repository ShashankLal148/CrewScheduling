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

int SMALL;

void readFile(vector<edge>& edges, int& cnt, map<string, int>& ind)
{
    SMALL = 0;
    map<string, string> days;
    days["MO"] = "Mon";
    days["TU"] = "Tue";
    days["WE"] = "Wed";
    days["TH"] = "Thu";
    days["FR"] = "Fri";
    days["SA"] = "Sat";
    days["SU"] = "Sun";

    ifstream file("train_edges_repeated_days.csv");
    string line;

    getline(file, line); //Ignore header
    vector<edge> edges2;
      set<pair<int,int>> st;
      int c=0;
    while (getline(file, line)) {

        istringstream ss(line);
        //trainNo, startStation, departureTime, endStation, arrivalTime, weight
        string tstr, ustr, dstr, vstr, astr, wstr;

        //This is for data.csv
        getline(ss, tstr, ',');
        getline(ss, ustr, ',');
        getline(ss, dstr, ',');
        getline(ss, vstr, ',');
        getline(ss, astr, ',');
        getline(ss, wstr, ',');

        //This is for data2.csv
        // string dstr1, dstr2, astr1, astr2;
        // getline(ss, dstr1, ',');
        // getline(ss, dstr2, ',');
        // getline(ss, ustr, ',');
        // getline(ss, vstr, ',');
        // getline(ss, astr1, ',');
        // getline(ss, astr2, ',');

        // while(astr2.length() > 2)
        // {
        //     astr2.pop_back();
        // }
        // assert(days.find(dstr2) != days.end());
        // assert(days.find(astr2) != days.end());
        // dstr = days[dstr2] + " " + dstr1; 
        // astr = days[astr2] + " " + astr1; 


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
        
      
        if(w<400)
        {
            st.insert({u,v});
            edges2.push_back({u,v,d,w});
        }
        // cout<<"Hi"<<endl;
        // if(!getline(file, line))
        //     break;
        //cout << (double)w/60 << endl;
        // cout<<c<<endl;
        c++;
        // cout<<c<<endl;
        if(c==5159)
            break;
    }
    for(auto it:edges2)
        {
            if(st.find({it.v,it.u})!=st.end())
                edges.push_back({it.u, it.v, it.d, it.w});
        }
        cout<<edges.size();
}
int calc(map<string, int> ind, vector<edge> edges, int cnt, int threshold) 
{
    set<string> homeStations = {
        "TVC", "KCVL", "NCJ", "CAPE", "ERS", 
        "QLN", "ERN", "MDU", "TEN", "SRR", 
        "PGT", "CLT", "ED"
    };

    set<int> homeCodes;
    for (string s : homeStations) homeCodes.insert(ind[s]);

    set<int> available = homeCodes;
    set<int> blocked;
    vector<vector<int>> adj(cnt);
    // for(auto it:edges)
    // {
    //     cout<<it.u<<" "<<it.v<<" "<<it.w<<" "<<it.d<<endl;
    // }
    for (int i = 0; i < edges.size(); i++) {
        adj[edges[i].u].push_back(i);
    }

    const int week = 10080; // Minutes in a week
    const int rest = 960;   // Minutes for a rest
    int people = 0;

    while (!available.empty()) {
        auto it = available.begin();
        advance(it, gen() % available.size());
        int home = *it;

        int start = -1; // Start time from home station
        int time = 0;   // Time elapsed during the trip
        bool progress = false; // Track progress in edge traversal
        int iterationCount = 0; // Fail-safe to avoid infinite loops

        while (true) {
            if (++iterationCount > 10000) { // Arbitrary iteration limit
                cerr << "Error: Exceeded iteration limit for node " << home << endl;
                break;
            }

            array<int, 3> best = {INT_MAX, -1, -1};
            progress = false;

            for (int j : adj[home]) {
                if (blocked.find(j) != blocked.end()) continue;

                auto [u, v, d, w] = edges[j];
                int nstart = start == -1 ? d : start;
                int ntime = start == -1 ? d : time;

                if (d < nstart) d += week;
                if (ntime > d) continue;

                ntime = d + w;
                int ctime = INT_MAX, K = -1;

                for (int k : adj[v]) {
                    if (blocked.find(k) != blocked.end()) continue;
                    auto [u2, v2, d2, w2] = edges[k];
                    if (v2 != u) continue;
                    if (d2 < nstart) d2 += week;
                    if (ntime > d2) continue;

                    if (d2 + w2 < ctime) {
                        ctime = d2 + w2;
                        K = k;
                    }
                }

                array<int, 3> opt = {ctime - nstart, j, K};
                if (opt < best) best = opt;
            }

            if (best[0] == INT_MAX) break; // No valid edges found

            progress = true; // Progress is made
            if (start == -1) start = edges[best[1]].d;
            time = start + best[0] + rest;
            blocked.insert(best[1]);
            if (best[2] != -1) blocked.insert(best[2]);
        }

        if (!progress) {
            available.erase(home); // Remove node if no progress
        } else {
            people++;
            if (people >= threshold) break;
        }
    }

    return people;
}





int main()
{
    map<string, int> ind; //Map station names to indices for easier access
    int cnt = 0; //Number of nodes
    vector<edge> edges; //Edge list
    readFile(edges, cnt, ind);
    int mi = INT_MAX;
    int ma = INT_MIN;
    double total = 0;
    for(int i = 0; i < 5; i++)
    {
        int p = calc(ind, edges, cnt, INT_MAX);
        cout<<p<<endl;
        total += p;
        mi = min(mi, p);
        ma = max(ma, p);
    }

    cout << "----------------" << endl;

    cout << mi << endl;
    cout << ma << endl;
    cout << total/5 << endl;
    return 0;
}