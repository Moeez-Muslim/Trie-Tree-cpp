#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <windows.h>

using namespace std;

struct TrieTreeNode
{
    char data;
    bool isEndOfWord;
    string meaning;
    TrieTreeNode* children[26];

    TrieTreeNode(char ch) : data(ch), isEndOfWord(false), meaning("")
    {
        for (int i = 0; i < 26; i++)
            children[i] = NULL;
    }
};

class TrieTree
{
private:
    TrieTreeNode* root;

public:
    TrieTree()
    {
        root = new TrieTreeNode('\0');  // Create an empty root
    }

    bool insertWord(const string& word, const string& meaning)  // Passing by reference to avoid making a copy, which improves time complexity
    {                                                           // argument is constant to avoid modification in the original input string
        TrieTreeNode* current = root;

        if (!(isValidWordOrMeaning(word) && isValidWordOrMeaning(meaning)))
        {
            cout << "Invalid word or meaning\n";
            return false;
        }

        for (char ch : word)  // Iterates through all characters of the string
        {
            ch = tolower(ch);  // Convert to lower case if not already

            int index = ch - 'a';  // a:[0],...,z[25]

            if (!current->children[index])   // Leaf Node
                current->children[index] = new TrieTreeNode(ch);

            current = current->children[index];
        }

        // Currently at the newly placed leaf node. Only leaf nodes will store meanings unless a word is part of another word

        current->isEndOfWord = true;
        current->meaning = meaning;
        
        return true;
    }

    string searchWord(const string& word)
    {
        TrieTreeNode* current = root;

        if (! isValidWordOrMeaning(word))
        {
            return "\0";
        }

        for (char ch : word)
        {
            ch = tolower(ch);

            int index = ch - 'a';

            if (!current->children[index])
                return ""; // Word not found

            current = current->children[index];
        }

        return current->meaning; // Return the meaning of the word if found
    }

    bool updateWord(const string& word, const string& newMeaning)
    {
        TrieTreeNode* current = root;

        if (!(isValidWordOrMeaning(word) && isValidWordOrMeaning(newMeaning)))
        {
            cout << "Invalid word or meaning\n";
            return false;
        }

        for (char ch : word)
        {
            ch = tolower(ch);

            int index = ch - 'a';

            if (!current->children[index])
                return false; // Word not found, cannot update

            current = current->children[index];
        }

        if (current->isEndOfWord)
        {
            current->meaning = newMeaning;
            return true; // Word found and updated successfully
        }

        return false; // Word not found, cannot update
    }

    void deleteWord(const string& word)
    {
        if (isValidWordOrMeaning(word))
        {
            if (deleteWordHelper(root, word, 0))
            {
                cout << "Word deleted successfully\n";
            }
            else
            {
                cout << "Word deleted successfully\n";
            }
        }
        else
        {
            cout << "Invalid Word\n";
            return;
        }
        
    }

    vector<string> getSuggestions(const string& prefix)
    {
        TrieTreeNode* current = root;

        for (char ch : prefix)
        {
            ch = tolower(ch);

            int index = ch - 'a';

            if (!current->children[index])
                return {}; // No suggestions if the prefix is not in the trie

            current = current->children[index];
        }

        vector<string> suggestions;
        findSuggestions(current, prefix, suggestions);

        // Sort the suggestions alphabetically using 'Algorithm' Library
        sort(suggestions.begin(), suggestions.end());

        // Take the top 10 suggestions (if available)
        int numSuggestions = min(static_cast<int>(suggestions.size()), (10));
        suggestions.resize(numSuggestions);

        return suggestions;
    }

    void readDictionaryFromFile()
    {
        const string filename = "Dictionary.txt";

        ifstream file(filename);

        if (!file.is_open())
        {
            cout << "Error: Unable to open file '" << filename << endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            istringstream iss(line);   // String Stream helps to break a string in parts
            string word, meaning;

            // Split the line into word and meaning
            if (iss >> word >> meaning)
            {
                // Check if word and meaning are valid (contain only [a-z] or [A-Z])
                if (isValidWordOrMeaning(word) && isValidWordOrMeaning(meaning))
                {
                    insertWord(word, meaning);
                }
            }
        }

        file.close();
    }

private:
    bool hasChildren(const TrieTreeNode* node)
    {
        for (int i = 0; i < 26; ++i)
        {
            if (node->children[i])
                return true;
        }
        return false;
    }

    void findSuggestions(TrieTreeNode* node, const std::string& currentPrefix, std::vector<std::string>& suggestions)
    {
        if (node->isEndOfWord)
            suggestions.push_back(currentPrefix);

        for (int i = 0; i < 26; ++i)
        {
            if (node->children[i])
            {
                char nextChar = 'a' + i;
                findSuggestions(node->children[i], currentPrefix + nextChar, suggestions);
            }
        }
    }

