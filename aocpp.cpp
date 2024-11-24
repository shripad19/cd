#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <climits>

using namespace std;

// Function to calculate the cost for a given condition
map<string, int> calculateCost(
    map<string, int>& H,
    map<string, vector<string>>& condition,
    int weight) {
    
    map<string, int> cost;

    if (condition.count("AND")) {
        vector<string> AND_nodes = condition["AND"];
        string Path_A = accumulate(
            AND_nodes.begin(), AND_nodes.end(), string(),
            [](const string& a, const string& b) {
                return a.empty() ? b : a + " AND " + b;
            });
        
        int PathA = 0;
        for (const auto& node : AND_nodes) {
            PathA += H[node] + weight;
        }
        cost[Path_A] = PathA;
    }

    if (condition.count("OR")) {
        vector<string> OR_nodes = condition["OR"];
        string Path_B = accumulate(
            OR_nodes.begin(), OR_nodes.end(), string(),
            [](const string& a, const string& b) {
                return a.empty() ? b : a + " OR " + b;
            });
        
        int PathB = INT_MAX;
        for (const auto& node : OR_nodes) {
            PathB = min(PathB, H[node] + weight);
        }
        cost[Path_B] = PathB;
    }

    return cost;
}

// Function to update costs for all nodes
map<string, map<string, int>> updateCost(
    map<string, int>& H,
    map<string, map<string, vector<string>>>& Conditions,
    int weight) {
    
    vector<string> Main_nodes;
    for (const auto& pair : Conditions) {
        Main_nodes.push_back(pair.first);
    }
    reverse(Main_nodes.begin(), Main_nodes.end());

    map<string, map<string, int>> least_cost;
    for (const auto& key : Main_nodes) {
        auto condition = Conditions[key];
        auto c = calculateCost(H, condition, weight);
        H[key] = min_element(c.begin(), c.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            })->second;
        least_cost[key] = c;

        cout << key << ": ";
        for (const auto& [path, cost] : c) {
            cout << path << " >>> " << cost << " ";
        }
        cout << endl;
    }

    return least_cost;
}

// Function to find the shortest path
string shortestPath(
    const string& Start,
    map<string, map<string, int>>& Updated_cost,
    map<string, int>& H) {
    
    string Path = Start;
    if (Updated_cost.count(Start)) {
        int Min_cost = min_element(
            Updated_cost[Start].begin(),
            Updated_cost[Start].end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            })->second;

        auto it = find_if(
            Updated_cost[Start].begin(),
            Updated_cost[Start].end(),
            [Min_cost](const auto& a) {
                return a.second == Min_cost;
            });
        
        string Next = it->first;
        vector<string> tokens;
        size_t pos = 0;
        string token;
        while ((pos = Next.find(" ")) != string::npos) {
            token = Next.substr(0, pos);
            tokens.push_back(token);
            Next.erase(0, pos + 1);
        }
        tokens.push_back(Next);

        if (tokens.size() == 1) {
            Path += "<--" + shortestPath(tokens[0], Updated_cost, H);
        } else {
            Path += "<--(" + it->first + ") [";
            Path += shortestPath(tokens[0], Updated_cost, H) + " + ";
            Path += shortestPath(tokens.back(), Updated_cost, H) + "]";
        }
    }

    return Path;
}

// Main function
int main() {
    map<string, int> H = {
        {"A", -1}, {"B", 5}, {"C", 2}, {"D", 4},
        {"E", 7}, {"F", 9}, {"G", 3}, {"H", 0},
        {"I", 0}, {"J", 0}
    };

    map<string, map<string, vector<string>>> Conditions = {
        {"A", {{"OR", {"B"}}, {"AND", {"C", "D"}}}},
        {"B", {{"OR", {"E", "F"}}}},
        {"C", {{"OR", {"G"}}, {"AND", {"H", "I"}}}},
        {"D", {{"OR", {"J"}}}}
    };

    int weight = 1;

    // Update costs and print
    cout << "Updated Cost:" << endl;
    auto Updated_cost = updateCost(H, Conditions, weight);
    cout << string(75, '*') << endl;

    // Find and print shortest path
    cout << "Shortest Path:" << endl;
    cout << shortestPath("A", Updated_cost, H) << endl;

    return 0;
}
