#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");

    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{

// 1.   Look for candidate called name

    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i].name, name) == 0)

// 2.   If found - update preferences
//      Return true

        {
            preferences[voter][rank] = i;
            return true;
        }
    }

// 3.   If not found - don't update preferences
//      Return false

    return false;
}

// Tabulate votes for non-eliminated candidates
void tabulate(void)
{

// 1.   Update votes from all non eliminated candidates

// 2.   Recall: each voter votes for their highest preference candidate
//      who has not been eliminated

    // check through voters' preferences
    for (int i = 0; i < voter_count; i++)
    {
        // check first preference first
        int rank = 0;


        for (int j = 0; j < candidate_count; j++)
        {
            // find candidate
            if (preferences[i][rank] == j)
            {
                // if candidate is not eliminated
                if (candidates[j].eliminated == false)
                {
                    // update vote total
                    candidates[j].votes++;
                }

                // if candidate is eliminated
                else
                {
                    // check the next rank
                    rank++;

                    // start from beginning of candidate list
                    j = 0;
                }
            }
        }
    }

    return;
}

// Print the winner of the election, if there is one
bool print_winner(void)
{

// 1.   If any candidate has more than half the vote
//      Print their name
//      Return true

    // define minimum votes to win
    int winning_vote = (voter_count / 2);

    // check for candidate with winning vote
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes > winning_vote)
        {
            printf("%s\n", candidates[i].name);
            return true;
        }
    }

// 2.   If nobody won yet
//      Return false

    return false;
}

// Return the minimum number of votes any remaining candidate has
int find_min(void)
{

// 1.   Return the minimum number of votes
//      of anyone in the election

    // define losing vote as maximum to start
    int losing_vote = voter_count;

    // find losing candidate
    for (int i = 0; i < candidate_count; i++)
    {
        if ((candidates[i].votes < losing_vote) && (candidates[i].eliminated == false))
        {
            losing_vote = candidates[i].votes;
        }
    }

    return losing_vote;
}

// Return true if the election is tied between all candidates, false otherwise
bool is_tie(int min)
{

// 1.   Accept minimum number of votes 'min' as input

    int losing_vote = min;

// 2.   If election is tied between all remaining candidates
//      Return true

    int tie_counter = 0;

    //check if any votes are not equal to the losing vote
    for (int i = 0; i < candidate_count; i++)
    {
        if ((candidates[i].votes != losing_vote) && (candidates[i].eliminated == false))
        {
            tie_counter++;
        }
    }

    // check if all votes are the same
    if (tie_counter == 0)
    {
        return true;
    }

//      Otherwise - return false

    return false;
}

// Eliminate the candidate (or candidates) in last place
void eliminate(int min)
{

// 1.   Eliminate anyone still in the race
//      who has the minimum number of votes

    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == min)
        {
            candidates[i].eliminated = true;
        }
    }

    return;
}
