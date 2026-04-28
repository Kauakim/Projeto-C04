// Antônio Vinícius Costa Alves Ferreira - 2282
// Felipe Soares dos Santos Silveira - 843
// Kauã de Oliveira Ribeiro - 777
// Lívia Vilela Ribeiro Guimarães - 797
// Luiz Henrique Azevedo de Carvalho - 813
// Vitor Nolasco Ynoguti - 818

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include "structs.h"
#include "btree.h"
#include "calc.h"
#include "extra.h"
#include "graphs.h"
#include "plots.h"

using namespace std;

void insertWords(Dictionary& dictionary)
{
    ifstream file_words("seed/words.txt");

    string lineWords;

    if (file_words.is_open()) {
        int aux = 0;
        Word word;

        while (getline(file_words, lineWords)) {
            if(aux == 0){
                word = Word();
                word.word = lineWords;
                cout << endl << "Word: " << word.word << endl;
            }
            if(aux == 1){
                word.translation = lineWords;
                cout << "Translation: " << word.translation << endl;
            }
            if(aux == 2){
                stringstream ss(lineWords);
                ss >> word.coordinates.x >> word.coordinates.y >> word.coordinates.z;

                cout << "X: " << word.coordinates.x << endl;
                cout << "Y: " << word.coordinates.y << endl;
                cout << "Z: " << word.coordinates.z << endl;

                if (findWordPointer(word.word, dictionary) != NULL) {
                    cout << "Word already exists, skipping: " << word.word << endl;
                } else {
                    dictionary.words.push_back(word);
                }
            }

            aux = (aux + 1) % 3;
        }
        file_words.close();
    } else {
        cerr << "Unable to open file." << endl;
    }

    cout << "\nWords inserted successfully." << endl;
}

void insertEdges(Dictionary& dictionary){
    ifstream file_edges("seed/edges.txt");

    string lineEdges;

    if (file_edges.is_open()) {
        int aux = 0;
        Edge edge;
        while (getline(file_edges, lineEdges)) {
            if(aux == 0){
                edge = Edge();
                edge.source = findWordPointer(lineEdges, dictionary);

                if (edge.source == NULL) {
                    cerr << "\nSource word not found: " << lineEdges << endl;
                    aux = 0;
                    continue;
                }
            }
            if(aux == 1){
                edge.target = findWordPointer(lineEdges, dictionary);

                if (edge.target == NULL) {
                    cerr << "Target word not found: " << lineEdges << endl;
                    aux = 0;
                    continue;
                }

                if (edge.source == edge.target) {
                    cerr << "Source and target are the same word: " << edge.source->word << endl;
                    aux = 0;
                    continue;
                }

                for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end(); it++) {
                    if (it->source == edge.source && it->target == edge.target || 
                        it->source == edge.target && it->target == edge.source) {
                        cout << "Edge already exists between \"" << edge.source->word << "\" and \"" << edge.target->word << "\", skipping." << endl;
                        aux = 0;
                        break;
                    }
                }

                edge.similarity = calculateSimilarity(*edge.source, *edge.target);

                dictionary.edges.push_back(edge);
            }

            aux = (aux + 1) % 2;
        }
        file_edges.close();
    } else {
        cerr << "Unable to open file." << endl;
    }

    cout << "\nEdges inserted successfully." << endl;
}

void removeWords(Dictionary& dictionary)
{
    string userWord;
    cout << "\nEnter the word to remove: ";
    getline(cin >> ws, userWord);

    Word* wordToRemove = findWordPointer(userWord, dictionary);

    if (wordToRemove == NULL) {
        cout << "\nWord not found: " << userWord << endl;
        return;
    }

    int removedEdges = 0;
    for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end();) {
        if (it->source == wordToRemove || it->target == wordToRemove) {
            it = dictionary.edges.erase(it);
            removedEdges++;
        } else {
            it++;
        }
    }

    for (list<Word>::iterator it = dictionary.words.begin(); it != dictionary.words.end(); it++) {
        if (it->word == userWord) {
            dictionary.words.erase(it);
            cout << "\nWord removed: " << userWord << endl;
            cout << "Edges removed: " << removedEdges << endl;
            return;
        }
    }
}

void removeEdges(Dictionary& dictionary)
{
    string firstWord;
    string secondWord;

    cout << "\nEnter the first word: ";
    getline(cin >> ws, firstWord);
    cout << "Enter the second word: ";
    getline(cin >> ws, secondWord);

    Word* first = findWordPointer(firstWord, dictionary);
    Word* second = findWordPointer(secondWord, dictionary);

    if (first == NULL) {
        cout << "\nWord not found: " << firstWord << endl;
        return;
    }
    if (second == NULL) {
        cout << "\nWord not found: " << secondWord << endl;
        return;
    }
    if (first == second) {
        cout << "\nThe words are the same: " << firstWord << endl;
        return;
    }

    int removedEdges = 0;

    for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end();) {
        bool isDirectEdge = (it->source == first && it->target == second);
        bool isReverseEdge = (it->source == second && it->target == first);

        if (isDirectEdge || isReverseEdge) {
            it = dictionary.edges.erase(it);
            removedEdges++;
        } else {
            it++;
        }
    }

    if (removedEdges == 0) {
        cout << "\nNo direct edges found between \"" << firstWord << "\" and \"" << secondWord << "\"." << endl;
    } else {
        cout << "\nRemoved edges between \"" << firstWord << "\" and \"" << secondWord << "\": " << removedEdges << endl;
    }
}

