#include<iostream>          // For standard I/O operations
#include<vector>            // For using vectors to store adjacency list
#include<queue>             // For BFS queue
#include<stack>             // Not used here, but typically for DFS
#include<omp.h>             // For OpenMP parallelism
using namespace std;

// Graph class to represent an undirected graph
class Graph {
    int V;                          // Number of vertices
    vector<vector<int>> adj;       // Adjacency list

public:
    // Constructor to initialize the graph with V vertices
    Graph(int V) : V(V) {
        adj.resize(V);             // Resize adjacency list to hold V vertices
    }

    // Function to add an undirected edge between u and v
    void addEdge(int u, int v) {
        adj[u].push_back(v);       // Add v to u's adjacency list
        adj[v].push_back(u);       // Add u to v's adjacency list (undirected)
    }

    void parallelBFS(int start);   // Function to perform parallel BFS
    void parallelDFS(int start);   // Function to perform parallel DFS

private:
    void dfsUtil(int v, vector<bool>& visited); // Utility function for DFS
};

// Parallel BFS using OpenMP (with limitations due to shared queue)
void Graph::parallelBFS(int start) {
    vector<bool> visited(V, false);    // Track visited nodes
    queue<int> q;                      // Queue for BFS

    visited[start] = true;             // Mark start node as visited
    q.push(start);                     // Enqueue start node

    while (!q.empty()) {
        int levelSize = q.size();      // Get current level size (breadth-wise)

        // Parallelize processing of all nodes at the current BFS level
        #pragma omp parallel for
        for (int i = 0; i < levelSize; ++i) {
            int u;

            // Critical section to safely dequeue a node
            #pragma omp critical
            {
                u = q.front();         // Get front node
                q.pop();               // Remove it from the queue
                cout << u << " ";      // Print the node
            }

            // Process all adjacent nodes of u
            for (int v : adj[u]) {
                if (!visited[v]) {
                    // Critical section to avoid race conditions when updating visited and queue
                    #pragma omp critical
                    {
                        if (!visited[v]) {
                            visited[v] = true;   // Mark node as visited
                            q.push(v);           // Enqueue the node
                        }
                    }
                }
            }
        }
    }
}

// Utility function for DFS using recursion and OpenMP tasks
void Graph::dfsUtil(int v, vector<bool>& visited) {
    // Critical section to avoid simultaneous console output
    #pragma omp critical
    {
        cout << v << " ";             // Print the current node
    }

    visited[v] = true;                // Mark node as visited

    // Parallelize the iteration over adjacent nodes
    #pragma omp parallel for
    for (int i = 0; i < adj[v].size(); ++i) {
        int u = adj[v][i];            // Get adjacent node
        if (!visited[u]) {
/*  Spawn a new parallel task for recursive DFS
    When the compiler encounters #pragma omp task, it:
    Creates a new task from the following block of code.
    This task can then be executed by any available thread in the OpenMP thread pool (not necessarily the thread that created it).
    Execution may happen immediately, later, or concurrently, depending on runtime scheduling.
*/

            #pragma omp task
            dfsUtil(u, visited);      // Recursively visit unvisited node
        }
    }

    #pragma omp taskwait              // Wait for all parallel DFS tasks to complete
}

// Driver function to start parallel DFS
void Graph::parallelDFS(int start) {
    vector<bool> visited(V, false);  // Initialize visited array

    // Start parallel region
    #pragma omp parallel
    {
        // Ensure only one thread initiates the DFS
        #pragma omp single
        dfsUtil(start, visited);     // Call DFS utility
    }
}

int main() {
    Graph g(7);                      // Create a graph with 7 vertices

    // Add edges to the graph
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(2, 6);

    cout << "\nParallel BFS starting from node 0:\n";
    g.parallelBFS(0);                // Run parallel BFS from node 0

    cout << "\n\nParallel DFS starting from node 0:\n";
    g.parallelDFS(0);                // Run parallel DFS from node 0

    return 0;                        // Exit the program
}
