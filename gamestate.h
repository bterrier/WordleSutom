#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <array>
#include <string>
#include <vector>

using std::string;
using std::vector;


static constexpr char ASCII_0 = 48;
static constexpr char ASCII_A = 65;

// constexpr function to compute powers of 3
static constexpr int pow3(int k)
{
    int result = 1;

    for (int i = 0 ; i < k ; ++i)
    {
        result *= 3;
    }

    return result;
}

// Fast version of pow() for powers of 3
constexpr int pow(int x, int y)
{
    if (x != 3 || y > 12)
    {
        int result = 1;

        for (int i = 0 ; i < y ; ++i)
        {
            result *= x;
        }

        return result;
    }
    else
    {
        constexpr std::array<int, 13> cache {
            pow3(0),
            pow3(1),
            pow3(2),
            pow3(3),
            pow3(4),
            pow3(5),
            pow3(6),
            pow3(7),
            pow3(8),
            pow3(9),
            pow3(10),
            pow3(11),
            pow3(12),
        };

        return cache[y];
    }
}


enum class Color
{
    Gray = 0,
    Yellow = 1,
    Green = 2
};


// Produces the pattern for a given tentative, assuming the truth
// Pattern is coded as an integer equal to sum_k a_k 3^k  where a_k=0,1,2 are the color obtained (0:gray,1:yellow,2:green)
// So this the number represents its "base 3" coding
int ComputePattern(const string &tentative, string truth)
{
    vector<Color> result(tentative.size(), Color::Gray);

    for(int k = 0; k < tentative.size(); k++)
    {
        if(tentative[k]==truth[k])
        {
            result[k] = Color::Green;
            truth[k] = '-';
        }
    }

    for(int k = 0; k < tentative.size(); k++)
    {
        if (result[k] != Color::Gray) continue;

        bool fnd = false;
        for(int k2 = 0; k2 < tentative.size(); k2++)
        {
            // If found elsewhere and that elsewhere is not already green
            if(tentative[k] == truth[k2])
            {
                fnd = true;
                truth[k2] = '-';
                break;
            }
        }
        if(fnd) result[k] = Color::Yellow;    // Yellow coded by 1

    }

    int res = 0;

    for(int k = 0; k < tentative.size(); k++)
    {
        res += int(result[k]) * pow(3,k);
    }

    return res;
}

/* A class to contain the current state of the game : words that have been played, and patterns obtained */
class GameState
{
    private:
        int K;
        struct Step
        {
            Step(const string &word, int p) :
                played_word(word),
                pattern(p)
            {}
            string played_word;
            int pattern;
        };
        vector<Step> steps;
        vector<int> green_mask;             // Redundant information for speed : the list of green letters (-1 if not known, 0 for A, 1 for B etc)

    public:
        // Base constructor
        explicit GameState(int K_)
        {
            K=K_;
            for(int k=0;k<K;k++) green_mask.push_back(-1);
        }

        // Constructor with mask
        GameState(int K_, const string &mask)
        {
            K=K_;
            for(int k=0;k<K;k++)
            {
                char c_mask = mask[k];
                if(c_mask >= ASCII_A && c_mask < ASCII_A + 26)  // if the mask specifies a letter
                {
                    green_mask.push_back(c_mask-ASCII_A);
                }
                else green_mask.push_back(-1);
            }
        }

        // Copy constructor
        GameState(const GameState &that) = default;


        int GetWordSize() const {return K;}


        // Update the state by giving a word and its associated obtained pattern (we don't check size, warning)
        void Update(const string &word, int pattern)
        {
            steps.emplace_back(word, pattern);

            // Decode pattern to register green letters
            int current = pattern;
            for(int k=0;k<K;k++)
            {
                int a = current%3;
                if(a == int(Color::Green))    // if the letter was good and well placed
                {
                    char c = word[k];
                    green_mask[k] = (c-ASCII_A);   // 0 for letter A
                }
                current = current/3;
            }
        }


        // Whether a word is compatible with current state of the game
        // Compatible : it could be the solution ie, it could have produced that sequence.

        bool isCompatible (const string &candidate_truth, bool check_only_last_step) const
        {
            // First check the green mask to save time
            for(int k=0; k<K; k++)
            {
                if(green_mask[k] != -1)
                {
                    char c = candidate_truth[k];
                    if(c-ASCII_A != green_mask[k]) return false;
                }

            }

            // Then check each of the previous steps of the game, to see whether that candidate truth word could have produced that series of patterns
            // We check in reverse assuming the later patterns carry more constraints (with option to check only that one if we know other are satisied)
            for(int i = steps.size() -1 ; i >=0 ; i--)
            {
                const string &word = steps[i].played_word;
                int pattern = steps[i].pattern;
                if(ComputePattern(word,candidate_truth) != pattern) return false;   // not compatible == That candidate_truth would not have produced that observed pattern

                if(i==steps.size() -1 && check_only_last_step) return true;
            }
            return true;
        }


        int NbOfCompatibleWords(const vector<string> &words)
        {
            int cnt = 0;
            for(int iw = 0; iw < words.size(); iw++)
            {
                string word = words[iw];
                if(isCompatible(word,false))
                {
                    cnt++;
                }
            }
            return cnt;
        }

};

#endif // GAMESTATE_H
