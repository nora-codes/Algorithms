#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
int check_loop(int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Look for candidate called name
    for (int j = 0; j < candidate_count; j++)
    {

        // If candidate is found - update ranks array
        if (strcmp(name, candidates[j]) == 0)
        {
            ranks[rank] = j;
            return true;
        }
    }

    // If no candidate found
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Update preferences array - using ranks array
    // Based on the current voter's ranks
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = (i + 1); j < candidate_count; j++)
        {
            int winner = ranks[i];
            int loser = ranks[j];
            preferences[winner][loser]++;
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // Define pair count
    pair_count = 0;

    // Add each pair of candidates to the pairs array
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = (i + 1); j < candidate_count; j++)
        {
            // If one candidate is preferred to the other
            // Update pair_count as total number of pairs

            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }

            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }

            // Don't add tie to array
        }
    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Sort pairs in order of decreasing strength of victory
    // Sorting type used: bubble sort

    // Define swap counter
    int swap_counter = 1;

    // Define temporary array for sorting data
    pair temp_pairs[MAX * (MAX - 1) / 2];

    while (swap_counter > 0)
    {
        // Reset counter
        swap_counter = 0;

        // Search through pairs array
        for (int i = 0; i < pair_count; i++)
        {
            // Check selected pair
            // If pair is not sorted
            if (preferences[pairs[i].winner][pairs[i].loser] < preferences[pairs[i + 1].winner][pairs[i + 1].loser])
            {
                // Swap and enter into temporary array
                temp_pairs[i] = pairs[i + 1];
                temp_pairs[i + 1] = pairs[i];

                // Move sorted data from temporary array to original array
                pairs[i] = temp_pairs[i];
                pairs[i + 1] = temp_pairs[i + 1];

                // Update swap counter
                swap_counter++;
            }
        }
    }

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Update locked array to create locked graph

    // Enter first (highest victory) pair into locked array
    locked[pairs[0].winner][pairs[0].loser] = true;

    // For all other pairs in decreasing order through sorted array
    // Check for a loop with check_loop function
    for (int i = 1; i < pair_count; i++)
    {
        if (check_loop(pairs[i].loser) != pairs[i].winner)
        {
            // If there is no loop - enter pair into locked array
            // In other words - add in an edge to the graph
            locked[pairs[i].winner][pairs[i].loser] = true;
        }

        // If there is a cycle - don't enter pair into locked array
        // In other words - don't add the last edge to the graph
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{
    // Print out winner of the election
    // In other words - print out the source of the graph

    // Cycle through locked array
    for (int j = 0; j < candidate_count; j++)
    {
        // Reset source counter to check for locked losses
        int source_counter = 0;

        for (int i = 0; i < candidate_count; i++)
        {
            // Search for locked pairs
            if (locked[i][j] == true)
            {
                // If locked pair is found - update the counter
                source_counter++;
            }
        }

        // Find candidate with no locked pairs in their losing row of the locked array
        // This candidate is the source of the graph - therefore the winner
        // Declare and print winner
        if (source_counter == 0)
        {
            printf("%s\n", candidates[j]);
        }
    }

    return;
}

int check_loop(int loser)
{
    // Iterate through sorted pairs
    for (int i = 0; i < pair_count ; i++)
    {
        // If pair is locked
        if (locked[pairs[i].winner][pairs[i].loser] == true)
        {
            // If winner of pair is equal to input loser
            if (pairs[i].winner == loser)
            {
                // Chain is found - check for next link in chain
                return (check_loop(pairs[i].loser));
            }
        }
    }

    // Return loser to check for loop
    return loser;
}
