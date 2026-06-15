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
#include "extra.h"
#include "graphs.h"
#include "plots.h"

using namespace std;

list<string> parseTranslations(const string& translationsLine)
{
    list<string> translations;
    string translation;
    stringstream ss(translationsLine);

    while (getline(ss, translation, ',')) {
        size_t start = translation.find_first_not_of(" \t");
        size_t end = translation.find_last_not_of(" \t");

        if (start != string::npos) {
            translations.push_back(translation.substr(start, end - start + 1));
        }
    }

    return translations;
}

void insertWordsFile(Dictionary& dictionary)
{
    ifstream file_words("seed/words.txt");

    string lineWords;

    if (file_words.is_open()) {
        int aux = 0;
        Word word;

        while (getline(file_words, lineWords)) {
            if (lineWords.empty()) {
                continue;
            }
            if(aux == 0){
                word = Word();
                word.word = lineWords;
                //cout << endl << "Word: " << word.word << endl;
            }
            if(aux == 1){
                word.translations = parseTranslations(lineWords);
                //cout << "Translations: ";
                //printTranslations(word.translations);
                //cout << endl;
            }
            if(aux == 2){
                stringstream ss(lineWords);
                ss >> word.coordinates.x >> word.coordinates.y >> word.coordinates.z;

                if (ss.fail()) {
                    cerr << "Invalid coordinates for word: " << word.word << endl;
                    aux = 0;
                    continue;
                }

                //cout << "X: " << word.coordinates.x << endl;
                //cout << "Y: " << word.coordinates.y << endl;
                //cout << "Z: " << word.coordinates.z << endl;

                if (findWordPointer(word.word, dictionary) != NULL) {
                    cout << "Word already exists, skipping: " << word.word << endl;
                } else {
                    // Insert the word into the dictionary
                    dictionary.words.push_back(word);
                    Word* insertedWord = &dictionary.words.back();
                    // Insert the word into both trees
                    insert(dictionary.alphabetRoot, insertedWord, 0);
                    insert(dictionary.sizeRoot, insertedWord, 1);
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

void insertEdgesFile(Dictionary& dictionary){
    ifstream file_edges("seed/edges.txt");

    string lineEdges;

    if (file_edges.is_open()) {
        int aux = 0;
        Edge edge;
        while (getline(file_edges, lineEdges)) {
            if (lineEdges.empty()) {
                continue;
            }
            if(aux == 0){
                edge = Edge();
                edge.source = findWordPointer(lineEdges, dictionary);

                if (edge.source == NULL) {
                    cerr << "\nSource word not found: " << lineEdges << endl;
                    continue;
                }
                aux = 1;
            } else {
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

                bool alreadyExists = false;
                for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end(); it++) {
                    if (it->source == edge.source && it->target == edge.target || 
                        it->source == edge.target && it->target == edge.source) {
                        cout << "Edge already exists between \"" << edge.source->word << "\" and \"" << edge.target->word << "\", skipping." << endl;
                        alreadyExists = true;
                        break;
                    }
                }

                if (!alreadyExists) {
                    edge.similarity = calculateSimilarity(*edge.source, *edge.target);
                    dictionary.edges.push_back(edge);
                }

                aux = 0;
            }
        }
        file_edges.close();
    } else {
        cerr << "Unable to open file." << endl;
    }

    cout << "\nEdges inserted successfully." << endl;
}

// FIX: Code ending after inserting the word
void insertWordsTerminal(Dictionary& dictionary)
{
    string userWord;
    string userTranslation;
    double x, y, z;

    cout << "\nEnter the word: ";
    getline(cin >> ws, userWord);
    cout << "Enter the translations (Separated by comma): ";
    getline(cin >> ws, userTranslation);
    cout << "Enter the coordinates (x y z): ";
    cin >> x >> y >> z;
    cin.ignore(1000, '\n');

    if (findWordPointer(userWord, dictionary) != NULL) {
        cout << "\nWord already exists, skipping: " << userWord << endl;
        return;
    }

    Word word;
    word = Word();
    word.word = userWord;
    word.translations = parseTranslations(userTranslation);
    word.coordinates.x = x;
    word.coordinates.y = y;
    word.coordinates.z = z;

    if (findWordPointer(word.word, dictionary) != NULL) {
        cout << "Word already exists, skipping: " << word.word << endl;
    } else {
        // Insert the word into the dictionary
        cout << "\nWord inserted successfully: " << userWord << endl;
        dictionary.words.push_back(word);
        Word* insertedWord = &dictionary.words.back();
        // Insert the word into both trees
        insert(dictionary.alphabetRoot, insertedWord, 0);
        insert(dictionary.sizeRoot, insertedWord, 1);
    }
}

void insertEdgesTerminal(Dictionary& dictionary)
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

    for (list<Edge>::iterator it = dictionary.edges.begin(); it != dictionary.edges.end(); it++) {
        if (it->source == first && it->target == second || 
            it->source == second && it->target == first) {
            cout << "\nEdge already exists between \"" << firstWord << "\" and \"" << secondWord << "\", skipping." << endl;
            return;
        }
    }

    Edge newEdge;
    newEdge.source = first;
    newEdge.target = second;
    newEdge.similarity = calculateSimilarity(*first, *second);

    dictionary.edges.push_back(newEdge);

    cout << "\nEdge inserted successfully between \"" << firstWord << "\" and \"" << secondWord << "\" with similarity: " << fixed << setprecision(2) << newEdge.similarity << endl;
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
            remove(dictionary.alphabetRoot, userWord, 0);
            remove(dictionary.sizeRoot, userWord, 1);
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
        cout << "Translations: ";
        printTranslations(foundWord->translations);
        cout << endl;
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
        if (it->similarity > 30.0) {
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
            cout << "  - " << (*it)->word << " (";
            printTranslations((*it)->translations);
            cout << ")" << endl;
        }
    }
}

void similarity(Dictionary& dictionary)
{
    string firstWord, secondWord;

    cout << "\nEnter the first word: ";
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
    cout << "\nWords by alphabetic order:" << endl;
    if (dictionary.alphabetRoot == NULL) {
        cout << "No words available" << endl;
        return;
    }

    show_in_order(dictionary.alphabetRoot);
}

void listBySize(Dictionary& dictionary)
{
    cout << "\nWords by size:" << endl;
    if (dictionary.sizeRoot == NULL) {
        cout << "No words available" << endl;
        return;
    }

    show_in_order(dictionary.sizeRoot);
}

// Function responsible for displaying the main menu
int menu() {
    int option;

    while (true) {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Insert Words by File\n";
        cout << "2. Insert Edges by File\n";
        cout << "3. Insert Word by Terminal\n";
        cout << "4. Insert Edge by Terminal\n";
        cout << "5. Remove Word\n";
        cout << "6. Remove Edge\n";
        cout << "7. Meaning\n";
        cout << "8. Synonyms\n";
        cout << "9. Calculate Similarity\n";
        cout << "10. List Words by Alphabetic Order\n";
        cout << "11. List Words by Size\n";
        cout << "12. Build Complete Graph\n";
        cout << "13. Convex Hull Perimeter\n";
        cout << "0. Leave\n";
        cout << "Pick an option: ";
        
        cin >> option;
        cin.ignore(1000, '\n');
        if(cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }
        
        if (option < 0 || option > 13) {
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
        cout << "0. No\n";

        cin >> option;
        cin.ignore(1000, '\n');
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
                insertWordsFile(dictionary);
                break;
            case 2:
                insertEdgesFile(dictionary);
                break;
            case 3:
                insertWordsTerminal(dictionary);
                break;
            case 4:
                insertEdgesTerminal(dictionary);
                break;
            case 5:
                removeWords(dictionary);
                break;
            case 6:
                removeEdges(dictionary);
                break;
            case 7:
                meaning(dictionary);
                break;
            case 8:
                synonyms(dictionary);
                break;
            case 9:
                similarity(dictionary);
                break;
            case 10:
                listByAlphabet(dictionary);
                break;
            case 11:
                listBySize(dictionary);
                break;
            case 12:
                buildAndDisplayCompleteGraph(dictionary);
                break;
            case 13: {
                convexHullPerimeter(dictionary);
                break;
            }
        }
        
        // Following the execution of a valid option, as if the user would like to pick another on end the process
        displayMenu = displayMenuAgain();
    }

    // Clean up memory before ending the program
    destruct(dictionary.alphabetRoot);
    destruct(dictionary.sizeRoot);

    return 0;
}
