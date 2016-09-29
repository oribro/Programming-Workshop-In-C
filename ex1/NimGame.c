
/**
 * @file NimGame.c
 * @author  Orib
 * @date 26 Jul 2015
 *
 * @brief This program plays the Nim game.
 *
 * @section DESCRIPTION
 * Input  : Number of balls to put in each of 2 boxes taken from the user.
 * Process: Take turns between 2 players, in each turn receive input from the user
 * to choose a box and amount of balls to choose from that box, and remove the balls
 * from the box.
 * Output : The declaration of the winner of this game, determined by the next player
 * to take balls after a box has been emptied.
 */

// ------------------------------ includes ------------------------------

// Enables us to work with input and output.
#include <stdio.h>
// Enables us to work with boolean variables.
#include <stdbool.h>

// -------------------------- const definitions -------------------------
/**
 * @def NUMBER_OF_BOXES 2
 * @brief The number of boxes in the game.
 */
#define NUMBER_OF_BOXES 2

/**
 * @def PLAYER_ONE 1
 * @brief A macro for the first player.
 */

#define PLAYER_ONE 1

/**
 * @def PLAYER_TWO 2
 * @brief A macro for the second player.
 */

#define PLAYER_TWO 2

/**
 * @def BOX_ONE 1
 * @brief A macro for the first box.
 */

#define BOX_ONE 1

/**
 * @def BOX_TWO 2
 * @brief A macro for the second box.
 */

#define BOX_TWO 2

/**
 * @def MAX_BALL_WITHDRAWAL 3
 * @brief A macro for the maximal amount of balls to take in a turn.
 */

#define MAX_BALL_WITHDRAWAL 3


// ------------------------------ functions -----------------------------


// ----------- Function declarations

/**
 * @brief This function starts the game by initializing the number
 * of balls in the boxes according to user input. If the initialization
 * could not be performed, we use the stdbool library to return false to
 * the main so that it will identify the problem and exit the program.
 * @param numOfBallsArr the array that holds the number of balls in
 * the boxes to be initialized.
 * @return true if the initialization has been performed successfully,
 *          false if there was a problem in the initialization, that is
 *          the entered number of balls is negative.
 */
bool initializeGame(int numOfBallsArr[]);
/**
 * @brief This function prints to the output the current status of
 * the boxes - the number of balls in each of them.
 * @param numOfBallsArr the array that holds the number of balls in
 * the boxes to be presented.
 */
void presentBoxes(int numberOfBallsArr[]);

/**
 * @brief This function is used by the presentBoxes function to ease printing.
 * It prints a line seperator.
 */
void seperatingLine();

/**
 * @brief This function plays a turn for the given player number.
 * It will ask from the player the box to choose and the number of balls
 * to take from the box and update the box status.
 * @param numOfBallsArr the array that holds the number of balls in
 * the boxes.
 * @param playerNumber the number of the player to take a turn.
 */
void makeTurn(int numOfBallsArr[], int playerNumber);

/**
 * @brief This function asks a box number from the given player until a legal number
 * is chosen. only 1 or 2 are legal box numbers.
 * @param playerNumber the number of the player to choose a box.
 * @return the number of the box the player chose.
 */
int chooseBox(int playerNumber);

/**
 * @brief This function asks the given player for the number of balls
 * to withdraw from the given box until a legal number is chosen.
 * Illegal numbers are negative numbers or numbers greater than 3 or
 * numbers that are greater from the current number of balls in the box.
 * @param numOfBallsArr the array that holds the number of balls in
 * the boxes.
 * @param playerNumber the number of the player to take balls.
 * @param boxNumber the number of the box to take balls from.
 * @return the number of the balls to take from the box.
 */
int takeBalls(int numOfBallsArr[], int playerNumber, int boxNumber);

/**
 * @brief This function plays the game by taking turns between the two players.
 * Once a box has been emptied, the next player to take a turn is the winner which
 * will be determined by this function.
 * @param the array that holds the number of balls in
 * the boxes.
 * @return the number of the player that won the game.
 */
int playGame(int numOfBallsArr[]);

// ----------- Function implementations.

/**
 * @brief The main function. Starts a game by adding balls to the boxes,
 * the boxes are represented in an array of size 2: the first cell holds
 * the balls in box 1 while the second cellholds the balls in box 2.
 * Plays the game by taking turns between two players, each turn remove
 * balls from some box. When one of the boxes is empty, finish the game
 * and declare the winner.
 * @return 0, to tell the system the execution ended without errors.
 */
int main()

