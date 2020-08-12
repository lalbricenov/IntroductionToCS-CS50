#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
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
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]] += 1;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] == preferences[j][i])
            {
                continue;
            }
            else
            {
                pair newPair;
                int winnerId, loserId;
                if (preferences[i][j] > preferences[j][i])
                {
                    // set winnerId to i
                    winnerId = i;
                    loserId = j;
                }
                else
                {
                    winnerId = j;
                    loserId = i;
                }

                newPair.winner = winnerId;
                newPair.loser = loserId;
                pairs[pair_count] = newPair;
                pair_count++;
            }
        }
    }
    return;
}

// function that compares two pairs
int cmpfunc(const void *a, const void *b)
{
    pair pairA = *(pair *)a;
    pair pairB = *(pair *)b;

    // return value < 0 means that a goes before b
    int strOfVicA = preferences[pairA.winner][pairA.loser] - preferences[pairA.loser][pairA.winner];
    int strOfVicB = preferences[pairB.winner][pairB.loser] - preferences[pairB.loser][pairB.winner];

    return strOfVicB - strOfVicA;// This is less than zero if the pair A has a stronger victory
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    qsort(pairs, pair_count, sizeof(pair), cmpfunc);
    return;
}

// Function that tells if a number is in an array
bool inArray(int elem, int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (array[i] == elem)
        {
            return true;
        }
    }
    return false;
}
bool createsCycle(int nPair)
{
    // adding the edge that goes from winner to loser creates a loop
    // if before adding it there was already a path from loser to winner

    int loser = pairs[nPair].loser;
    int winner = pairs[nPair].winner;
    // Path from loser to winner?

    //Start from loser and do a depth-first-search bfs

    // Start with a frontier that contains the initial state
    int frontier[candidate_count];// stores the indices of the candidates in the frontier
    for (int i = 0; i < candidate_count; i++)
    {
        frontier[i] = -1;//Initialize all the elements in the frontier to -1
    }
    int size_frontier = 1;
    frontier[0] = loser;
    // Start with an empty explored set
    int explored[candidate_count];// stores the indices of the candidates in the explored set
    for (int i = 0; i < candidate_count; i++)
    {
        explored[i] = -1;//Initialize all the elements in the explored set to -1
    }
    int size_explored = 0;
    // Repeat
    while (1)
    {
        // if the frontier is empty, then no solution
        if (size_frontier == 0)
        {
            return false;// There is no solution, that means that no loop is created
        }
        // Remove a node from the frontier: the last one
        int removed_node = frontier[size_frontier - 1];
        frontier[size_frontier - 1] = -1;
        size_frontier--;
        // If node contains goal state return the solution
        if (removed_node == winner)
        {
            return true; // There is a solution, that means that a loop will be created
        }
        // Add the node to the explored set
        explored[size_explored] = removed_node;
        size_explored++;
        // Expand node, add resulting nodes to the frontier if they
        // aren't already in the frontier or the explored set
        for (int j = 0; j < candidate_count; j++)
        {
            if (j == removed_node)
            {
                continue;
            }
            if (locked[removed_node][j])// If there is an edge from the removed node to node j
            {
                if (!inArray(j, frontier, size_frontier) && !inArray(j, explored, size_explored))
                {
                    frontier[size_frontier] = j;
                    size_frontier++;
                }
            }
        }


    }
    //

}
// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int nPair = 0; nPair < pair_count; nPair++)
    {
        // If adding this pair doesn't create a cycle, add it.
        if (!createsCycle(nPair))
        {
            int winner = pairs[nPair].winner;
            int loser = pairs[nPair].loser;
            locked[winner][loser] = true;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // Go up in the graph starting from the first node that is locked
    int winner = -1;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[i][j])// i is the winner and j is the loser
            {
                winner = i;
                break;
            }
        }
        if (winner != -1)
        {
            break;
        }
    }

    // Find a node that has the current winner as a loser, that is, locked[i][winner]
    while (1)
    {
        bool newWinnerFound = false;
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[i][winner])
            {
                winner = i;
                newWinnerFound = true;
                break;
            }
        }

        if (!newWinnerFound)
        {
            break;
        }
    }

    printf("%s\n", candidates[winner]);
    return;
}

