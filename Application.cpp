#include <cmath>
#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "gamestate.h"

using namespace std;


// Convert a string of 0,1,2 into the pattern code (warning, nothing checked)
int StringToPattern(const string &s)
{
    int res = 0;
    for(int k=0;k<s.size();k++)
    {
        char c = s[k];              // Ascii code
        int a = c - ASCII_0;        // 0,1 or 2
        res += a * pow(3,k);
    }
    return res;
}


// The Unicode colored squares associated to a given pattern
string PatternToStringOfSquares(int pattern, int K)
{
    string res;

    int current = pattern;
    for(int k=0; k<K;k++)
    {
        Color a = Color(current%3);
        res += a==Color::Green ? "\U0001F7E9" : (a==Color::Yellow ? "\U0001F7E8" : "\u2B1B");
        current = current/3;
    }
    return res;
}


// ==================================================================================================================




// ==================================================================================================================

float ComputeEntropy(const GameState &initial_state, const string &word, const vector<string> &possible_solutions)
{    
    float entropy = 0;
    int K = initial_state.GetWordSize();

    // For each pattern we could get, compute expected entropy
    for(int pattern = 0; pattern < pow(3,K); pattern++)
    {
        // If we got that pattern from that word, in which state would we be
        GameState state(initial_state);
        state.Update(word,pattern);

        // In that case, count how many would be compatible as being the ground truth
        int cnt = 0;            
        for(int jw = 0; jw < possible_solutions.size();jw++)
        {
            string candidate_word = possible_solutions[jw];
            cnt += state.isCompatible(candidate_word,true);     // we are checking previously possible solutions, so we look only at last step
        }
        float p = (float) cnt / (float) possible_solutions.size();
        if(p>0) 
        {
            entropy += - p * log(p) / log(2);
        }
    }

    return entropy;        
}


string ComputeBestChoice(GameState initial_state, const vector<string> &words)
{    
    vector<string> candidate_pool = words;

    // Build the list of remaining possible solutions at this stage
    vector<string> possible_solutions;
    for(int iw = 0; iw < words.size(); iw++)
    {        
        string word = words[iw]; 
        if(initial_state.isCompatible(word,false))
        {
            possible_solutions.push_back(word);
        }
    }
    
    // If only one, we are done
    if(possible_solutions.size() == 1) return possible_solutions[0];

    // If less than 10 : display
    cout << "Number of possible solutions " << possible_solutions.size() << " :";
    if(possible_solutions.size() < 10)
    {        
        for(int iw=0;iw<possible_solutions.size();iw++)
        {
            cout << possible_solutions[iw] << ",";
        }
    }
    cout << endl;

    // If less than 3, we limit our choice to the possible solutions, so we try to "shoot to kill"
    if(possible_solutions.size() < 4)
    {        
        candidate_pool = possible_solutions;
    }

    // Now find the word with the maximum entropy    
    string best_choice;
    float best_entropy = -1;
    for(int iw = 0; iw < candidate_pool.size(); iw++)
    {   
        // if(iw%100==0) cout << "N°"<<iw<<endl;     // verbose
        string word = candidate_pool[iw];        

        float entropy = ComputeEntropy(initial_state, word, possible_solutions);
        if(entropy > best_entropy)
        {
            best_entropy = entropy;
            best_choice = word;    
            cout << "New best option (n°" << iw << ") : " << best_choice << " : " << best_entropy << " bits" << endl;        
        }  
        
    }    
    return best_choice;
}




// ==================================================================================================================

// Simple tests
// ============

void PrintTest(const string &truth, const string &word)
{
    cout << "(" << truth << ")" << " " << word << " " << PatternToStringOfSquares(ComputePattern(word,truth),word.size()) << endl;
}

void BasicRuleTest()
{
    PrintTest("ABCDE","AXXXX");     // 🟩⬛⬛⬛⬛ one good
    PrintTest("ABCDE","XAXXX");     // ⬛🟨⬛⬛⬛ one misplaced
    PrintTest("ABCDE","AEXXX");     // 🟩🟨⬛⬛⬛ one good one misplaced

    PrintTest("ABCDE","AAXXX");     // 🟩⬛⬛⬛⬛ one good once
    PrintTest("ABCDE","XAAXX");     // ⬛🟨⬛⬛⬛ double misplacement of the same letter

    PrintTest("AABCD","AXAXX");     // 🟩⬛🟨⬛⬛ same letter twice one good, a second copy misplaced
    PrintTest("AABCD","AAXXX");     // 🟩🟩⬛⬛⬛
    PrintTest("AABCD","AAXXA");     // 🟩🟩⬛⬛⬛
    PrintTest("AAACD","AAXXA");     // 🟩🟩⬛⬛🟨 there is a third copy somewhere
}