void meaning(Dictionary& dictionary)
{
    string userWord;
    cout << "\nEnter the word to search: ";
    getline(cin >> ws, userWord);
    
    Word* foundWord = findWordPointer(userWord, dictionary);
    
    if (foundWord == NULL) {
        cout << "\nWord not found: " << userWord << endl;
    } else {
        cout << "\nWord: " << foundWord->word << endl;
        cout << "Translation: " << foundWord->translation << endl;
    }
}

void synonyms(Dictionary& dictionary)
{
    string userWord;
    cout << "\nEnter the word to find synonyms: ";
    getline(cin >> ws, userWord);
    
    Word* foundWord = findWordPointer(userWord, dictionary);
    
    if (foundWord == NULL) {
        cout << "\nWord not found: " << userWord << endl;
        return;
    }
    
    list<Word*> synonymsList;
    
    for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end(); it++) {
        if (it->similarity > 70.0) {
            if (it->source == foundWord) {
                synonymsList.push_back(it->target);
            }
            else if (it->target == foundWord) {
                synonymsList.push_back(it->source);
            }
        }
    }
    
    if (synonymsList.empty()) {
        cout << "\nNo synonyms found for: " << userWord << endl;
    } else {
        cout << "\nSynonyms for \"" << userWord << "\":" << endl;
        for (list<Word*>::iterator it = synonymsList.begin(); it != synonymsList.end(); it++) {
            cout << "  - " << (*it)->word << " (" << (*it)->translation << ")" << endl;
        }
    }
}

// TODO: Use euclidean distance to calculate similarity between two words
void similarity(Dictionary& dictionary)
{
    string firstWord, secondWord;

    cout << "\n Enter the first word: ";
    getline(cin >> ws, firstWord);
    cout << "Enter the second word: ";
    getline(cin >> ws, secondWord);

    Word* word1 = findWordPointer(firstWord, dictionary);
    Word* word2 = findWordPointer(secondWord, dictionary);

    if (word1 == NULL) {
        cout << "\nWord not found: " << firstWord << endl;
        return;
    }
    if (word2 == NULL) {
        cout << "\nWord not found: " << secondWord << endl;
        return;
    }
    if (word1 == word2) {
        cout << "\nThe words are the same: " << firstWord << endl;
        return;
    }
    
    double similarity = calculateSimilarity(*word1, *word2);
    cout << "\nSimilarity between \"" << firstWord << "\" and \"" << secondWord << "\": " << fixed << setprecision(2) << similarity << endl;
}

void listByAlphabet(Dictionary& dictionary)
{
    cout << "\nFeature in development." << endl;
}

void listBySize(Dictionary& dictionary)
{
    cout << "\nFeature in development." << endl;
}

// Function responsible for displaying the main menu
int menu() {
    int option;

    while (true) {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Insert Word\n";
        cout << "2. Insert Edge\n";
        cout << "3. Remove Word\n";
        cout << "4. Remove Edge\n";
        cout << "5. Meaning\n";
        cout << "6. Synonyms\n";
        cout << "7. Calculate Similarity\n";
        cout << "8. List Words by Alphabetic Order\n";
        cout << "9. List Words by Size\n";
        cout << "0. Leave\n";
        cout << "Pick an option: ";
        
        cin >> option;
        if(cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }
        
        if (option < 0 || option > 9) {
            cout << "Invalid option. Try again.\n";
            return menu();
        }

        return option;
    }
}

// Function to display menu again or end process
bool displayMenuAgain() {
    int option;
    
    while (true) {
        cout << "\nDo you wish to pick another option?\n";
        cout << "1. Yes\n";
        cout << "0. No\n\n";

        cin >> option;
        if(cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }
        
        switch (option) {
            case 0:
                cout << "\nEnding process...\n";
                return false;
            case 1:
                cout << "\nReturning to the menu...\n";
                return true;
            default:
                cout << "Invalid option.\n";
        }
    }           
}

int main()
{   
    // Global dictionary variable that will be used across the different functions of this code
    Dictionary dictionary;
    // Variable that represents the option the user picks from the menu
    int option;
    // Variable that determines if the menu should be displayed
    bool displayMenu = true;
    
    // Main loop
    while (true) {
        if (displayMenu) {
            option = menu();
        }
        else {
            break;
        }
        
        // Logic responsible for calling the different functions based on the user's choice
        switch(option) {
            case 0:
                cout << "\nEnding process...\n";
                return 0;

            case 1:
                insertWords(dictionary);
                break;
            case 2:
                insertEdges(dictionary);
                break;
            case 3:
                removeWords(dictionary);
                break;
            case 4:
                removeEdges(dictionary);
                break;
            case 5:
                meaning(dictionary);
                break;
            case 6:
                synonyms(dictionary);
                break;
            case 7:
                similarity(dictionary);
                break;
            case 8:
                listByAlphabet(dictionary);
                break;
            case 9:
                listBySize(dictionary);
                break;
        }
        
        // Following the execution of a valid option, as if the user would like to pick another on end the process
        displayMenu = displayMenuAgain();
    }

    return 0;
}