    bool deleteWordHelper(TrieTreeNode*& node, const std::string& word, int depth)
    {
        if (!node)  // Check if tree is empty
            return false;

        if (depth == word.length())  // It is possible that one word is part of another so we need to keep this check of using length
        {
            if (node->isEndOfWord)
            {
                node->isEndOfWord = false;  // This path is not recognized as word anymore
                node->meaning = "";
                return true;
            }
            return false;
        }

        // Now if word is not part of another word then we need to delete this path uptil where possible
        int index = word[depth] - 'a';
        if (deleteWordHelper(node->children[index], word, depth + 1))
        {
            delete node->children[index];
            node->children[index] = NULL;
            return node->isEndOfWord || !hasChildren(node);
        }

        return false;
    }

    bool isValidWordOrMeaning(const std::string& str)
    {
        for (char ch : str)
        {
            if (!isalpha(ch))
                return false;
        }
        return true;
    }
};

void displayMenu()
{
    system("cls"); // Clear the console screen
    cout << "Main Menu\n";
    cout << "1. Add a word with its meaning\n";
    cout << "2. Delete Word\n";
    cout << "3. Search a word\n";
    cout << "4. Update Word\n";
    cout << "\nPress 'Esc' to exit\n";
}

void displaySuggestions(const vector<string>& suggestions)
{
    cout << "\n\nSuggestions:\n";
    for (const string& suggestion : suggestions)
    {
        cout << suggestion << endl;
    }
}


int main()
{
    TrieTree trie;

    cout << "Loading dictionary. Please wait...";
    trie.readDictionaryFromFile();
    system("cls");

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD NumEvents = 0;
    INPUT_RECORD InRec;

    string word, meaning;
    string userInput;
    string searchResult;
    vector<string> suggestions;


    char choice = '\0';

    while (true)
    {
        displayMenu();

        ReadConsoleInput(hInput, &InRec, 1, &NumEvents);   // Detects input event

        if (InRec.EventType == KEY_EVENT && InRec.Event.KeyEvent.bKeyDown)  // Checks if input event is key press
        {
            // Check for 'Esc' to quit
            if (InRec.Event.KeyEvent.uChar.AsciiChar == 27)
            {
                break;
            }

            // Update choice based on the pressed key
            choice = InRec.Event.KeyEvent.uChar.AsciiChar;

            
            switch (choice)
            {
            case '1':  // Add a word in dictionary
                cout << "Enter a word to add: ";
                cin >> word;
                cout << "Enter its meaning: ";
                cin >> meaning;
                if (trie.insertWord(word, meaning))
                {
                    cout << "Word added successfully!\n\n";
                }

                break;

            case '2':  // Add a word in dictionary
                cout << "Enter a word to delete: ";
                cin >> word;
                trie.deleteWord(word);

                break;

            case '3':

                //cout << "Press any key to start\n";
                while (true)
                {
                    ReadConsoleInput(hInput, &InRec, 1, &NumEvents);

                    system("cls");
                    cout << "\t\t\tSearch a word\n\n";
                    cout << "Enter word: " << userInput;
                    displaySuggestions(suggestions);

                    if (InRec.EventType == KEY_EVENT && InRec.Event.KeyEvent.bKeyDown)
                    {
                        // Check for 'Enter' to quit
                        if (InRec.Event.KeyEvent.uChar.AsciiChar == 13)
                        {
                            break;
                        }

                        char pressedKey = InRec.Event.KeyEvent.uChar.AsciiChar;

                        // Append the pressed key to the input string
                        userInput += pressedKey;

                        // Get suggestions based on the current input string
                        suggestions = trie.getSuggestions(userInput);

                    }
                }

                system("cls");
                cout << "\t\t\tSearch a word\n\n";
                cout << "Enter word: " << userInput  << endl;

                searchResult = trie.searchWord(userInput);
                if (searchResult == "")
                {
                    cout << "word not found\n\n";
                }
                else if (searchResult == "\0")
                {
                    cout << "Invalid word\n\n";
                }
                else
                {
                    cout << "Meaning: " << searchResult << endl << endl;
                }

                userInput = "";
                searchResult = "";
                suggestions.clear();

                break;

            case '4':
                cout << "Enter a word to update: ";
                cin >> word;
                cout << "Enter its meaning: ";
                cin >> meaning;
                if (trie.updateWord(word, meaning))
                    cout << "Word updated successfully!\n";
                else
                    cout << "Word not found";

                break;

            default:
                cout << "Invalid choice\n";
                break;
            }

            // Wait for a key press before going back to the menu
            system("pause");
        }
    }

    return 0;
}
