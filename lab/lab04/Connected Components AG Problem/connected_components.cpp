/*  Identifier = 15C1680EE94C640EC35E1694295A3625C3254CBA

    EECS 281 Lab 4: Connected Components

    For this lab, you will write a program that calculates the
    number of connected components in an undirected graph.

    Your program will receive an input file in the following format:
    1) The first line of input contains the number of vertices V and number
       of edges E in the graph.
    2) The next E lines list out the connections between vertices in the
       graph in the format of "u v", which indicates the existence of an edge
       between u and v.

    For example, the following input file would be read as follows:
    4 2  (the graph has 4 vertices numbered from 0-3, and two edges exist in
          the graph)
    0 1  (the first of these edges connects vertex 0 with vertex 1)
    0 2  (the second of these edges connects vertex 0 with vertex 2)

    In this case, vertices {0, 1, 2} form a connected component, and vertex
    {3} forms a connected component (since 3 is not connected to anything).
    Thus, your program should print out 2.

    You will be using union-find to complete this lab. The following starter
    code has been provided. Feel free to modify this code in your
    implementation. Good luck!
*/

#include <iostream>
using std::cin;
using std::cout;
using std::endl;
#include <vector>
using std::vector;


class Graph {
    uint32_t V;
    uint32_t E;
    vector<uint32_t> com;
    vector<uint32_t> stats;

    // TODO: add any additional member variables, as needed

public:
    // Graph constructor that initializes the graph and its member variables
    Graph(uint32_t v, uint32_t e) : V{v}, E{e} {
        // TODO: initialize member variables that you added above
        V = v, E = e;
        uint32_t link_a = 0, link_b = 0;
        com.reserve(V);
        stats.reserve(V);

        for (uint32_t i = 0; i < V; i++) com.push_back(i);
        for (uint32_t i = 0; i < E; i++)
        {
            cin >> link_a;
            cin >> link_b;
            if (com[link_a] == link_a) 
            {
                if (link_a != find_set(link_b)) com[link_a] = link_b;
            }
            else if (com[link_a] != link_b) union_set(link_a, com[link_b]);
        }

    }  // Graph()


    uint32_t find_set(uint32_t v) {
        // TODO: implement the find_set function
        while (com[v] != v) v = com[v];
        return v;
    }  // find_set()


    void union_set(uint32_t a, uint32_t b) {
        // TODO: implement the union_set function
        uint32_t idx_a = find_set(a), idx_b = find_set(b), cur = 0, temp = 0;
        if (idx_a < idx_b)
        {
            com[idx_b] = idx_a;
            cur = b;
        }
        else
        {
            com[idx_a] = idx_b;
            idx_a = idx_b;
            cur = b;
        }
        
        while(idx_a != cur)
        {
            temp = com[cur];
            com[cur] = idx_a;
            cur = temp;
        }

    }  // union_set()


    uint32_t count_components() {
        // TODO: implement the count_components function
        uint32_t count = 0;
        for (uint32_t i = 0; i < V; ++i) {
            if (com[i] == i) stats[i] = 1;
            else {
                int root = find_set(i);
                stats[root] = 1;
            }
        }
        for (uint32_t i = 0; i < V; ++i) count += stats[i];
        return count;
    }  // count_components()
};  // Graph{}


int main() {
    // You do not need to modify main.
    std::ios_base::sync_with_stdio(false);
    uint32_t vertex_count, edge_count = 0;

    cin >> vertex_count;
    cin >> edge_count;

    Graph g(vertex_count, edge_count);

    cout << g.count_components() << endl;
    return 0;
}  // main()
