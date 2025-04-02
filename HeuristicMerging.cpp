#include <bits/stdc++.h>
using namespace std;

struct edge {
    int u; // Start node
    int v; // End node
    int d; // Departure time
    int w; // Weight
    int trainNo; // Train number

    bool operator < (const edge& n) const {
        return d < n.d;
    }
};

// Convert day and time to minutes since start of the week
int get_minutes(string dayTime) {
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

// Read CSV file to create edge list
void readFile(vector<edge>& edges, int& cnt, map<string, int>& ind) {
    map<string, string> days = {{"MO", "Mon"}, {"TU", "Tue"}, {"WE", "Wed"},
                                {"TH", "Thu"}, {"FR", "Fri"}, {"SA", "Sat"}, {"SU", "Sun"}};
    ifstream file("train_edges_repeated_days.csv");
    string line;
    getline(file, line); // Ignore header

    while (getline(file, line)) {
        istringstream ss(line);
        string tstr, ustr, dstr, vstr, astr, wstr;

        getline(ss, tstr, ',');  // Train number
        getline(ss, ustr, ',');  // Start station
        getline(ss, dstr, ',');  // Departure time
        getline(ss, vstr, ',');  // End station
        getline(ss, astr, ',');  // Arrival time
        getline(ss, wstr, ',');  // Weight

        // Assign indices to nodes
        if (ind.find(ustr) == ind.end()) ind[ustr] = cnt++;
        if (ind.find(vstr) == ind.end()) ind[vstr] = cnt++;

        int u = ind[ustr];
        int v = ind[vstr];
        int d = get_minutes(dstr);
        int a = get_minutes(astr);
        if (a < d) a += 10080; // Adjust for trips crossing the week boundary
        int w = a - d;

        int trainNo = stoi(tstr); // Convert train number
        if(w<480)
        edges.push_back({u, v, d, w, trainNo});
    }
}

// Merge edges based on criteria
void mergeEdges(vector<edge>& edges) {
    unordered_map<int, vector<edge>> trainEdges;
    for (const auto& e : edges) {
        trainEdges[e.trainNo].push_back(e);
    }

    vector<edge> mergedEdges;
    for (auto& [trainNo, trainEdgeList] : trainEdges) {
        sort(trainEdgeList.begin(), trainEdgeList.end(), [](const edge& a, const edge& b) {
            return a.d < b.d;
        });
        for (size_t i = 0; i < trainEdgeList.size() - 1; ++i) {
            edge& e1 = trainEdgeList[i];
            edge& e2 = trainEdgeList[i + 1];
            if (e1.v == e2.u && e1.w + e2.w < 360) {
                // Merge edges e1 and e2
                mergedEdges.push_back({e1.u, e2.v, e1.d, e1.w + e2.w, trainNo});
                i++; // Skip the next edge since it’s merged
            } else {
                mergedEdges.push_back(e1);
            }
        }
        if (trainEdgeList.size() % 2 != 0) {
            mergedEdges.push_back(trainEdgeList.back());
        }
    }
    edges = mergedEdges;
}

int main() {
    map<string, int> ind; // Map station names to indices
    int cnt = 0;          // Number of nodes
    vector<edge> edges;   // Edge list
    readFile(edges, cnt, ind);

    cout << "Before merging, total edges: " << edges.size() << endl;
    mergeEdges(edges);
    cout << "After merging, total edges: " << edges.size() << endl;

    // Display merged edges
    for (const auto& e : edges) {
        cout << "Train " << e.trainNo << ": " << e.u << " -> " << e.v
             << " | Departure: " << e.d << " | Weight: " << e.w << endl;
    }
    return 0;
}