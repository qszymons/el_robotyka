#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// siatka 2d, sprawdzanie punktów
struct Node {
    int x, y;

    bool operator<(const Node& other) const {
        return tie(x, y) < tie(other.x, other.y);
    }
    bool operator==(const Node& other) const {
        return x == other.x && y == other.y;
    }
};

//liczy dystans między punktami
double heuristic(const Node& a, const Node& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) +
                (a.y - b.y) * (a.y - b.y));
}

//otwarzanie ścieszki
vector<Node> reconstruct_path(map<Node, Node>& cameFrom, Node current) {
    vector<Node> total_path;
    total_path.push_back(current);

    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom[current];
        total_path.push_back(current);
    }

    reverse(total_path.begin(), total_path.end());
    return total_path;
}

// sam w sobie algorytm a*
vector<Node> A_Star(
    const Node& start,
    const Node& goal,
    const vector<vector<int>>& grid)
{
    int rows = grid.size();
    int cols = grid[0].size();

    struct PQItem {
        Node node;
        double fScore;
        bool operator>(const PQItem& other) const {
            return fScore > other.fScore;
        }
    };

    priority_queue<PQItem, vector<PQItem>, greater<PQItem>> openSet;
    openSet.push({ start, heuristic(start, goal) });

    map<Node, Node> cameFrom;
    map<Node, double> gScore;
    map<Node, double> fScore;

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    
    vector<pair<int, int>> directions = {
        {0, 1},   
        {0, -1},  
        {-1, 0},  
        {1, 0}    
    };

    set<pair<int,int>> openSetLookup;
    openSetLookup.insert({start.x, start.y});

    while (!openSet.empty()) {
        Node current = openSet.top().node;
        openSet.pop();
        openSetLookup.erase({current.x, current.y});

        if (current == goal) {
            return reconstruct_path(cameFrom, current);
        }

        for (auto& d : directions) {
            Node neighbor{ current.x + d.first, current.y + d.second };

            if (neighbor.x < 0 || neighbor.x >= cols ||
                neighbor.y < 0 || neighbor.y >= rows)
                continue;

            if (grid[neighbor.y][neighbor.x] == 5)
                continue;

            double tentative_gScore = gScore[current] + 1.0;

            if (!gScore.count(neighbor) || tentative_gScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentative_gScore;
                fScore[neighbor] = tentative_gScore + heuristic(neighbor, goal);

                if (!openSetLookup.count({neighbor.x, neighbor.y})) {
                    openSet.push({ neighbor, fScore[neighbor] });
                    openSetLookup.insert({neighbor.x, neighbor.y});
                }
            }
        }
    }

    return {};
}

// wczytywanie mapki
vector<vector<int>> loadGrid(const string& filename) {
    ifstream file(filename);

    vector<vector<int>> grid;
    string line;

    if (!file.is_open()) {
        cout << "Blad: Nie udalo sie wczytac pliku " << filename << endl;
        return {};
    }

    while (getline(file, line)) {
        vector<int> row;
        for (char c : line) {
            if (c == '0') row.push_back(0);
            else if (c == '5') row.push_back(5);
            else if (c == '3') row.push_back(3);
        }
        grid.push_back(row);
    }

    reverse(grid.begin(), grid.end());

    return grid;
}

// zapis
void savePathToFile(const vector<vector<int>>& grid,
                    const vector<Node>& path,
                    const string& filename)
{
    vector<vector<int>> output = grid;

    for (const Node& n : path) {
        output[n.y][n.x] = 3;
    }

    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Blad: Nie udalo sie zapisac pliku: " << filename << endl;
        return;
    }

    vector<vector<int>> outCopy = output;
    reverse(outCopy.begin(), outCopy.end());

    for (const auto& row : outCopy) {
        for (int cell : row)
            file << cell;
        file << "\n";
    }

    file.close();
    cout << "Sciezka zostala zapisana do: " << filename << endl;
}

int main() {
    vector<vector<int>> grid = loadGrid("F:/tester/grid.txt");
    // zmienić na zajęciach bo mi coś nie działało też z wczytytwaniem


    if (grid.empty()) {
        return 1;
    }

    Node start{0, 0};       
    Node goal{19, 19};       
    vector<Node> path = A_Star(start, goal, grid);

    if (path.empty()) {
        cout << "Brak sciezki!" << endl;
    } else {
        cout << "Znaleziono sciezke!\n";

        savePathToFile(
            grid,
            path,
            "F:/tester/.vscode/path_output.txt"
            // zmienić na zajęciach, ścieszka lokalna do zapisu bo coś nie działo mi
        );
    }

    return 0;
}