// ==================================================================================================================

// Load words with a given length. File are assumed to be like "data/mots_5.txt"
vector<string> LoadWords(int K, int N)
{
    // Read file dictionnary of words
    vector<string> words;
    const string filename = "data/mots_" + to_string(K) + ".txt";
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        // A bit extreme, but if we are here the program won't work and will crash soon emough.
        abort();
    }

    string line;
    
    while(getline(file,line) && words.size() < N)
    {
        // Convert to CAPS : after we assume there is nothing elese than A-Z
        for_each(line.begin(), line.end(), [](char & c)
        {
            c = ::toupper(c);
        });

        if(find(words.begin(), words.end(), line) == words.end() )
        {
            words.push_back(line);            
        }            
    }
    return words;
}


// Load words matching a certain mask. 
// E.g if mask = "F......" : length=7 and starts with F.
vector<string> LoadWordsWithMask(int N, const string &mask)
{
    int K = mask.size();
    vector<string> words = LoadWords(K,N);
    vector<string> res;
    int cnt = 0;
    for(int iw = 0; iw < words.size(); iw++)
    {
        string word = words[iw];
        bool word_ok = true;
        for(int k=0;k<K;k++)
        {
            char c_mask = mask[k];
            if(c_mask >= ASCII_A && c_mask < ASCII_A + 26)  // if the mask specifies a letter, check the word satisfies it
            {
                char c_word = word[k];
                if(c_word != c_mask)
                {
                    word_ok =false;
                    break;
                }
            }
        }
        if(word_ok) 
        {
            res.push_back(word);
            //cout << cnt << " " << word << endl;
            cnt++;
        }
    }
    return res;
}


// =================================================================================================

// Automatically plays a game with a given solution "ground_truth" and a given initial_mask

int AutomaticPlay(const vector<string> & words, const string &ground_truth, const string &initial_mask)
{    
    cout << "\n*** NEW GAME Truth=" << ground_truth << endl;
    
    int K = words[0].size();
    GameState state(K,initial_mask);
    int nb_compat = state.NbOfCompatibleWords(words);
    cout << "Nb of compatible words : " << nb_compat << " Entropy=" << log(nb_compat)/log(2);

    const int MAX_STEPS = 6;
    for(int s = 0; s < MAX_STEPS; s++)
    {        
        string proposal;

        // If first steps Use known best words for opening
        if(s==0)
        {
            if(initial_mask == ".....") proposal = "TARIE";
            if(initial_mask == "......") proposal = "SORTIE";
        }

        if(proposal.empty()) proposal = ComputeBestChoice(state, words);
        cout << '\n' << proposal;

        int pattern = ComputePattern(proposal, ground_truth);
        cout << " " << PatternToStringOfSquares(pattern,state.GetWordSize()) << " ";

        if(proposal == ground_truth) 
        {
            cout << "SOLVED IN " << (s+1) << " STEPS" << endl;
            return s+1;            
        }
        
        float old_entropy = log(state.NbOfCompatibleWords(words))/log(2);
        state.Update(proposal, pattern);
        float new_entropy = log(state.NbOfCompatibleWords(words))/log(2);

        cout << "Entropy gain = " << (old_entropy-new_entropy);
        cout << " Nb of compatible words : " << state.NbOfCompatibleWords(words) << " New entropy=" << log(state.NbOfCompatibleWords(words))/log(2) << " ";            
    }
    return MAX_STEPS;
}


int AutoWordle(const string &ground_truth)
{    
    int K = ground_truth.size();
    
    vector<string> words = LoadWords(K,4096);

    string initial_mask;
    for(int k=0;k<K;k++) initial_mask += '.';

    int score = AutomaticPlay(words, ground_truth, initial_mask);

    return score;
}

