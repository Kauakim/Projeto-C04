#include <iostream>
#include <list>
#include <iomanip>
#include "structs.h"
#include "graphs.h"

using namespace std;

void buildAndDisplayCompleteGraph(Dictionary& dictionary)
{
    if (dictionary.edges.empty()) {
        cout << "\nNo edges in the graph yet. Insert edges first (options 2 or 4)." << endl;
        return;
    }

    cout << "\n=== COMPLETE GRAPH ===" << endl;
    int edgeCount = 0;

    for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end(); ++it) {
        cout << it->source->word << " -- " << it->target->word
             << " (similarity: " << fixed << setprecision(2) << it->similarity << ")" << endl;
        edgeCount++;
    }

    cout << "\nTotal edges: " << edgeCount << endl;
}
