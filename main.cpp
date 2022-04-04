#include <fstream>
#include <iostream>
#include <omp.h>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#define NO_OF_THREADS 4

using namespace std;

// Class to encapsulate node
class Node {
public:
  // Each node has an integer ID corrosponding
  // to it's index in edgeMatrix
  short unsigned int id;

  // Static counter to assign default value to id
  inline static short unsigned int counter = 0;

  // Has node been discoverd by BFS
  bool discovered_serial;
  bool discovered_parallel;

  // Distance form root node chosen for BFS
  // d = 0 for root node
  short int d_serial;
  short int d_parallel;

  // Construct undiscovered node with
  // infinite(-1) distance form root
  Node() {
    id = counter++;
    discovered_serial = false;
    discovered_parallel = false;
    d_serial = -1;
    d_parallel = -1;
  }
};

// Class to encapsulate graph
class Graph {
public:
  // Number of nodes in the graph
  unsigned short int no_of_nodes;

  // Edge matrix vector
  vector<vector<bool>> edgeMatrix;

  // Node vector
  vector<Node> N;

  // Construct trivial graph with n nodes
  Graph(int n) {
    no_of_nodes = n;

    // Initialize edgeMatrix
    edgeMatrix = vector<vector<bool>>(n, vector<bool>(n));

    // Initialize node vector
    N = vector<Node>(n);
  }

  // Add edge from node s to node d
  void add_edge(unsigned short int s, unsigned short int d) {
    edgeMatrix[s][d] = true;
  }

  // Print adjacency matrix
  void print_matrix() {
    cout << "Adjacency Matrix of the graph is: " << endl;
    for (auto i : edgeMatrix) {
      for (int j = 0; j < i.size() - 1; ++j) {
        cout << i[j] << ", ";
      }
      cout << i[i.size() - 1] << endl;
    }
  }

  // Print distance of all nodes after BFS
  void print_distance() {
    cout << "Node ID\t|Serial\t |Parallel" << endl;
    cout << "\t|Distance|Distance" << endl;
    cout << "--------|--------|--------" << endl;
    for (auto i : N) {
      cout << i.id << "\t|" << i.d_serial << "\t |" << i.d_parallel << endl;
    }
    cout << "--------|--------|--------" << endl;
  }

  // Serial BFS
  void BFS_serial(unsigned short int);

  // TODO Implement Parallel BFS
  void BFS_parallel(unsigned short int);
};

void Graph::BFS_serial(unsigned short int s_id) {
  // s is now pointer to source
  Node *s = &N[s_id];

  // BFS Queue
  queue<Node *> Q;

  // Distance of source will be 0
  s->d_serial = 0;

  // Source has been discovered
  s->discovered_serial = true;

  Q.push(s);

  // BFS algorithm
  while (!Q.empty()) {
    Node *u = Q.front();
    Q.pop();
    for (int v = 0; v < no_of_nodes; ++v) {
      if ((edgeMatrix[u->id][v]) && (!N[v].discovered_serial)) {
        N[v].discovered_serial = true;
        N[v].d_serial = u->d_serial + 1;
        Q.push(&N[v]);
      }
    }
    u->discovered_serial = true;
  }
}

void Graph::BFS_parallel(unsigned short int s_id) {
  // s is now pointer to source
  Node *s = &N[s_id];

  // BFS Queue
  queue<Node *> Q;

  // Distance of source will be 0
  s->d_parallel = 0;

  // Source has been discovered
  s->discovered_parallel = true;

  Q.push(s);

  // BFS algorithm
  while (!Q.empty()) {
    Node *u = Q.front();
    Q.pop();
    #pragma omp parallel for
    for (int v = 0; v < no_of_nodes; ++v) {
      if ((edgeMatrix[u->id][v]) && (!N[v].discovered_parallel)) {
        N[v].discovered_parallel = true;
        N[v].d_parallel = u->d_parallel + 1;

        Q.push(&N[v]);
      }
    }
    u->discovered_parallel = true;
  }
}

int main(void) {
  omp_set_num_threads(NO_OF_THREADS);

  // Graph G(18);

  // G.add_edge(0, 4);
  // G.add_edge(4, 1);
  // G.add_edge(0, 1);
  // G.add_edge(5, 4);
  // G.add_edge(2, 5);
  // G.add_edge(5, 6);
  // G.add_edge(4, 7);
  // G.add_edge(4, 8);
  // G.add_edge(8, 5);
  // G.add_edge(9, 5);
  // G.add_edge(9, 6);
  // G.add_edge(6, 10);
  // G.add_edge(7, 8);
  // G.add_edge(8, 9);
  // G.add_edge(7, 14);
  // G.add_edge(11, 7);
  // G.add_edge(9, 13);
  // G.add_edge(10, 17);
  // G.add_edge(11, 12);
  // G.add_edge(12, 13);
  // G.add_edge(13, 16);
  // G.add_edge(13, 17);
  // G.add_edge(14, 15);
  // G.add_edge(6, 5);
  // G.add_edge(8, 7);
  // G.add_edge(12, 11);
  // G.add_edge(13, 12);
  // G.add_edge(16, 13);
  // G.add_edge(15, 14);

  ifstream fin;
  string line;
  int n = 0;

  // by default open mode = ios::in mode
  fin.open("test.txt");

  // Execute a loop until EOF (End of File)
  while (fin) {
    // Read a Line from File
    getline(fin, line);

    // Print line in Console
    if (line[0] != '#') {
      n = stoi(line);
      cout << line << endl;
      break;
    }
  }

  Graph G(n);

  while (fin) {

    // Read a Line from File
    getline(fin, line);

    istringstream ss(line);

    // For storing each word
    string word;

    // Traverse through all words
    // while loop till we get
    // strings to store in string word
    while (ss >> word) {
      unsigned short int n1 = stoi(word);
      ss >> word;
      unsigned short int n2 = stoi(word);

      G.add_edge(n1, n2);
    }
  }

  // Close the file
  fin.close();

  G.BFS_serial(0);
  G.BFS_parallel(0);
  G.print_distance();
}
