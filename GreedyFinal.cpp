#include <bits/stdc++.h>
using namespace std;

// Struct for representing an edge
struct edge {
    int u; // Starting station
    int v; // Destination station 
    int d; // Departure time in minutes
    int w; // Journey duration 

    bool operator < (const edge n) const {
        return d < n.d; // Sort edges by departure time
    }
};

// Convert "Day HH:MM" to minutes from Sunday 00:00
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

int main() {
    // Input file with repeated day entries
    std::ifstream file("train_edges_repeated_days.csv");
    std::string line;
    map<string, int> ind; // Map station names to unique IDs
    int cnt = 0;
    vector<edge> edges;
    
    // Set of home stations (only these stations can originate a journey)
    std::set<std::string> home = {
        "TVC", "KCVL", "NCJ", "CAPE", "ERS", 
        "QLN", "ERN", "MDU", "TEN", "SRR", 
        "PGT", "CLT", "ED"
    };

    std::getline(file, line); // Skip header line

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        string tstr, ustr, dstr, vstr, astr, wstr;

        std::getline(ss, tstr, ',');
        std::getline(ss, ustr, ',');
        std::getline(ss, dstr, ',');
        std::getline(ss, vstr, ',');
        std::getline(ss, astr, ',');
        std::getline(ss, wstr, ',');

        // Assign new IDs for previously unseen stations
        if (ind.find(ustr) == ind.end()) {
            ind[ustr] = cnt++;
        }
        if (ind.find(vstr) == ind.end()) {
            ind[vstr] = cnt++;
        }

        int u = ind[ustr];
        int v = ind[vstr];
        int d = convertToMinutes(dstr);
        int a = convertToMinutes(astr);
        if (a < d)
            a += 10080; // Adjust for arrival on next week day
        int w = a - d;
        // Only include journeys with duration less than 8 hours
        if (w < 8 * 60)
            edges.push_back({u, v, d, w});
    }
    
    // Convert home station names to their corresponding IDs
    set<int> homeCodes;
    for (string s : home)
        homeCodes.insert(ind[s]);

    cout << cnt << endl;
    sort(edges.begin(), edges.end());
    
    // Waiting queue: Each station holds a set of tuples (arrival time, origin station, cumulative travel time)
    vector<multiset<tuple<int, int, int>>> wait(cnt);

    int invalid = 0;
    int people = 0;
    int stuck = 0;
    // Process each journey, updating waiting queues at stations
    for (auto [u, v, d, w] : edges) {
        bool send = false;
        for (auto [t, i, cum_w] : wait[u]) {
            if (t > d)
                break;
            // Proceed if cumulative travel time is within 8 hours
            if (cum_w + w <= 8 * 60) {
                wait[u].erase(wait[u].find({t, i, cum_w}));
                wait[v].insert({d + w , i, cum_w + w});
                send = true;
                break;
            } else { // Otherwise, require a rest period of 8 hours before continuing
                wait[u].erase(wait[u].find({t, i, cum_w}));
                wait[v].insert({d + w + 8 * 60, i, 0});
                send = true;
                break;
            }
        }
        // If no journey is taken from current station, start a new journey only if from a home station
        if (!send) {
            if (homeCodes.find(u) != homeCodes.end())
                wait[v].insert({d + w, u, w});
            else {
                invalid++;
            }
        }
    }
    
    // Count people left waiting, and check if they're stuck away from their home station
    for (int i = 0; i < cnt; i++) {
        for (auto [t, j, cum_w] : wait[i]) {
            people++;
            if (i != j)
                stuck++;
        }
    }
    
    cout << people << " " << stuck << " " << invalid << endl;
    return 0;
}