{
    // An array to store the number of balls for the boxes.
    // The first cell holds the balls in box 1 while the second cell holds the
    // balls in box 2.
    int numOfBallsArr[NUMBER_OF_BOXES];
    // The winner of the game: the player that takes a turn after one of the boxes
    // became empty.
    int winner;
    // Initialize the game. Exit the program in case of illegal input (neg number).
    if (initializeGame(numOfBallsArr) == false)
    {
        return 0;
    }
    // Play the game and determine the winner.
    winner = playGame(numOfBallsArr);
    // Present the state of the boxes after one of them became empty.
    presentBoxes(numOfBallsArr);
    printf("Player %d wins the game.\n", winner);
    return 0;

}

//         See function declaration section above for function documentation

bool initializeGame(int numOfBallsArr[])
{
	int i;
    // Go over the boxes.
    for (i = 0; i < NUMBER_OF_BOXES; i++)
    {
        // Ask for number of balls for the cur box.
        printf("How many balls in box %d?\n", i + 1);
        scanf("%d", &numOfBallsArr[i]);
        // The number is negative and therefor an illegal input, the program will exit.
        if (numOfBallsArr[i] <= 0)
        {
            printf("Number of balls in box must be positive.\n");
            return false;
        }
    }
    return true;
}

int playGame(int numOfBallsArr[])
{
    // The current player to take a turn.
    int playerNumber = PLAYER_ONE;
    // Play until one of the boxes is empty.
    while (numOfBallsArr[BOX_ONE - 1] != 0 && numOfBallsArr[BOX_TWO - 1] != 0)
    {
        // Present the state of the boxes.
        presentBoxes(numOfBallsArr);
        // Make a turn for the current player.
        makeTurn(numOfBallsArr, playerNumber);
        // Advance to the next player to take a turn.
        if (playerNumber == PLAYER_ONE)
        {
            playerNumber = PLAYER_TWO;
        }
        else
        {
            playerNumber = PLAYER_ONE;
        }
    }
    // One of the boxes became empty after the last turn, therefor the next
    // player to take a turn is the winner.
    return playerNumber;
}

void presentBoxes(int numberOfBallsArr[])
{
    // The number of balls in the current box.
    int curNumOfBalls, i , j;
    seperatingLine();
    // Go over the boxes.
    for (i = 0; i < NUMBER_OF_BOXES; i++)
    {
        curNumOfBalls = numberOfBallsArr[i];
        // Print the balls in each box.
        printf("Box %d: ", i + 1);
        for (j = 0; j < curNumOfBalls; j++)
        {
            printf("o");
        }
        printf("\n");
    }
    seperatingLine();
}

void seperatingLine()
{
    printf("---------------\n");
}

void makeTurn(int numOfBallsArr[], int playerNumber)
{
    // The number of the box the player chose.
    int boxNumber;
    // The number of balls the player wants to take from the box.
    int numOfBalls;
    // Pick a box to take balls from.
    boxNumber = chooseBox(playerNumber);
    // Take chosen amount of balls from the box.
    numOfBalls = takeBalls(numOfBallsArr, playerNumber, boxNumber);
    // Update the amount of balls in the wanted box.
    numOfBallsArr[boxNumber - 1] -= numOfBalls;

}

int chooseBox(int playerNumber)
{
    // The number of the box the player chose.
    int boxNumber;
    // Ask for a legal box number: only 1 or 2.
    do
    {
    	printf("Player %d, it's your turn.\n", playerNumber);
        printf("Player %d, choose a box (1 or 2):\n", playerNumber);
        scanf("%d", &boxNumber);
    }
    while(boxNumber < BOX_ONE || boxNumber > BOX_TWO);
    return boxNumber;
}

int takeBalls(int numOfBallsArr[], int playerNumber, int boxNumber)
{
    // The number of balls the player wants to take from the box.
    int numOfBalls;
    // Ask for the number of balls.
    printf("Player %d, how many balls do you want to take from box %d?\n",
           playerNumber, boxNumber);
    scanf("%d", &numOfBalls);
    // Keep trying to get a legal number of balls.
    while(numOfBalls <= 0 || numOfBalls > MAX_BALL_WITHDRAWAL ||
          numOfBalls > numOfBallsArr[boxNumber - 1])
    {
        // Negative amount of balls is illegal.
        if (numOfBalls <= 0)
        {
            printf("Number of balls to take must be positive.\n");
        }
            // Only 3 balls at max can be taken at a time.
        else if (numOfBalls > MAX_BALL_WITHDRAWAL)
        {
            printf("Cannot take more than 3 balls at a time.\n");
        }
            // Withdrawing more balls than the current number of balls in the box is illegal.
        else
        {
            printf("Cannot take more balls than what's in the box.\n");
        }
        printf("Player %d, how many balls do you want to take from box %d?\n",
               playerNumber, boxNumber);
        scanf("%d", &numOfBalls);
    }
    return numOfBalls;
}
