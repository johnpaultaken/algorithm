#include <iostream>
using std::cout;

#include <memory>
using std::shared_ptr;
using std::make_shared;

#include <deque>
using std::deque;

#include <vector>
using std::vector;

#include <set>
using std::set;

#include <limits>
constexpr unsigned int UMAX = std::numeric_limits<unsigned int>::max();

using std::pair;
using std::make_pair;

struct node_t;

// 1. No negative weight for now.
//    Allowing negative weight would need preventing loops in path ?
// 2. Making _to a reference is to allow edges also to be fully constructed when the node is constructed.
//    This makes graph initialization in main() more readable.
struct edge_t
{
    unsigned int _weight;
    shared_ptr<node_t> & _to;
};

struct node_t
{
    unsigned int _id;
    deque<edge_t> _edges_out;
};

using graph_t = deque<shared_ptr<node_t>>;
using path_t = deque<shared_ptr<node_t>>;

//
// Using BFS.
// Notes:
// 1. Nodes are included in the next level of BFS traversal only if a shorter path to them was found in the current level.
// 2. A set<node_t> tracks the next level of BFS traversal because a shorter path to a node could be found more than once
//    in the current level of traversal. In such case we want to avoid duplicate processing of the same node in the next level.
// 3. There is no need to keep track of the current BFS traversal path because the current shortest path to the node being
//    processed is same as that.
//
// Params :
// graph - nodes of graph. These nodes must be sorted by id starting from 0.
// from  - node path starts from.
// Return : sortest path to all other nodes
//
pair<vector<path_t>, vector<unsigned int>>
shortest_path(
    const graph_t & graph,
    const shared_ptr<node_t> & from)
{
    // verify graph nodes are sorted by id
    for (unsigned int i = 0; i<graph.size(); ++i)
    {
        if (graph[i]->_id != i)
        {
            throw std::invalid_argument("graph nodes not sorted by id.");
        }
    }
    // keeps track of the current shortest path to each node
    vector<path_t> shortest_paths(graph.size());
    shortest_paths[from->_id].push_back(from);
    // keeps track of the current shortest distance to each node
    vector<unsigned int> shortest_distances(graph.size(), UMAX);
    shortest_distances[from->_id] = 0;

    // nodes selected for the current and next BFS traversal levels.
    set<shared_ptr<node_t>> current, next;
    current.insert(from);
    while (!current.empty())
    {
        for (auto & node : current)
        {
            for (auto & edge : node->_edges_out)
            {
                auto new_distance = shortest_distances[node->_id] + edge._weight;
                if (new_distance < shortest_distances[edge._to->_id])
                {
                    shortest_paths[edge._to->_id] = shortest_paths[node->_id];
                    shortest_paths[edge._to->_id].push_back(edge._to);

                    shortest_distances[edge._to->_id] = new_distance;

                    next.insert(edge._to);
                }
            }
        }
        current = std::move(next);
    }
    return make_pair(shortest_paths, shortest_distances);
}

void display(pair<vector<path_t>, vector<unsigned int>> & result)
{
    cout << "\ndistance \t path";
    auto paths = result.first;
    auto distances = result.second;
    for (size_t i = 0; i < distances.size(); ++i)
    {
        cout << "\n" << distances[i] << " \t\t ";
        for (auto & node : paths[i])
        {
            cout << node->_id << " ";
        }
    }
}

int main()
{
    shared_ptr<node_t> zero, one, two, three;
    zero = make_shared<node_t>(node_t{ 0,{ { 5, one },{ 15, two } } });
    one = make_shared<node_t>(node_t{ 1,{ { 5, zero },{ 6, two } } });
    two = make_shared<node_t>(node_t{ 2,{ { 15, zero },{ 6, one },{ 2, three } } });
    three = make_shared<node_t>(node_t{ 3,{ { 2, two } } });
    graph_t graph{ zero, one, two, three };

    auto result = shortest_path(graph, zero);

    display(result);
    cout << "\ndone";
}
