#include <bits/stdc++.h>
using namespace std;

struct edge {
    int u, v, d, w;
    bool operator<(const edge& other) const { return d < other.d; }
};

int convertToMinutes(const std::string& dayTime) {
    unordered_map<string, int> dayToMinutes = {
        {"Mon", 0}, {"Tue", 1440}, {"Wed", 2880}, {"Thu", 4320}, 
        {"Fri", 5760}, {"Sat", 7200}, {"Sun", 8640}
    };
    istringstream ss(dayTime);
    string day, time;
    ss >> day >> time;
    int hours = stoi(time.substr(0, 2));
    int minutes = stoi(time.substr(3, 2));
    return dayToMinutes[day] + hours * 60 + minutes;
}

int main() {
    ifstream file("train_edges_repeated_days.csv");
    string line;
    map<string, int> stationIndex;
    map<int, string> indexToStation;  // Reverse lookup map
    int cnt = 0;
    vector<edge> edges;

    set<string> homeStations = {
        "TVC", "KCVL", "NCJ", "CAPE", "ERS", 
        "QLN", "ERN", "MDU", "TEN", "SRR", 
        "PGT", "CLT", "ED"
    };

    getline(file, line);
    while (getline(file, line)) {
        istringstream ss(line);
        string tstr, ustr, dstr, vstr, astr, wstr;
        
        getline(ss, tstr, ',');
        getline(ss, ustr, ',');
        getline(ss, dstr, ',');
        getline(ss, vstr, ',');
        getline(ss, astr, ',');
        getline(ss, wstr, ',');

        if (stationIndex.find(ustr) == stationIndex.end()) {
            stationIndex[ustr] = cnt;
            indexToStation[cnt] = ustr;  // Store reverse mapping
            cnt++;
        }
        if (stationIndex.find(vstr) == stationIndex.end()) {
            stationIndex[vstr] = cnt;
            indexToStation[cnt] = vstr;  // Store reverse mapping
            cnt++;
        }

        int u = stationIndex[ustr];
        int v = stationIndex[vstr];
        int d = convertToMinutes(dstr);
        int a = convertToMinutes(astr);
        if (a < d) a += 10080;
        int w = a - d;
        
        if (w <= 8 * 60)  // Ignore edges longer than 8 hours
            edges.push_back({u, v, d, w});
    }

    vector<vector<edge>> adj(cnt);
    for (auto e : edges) {
        adj[e.u].push_back(e);
    }
    for (int i = 0; i < cnt; i++) {
        sort(adj[i].begin(), adj[i].end());
    }

    int crewRequired = 0;
    set<tuple<int, int, int>> visited;

    for (const auto& [stationName, startStation] : stationIndex) {
        for (auto& e : adj[startStation]) {
            if (visited.count({e.u, e.v, e.d})) continue;

            int currentStation = e.v;
            int currentTime = e.d + e.w;
            int travelTime = e.w;
            bool completedTrip = false;

            while (travelTime <= 8 * 60) {
                bool foundNext = false;

                for (auto& next : adj[currentStation]) {
                    if (visited.count({currentStation, next.v, next.d})) continue;
                    if (next.d >= currentTime) {
                        travelTime += next.w;
                        currentTime = next.d + next.w;
                        currentStation = next.v;
                        visited.insert({currentStation, next.v, next.d});
                        foundNext = true;
                        break;
                    }
                }
                if (!foundNext) break;

                // Check if the return trip goes to a home station
                for (auto& returnTrip : adj[currentStation]) {
                    const string& returnStationName = indexToStation[returnTrip.v]; // Get the station name
                    if (returnTrip.d >= currentTime + 8 * 60 && homeStations.count(returnStationName)) {
                        crewRequired++;
                        completedTrip = true;
                        visited.insert({currentStation, returnTrip.v, returnTrip.d});
                        break;
                    }
                }
                if (completedTrip) break;
            }

            if (completedTrip) continue;
        }
    }

    cout << "Minimum crew members required: " << crewRequired << endl;
    return 0;
}