// Automatically plays a "SUTOM" like game, with first letter given
int AutoSutom(const string &ground_truth)
{    
    int K = ground_truth.size();
    string initial_mask;
    initial_mask += ground_truth[0];        // First letter
    for(int k=1;k<K;k++) initial_mask += '.';
    vector<string> words = LoadWordsWithMask(100000,initial_mask);

    int score = AutomaticPlay(words, ground_truth, initial_mask);

    return score;
}


// =================================================================================================

void FindBestOpening(int K)
{
    int MAX_NUMBER_OF_WORDS = 4096;
    vector<string> words = LoadWords(K, MAX_NUMBER_OF_WORDS);
    GameState initial_state(K);
    ComputeBestChoice(initial_state,words);
}


// A series of random tests to compute average performance
void ComputeAveragePerformance(int K, int NB_TESTS)
{
    int MAX_NUMBER_OF_WORDS = 4096;
    vector<string> words = LoadWords(K, MAX_NUMBER_OF_WORDS);
    string initial_mask;
    for(int k=0;k<K;k++) initial_mask +='.';

    std::random_device rd;                          // obtain a random number from hardware
    std::mt19937 gen(rd());                         // seed the generator
    std::uniform_int_distribution<> distr(0, 1000); // define the range

    float avg = 0;
    for(int i = 0; i < NB_TESTS; i++)
    {  
        int n = distr(gen);
        string truth = words[n];                        // choose a word
        int s = AutomaticPlay(words, truth, initial_mask);            // get performance
        avg = ((avg * (float) i + s))/((float) i+1);        // update average
        i++;
        cout << "*** CURRENT AVERAGE = " << avg << " (" << i << " tests)\n" << endl;
    }
}

// A series of random tests to compute average performance
void ComputeAverageSutomPerformance(int K, int NB_TESTS)
{
    std::random_device rd;                          // obtain a random number from hardware
    std::mt19937 gen(rd());                         // seed the generator
    std::uniform_int_distribution<> distr(0, 1000); // define the range

    int MAX_NUMBER_OF_WORDS = 4096;
    
    float avg = 0;
    for(int i = 0; i < NB_TESTS; i++)
    {  
        vector<string> words = LoadWords(K, MAX_NUMBER_OF_WORDS);
        int n = distr(gen);
        
        string truth = words[n];  // choose a word

        string initial_mask;
        initial_mask += truth[0];
        for(int k=1;k<K;k++) initial_mask += '.';
        
        int s = AutoSutom(truth);            // get performance

        avg = ((avg * (float) i + s))/((float) i+1);        // update average
        i++;
        cout << "*** CURRENT AVERAGE = " << avg << " (" << i << " tests)" << endl << endl;
    }
}



// Play an interactive game : enter what you get and the algo will suggest next word
void RealInteractiveGame()
{
    string initial_mask;
    cout << "Enter initial mask:";
    cin >> initial_mask;

    int K = initial_mask.size();

    vector<string> words = LoadWordsWithMask(100000,initial_mask);

    GameState state(K,initial_mask);

    for(int s = 0; s < 6; s++)
    {      
        string proposal;
        
        // If first steps Use known best words for opening
        if(s==0)
        {
            if(initial_mask == ".....") proposal = "TARIE";
            if(initial_mask == "......") proposal = "SORTIE";
        }
        
        if(proposal.empty()) proposal = ComputeBestChoice(state, words);
        
        cout << "Suggestion : " << proposal << '\n';
        cout << "Choix :";
        string choice;
        cin >> choice;
        cout << "Resultat obtenu :";            // expect a string like 21002 for green/yellow/gray/gray/green
        string result;
        cin >> result;               
        state.Update(choice, StringToPattern(result));        
    }
}


// =================================================================================================

int main()
{
    // Below a few things you can do

    BasicRuleTest();

    auto clock = chrono::steady_clock();
    const auto start = clock.now();

    AutoWordle("REPAS");
    AutoWordle("SAPIN");

    const auto end = clock.now();
    chrono::nanoseconds dt = end - start;
    cout << "Time spent: " << double(dt.count()) / 1'000'000 << "ms" << endl;

    AutoSutom("DIAMETRE");

    FindBestOpening(5);  

    ComputeAveragePerformance(5,10);

    ComputeAverageSutomPerformance(7,10);

    RealInteractiveGame();
    
    return 0;
}
