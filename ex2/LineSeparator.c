/**
 * @file LineSeparator.c
 * @author  orib
 * @version 1.0
 * @date 3 Aug 2015
 *
 * @brief Program that receives examples of points in a space
 * and learns how to classify new examples according to them.
 *
 *
 * @section DESCRIPTION
 * The program implements a Perceptron that classifies points in a space.
 * Input  : Text file with the dimension of the space, the number of point examples
 * and the examples, and a group of points to classify.
 * Process: Parsing the file and initializing objects with the data,
 * using the objects to create a separator and using the separator to
 * tag points that need to be classified.
 * The parsing of the file will be performed exactly once, using minimal memory space
 * by calculating the current data in every line and overriding already used memory
 * areas as we advance to the next lines (using pointers to the same structures).
 * Output : The tag (classification) of the points that require tagging.
 */

// ------------------------------ includes ------------------------------

#include "LineSeparator.h"
#include <assert.h>


// ------------------------------ functions -----------------------------

// ------------------------------ declarations -----------------------------

// Functions that are used by this file to assist in performing the bigger task.
// "private" functions from OOP, as opposed to the "public" methods that are in the
// header file.

/**
 * @brief Compares an example point with the separator vector using dot product
 * in order to increase the precision of the vector.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_examplePoint pointer to the current example point we read.
 * @param p_separator pointer to the separator vector to be updated.
 */
void updateSeparator(const int dimension, Point *p_examplePoint, Vector *p_separator);

/**
 * @brief Classifies a point by using dot product between it and the separator.
 * The resulted tag can be 1 or -1 depending of the result of the dot product:
 * value smaller than 0.00001 will result in -1 while bigger or equal to 0.00001
 * will result in 1. Values between +-0.00001 will be classified as -1 even though
 * the point belongs on neither side of the separator.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_point pointer to the current point we read.
 * @param p_separator pointer to the separator vector.
 * return the resulted tag of the point to be printed.
 */
int tagPoint(const int dimension, Point *p_point, Vector *p_separator);

/**
 * @brief Reads the coordinates of a given line in the file and stores them
 * in a struct.
 * @param line the current line in the file we read.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_point pointer to the current point we read.
 * @return The last value in the line: the tag of the point in case of an example point
 * and 0 in case of a point to tag (since it has no tag value).
 */
char* parsePointCoordinates(char line[], const int dimension, Point *p_point);


/**
 * @brief defines an addition between 2 vectors in the space.
 * @param firstVecCoordinates the coordinates of the first vector to add.
 * @param secondVecCoordinates the coordinates of the second vector to add.
 * @param dimension the number of coordinates of the vectors.
 * @return the coordinates of the result vector, being saved in the FIRST coordinates
 * array (overriding the previous coordinates it had, thus saving memory space).
 */
double* vectorAddition(double firstVecCoordinates[], const double secondVecCoordinates[],
                       const int dimension);

/**
 * @brief defines an multiplication of vector by scalar in the space.
 * @param scalar the value to multiply the vector by.
 * @param vecCoordinates the coordinates of the vector to be multiplied.
 * @param dimension the number of coordinates of the vector.
 * @return the coordinates of the result vector, being saved in the coordinates
 * array (overriding the previous coordinates it had, thus saving memory space).
 */
double* scalarMultiplication(const int scalar, double vecCoordinates[], const int dimension);

/**
 * @brief defines a dot product between 2 vectors.
 * @param firstVecCoordinates the coordinates of the first vector.
 * @param secondVecCoordinates the coordinates of the second vector.
 * @param dimension the number of coordinates of the vectors.
 * @return the dot product of the 2 vectors.
 */
double getDotProduct(const double firstVecCoordinates[], const double secondVecCoordinates[],
                     const int dimension);

// ------------------------------ implementations -----------------------------

/**
 * @brief  Checks for legal input file and calls the
 * parser to parse the file.
 * @param argc the number of arguments the program receives - only 2 is legal.
 * @param argv the name of the program and the path to the input file.
 * @return 0, to tell the system the execution ended without errors.
 */
int main(const int argc, const char* argv[])
{
	// Pointer to the file we want to parse.
	FILE *p_file = NULL;
    // Illegal number of arguments.
	if (argc != NUM_OF_ARGS)
	{
		printf("Usage: LineSeparator <input file>\n");
		return 0;
	}
	// Attempt to open the given file for reading.
	p_file = fopen(argv[FILE_INDEX], "r");
	// The file could not be open.
	if (p_file == NULL)
	{
		printf("Unable to open input file: %s\n", argv[1]);
		return 0;
	}
	// The input to the program is legal. Start parsing.
    parseFile(p_file);
    // Parsing is completed, close the file.
    fclose(p_file);
	return 0;
}

/**
 * @brief The chief method of the program. Reads the file and uses it's data to
 * create a separator and tag the new points according to it.
 * @param p_file pointer to the file to parse.
 */
void parseFile(FILE* p_file)
{
    // The current line we parse.
    char line[MAX_CHARS_IN_LINE] = {0};
    // The dimension of the vector space aka number of coordinates in each point.
    int dimension = 0;
    // The number of example points in the given file.
    int numOfExamplePoints = 0;
    // An example point in the space as given in the file.
    Point point;
    // The linear separator to be created.
    Vector separator;
    // A pointer to the linear separator we will perform actions with.
    Vector *p_separator = &separator;
    // Parse the dimension of the space.
    fgets(line, MAX_CHARS_IN_LINE, p_file);
    sscanf(line, "%d", &dimension);
    assert(dimension > MIN_DIMENSION);
    // Parse the number of example points.
    fgets(line, MAX_CHARS_IN_LINE, p_file);
    sscanf(line, "%d", &numOfExamplePoints);
    assert(numOfExamplePoints > 0);
    // Create the line separator according to the given example points in the file.
    p_separator = getSeparatorFromExamplePoints(p_file, line, numOfExamplePoints, dimension,
                                                &point, &separator);
    // We have the complete separator, now we can start tagging the untagged examples.
    tagUntaggedExamplePoints(p_file, line, dimension, &point, p_separator);
}


/**
 * @brief Reads the section of the file that includes the example points
 * and creates a separator according to those points.
 * @param p_file pointer to the file to parse.
 * @param line the current line in the file we read.
 * @param numOfExamplePoints the amount of example points to read.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_examplePoint pointer to the current example point we read.
 * @param p_separator pointer to the separator vector to be created.
 * @return pointer to the separator vector that was created.
 */
Vector* getSeparatorFromExamplePoints(FILE* p_file, char line[], const int numOfExamplePoints,
                                      const int dimension, Point *p_examplePoint,
                                      Vector *p_separator)
{
    int i;
    // The tag of the example point.
    char *tag;
    // Initialize the separator vector.
    for (i = 0; i < MAX_DIMENSION; i++)
    {
        p_separator -> _coordinates[i] = 0;
    }

    // Go over the example points save their data in an adequate struct.
    for (i = 0; i < numOfExamplePoints; i++)
    {
        fgets(line, MAX_CHARS_IN_LINE, p_file);
        // Save the coordinates in the struct.
        tag = parsePointCoordinates(line, dimension, p_examplePoint);
        // Save the tag in the struct.
        sscanf(tag, "%d", &p_examplePoint -> _tag);
        // The tag can be only 1 or -1.
        assert(p_examplePoint -> _tag == NEGATIVE_SIDE ||
        		p_examplePoint -> _tag == POSITIVE_SIDE);
        // Update the coordinates of the separator according to the current point.
        updateSeparator(dimension, p_examplePoint, p_separator);
    }
    return p_separator;
}

/**
 * @brief Reads the coordinates of a given line in the file and stores them
 * in a struct.
 * @param line the current line in the file we read.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_point pointer to the current point we read.
 * @return The last value in the line: the tag of the point in case of an example point
 * and 0 in case of a point to tag (since it has no tag value).
 */
char* parsePointCoordinates(char line[], const int dimension, Point *p_point)
{
    int j;
    // The current numeric value in the line.
    char *curNum;
    // Get the next numeric value in the line.
    curNum = strtok(line, COMMA);
    assert(curNum != NULL);
    for (j = 0; j < dimension; j++)
    {
        sscanf(curNum, "%lf", &p_point -> _coordinates[j]);
        curNum = strtok(NULL, COMMA);
    }
    // Return the last numeric value: a tag or 0.
    return curNum;
}

/**
 * @brief Reads the last section of the file that includes the points to tag
 * and tags them according to the separator vector.
 * @param p_file pointer to the file to parse.
 * @param line the current line in the file we read.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_point pointer to the current point we read.
 * @param p_separator pointer to the separator vector.
 */
void tagUntaggedExamplePoints(FILE* p_file, char line[], const int dimension,
                              Point *p_point, Vector *separator)
{
    // The tag we will grant the untagged point.
    int tagOfPoint = 0;
    // Keep searching for untagged examples until the end of file.
    while (1)
    {
        // We reached the EOF marker and so the parsing is completed.
        if (fgets(line, MAX_CHARS_IN_LINE, p_file) == NULL)
        {
            break;
        }
        // Obtain the coordinates of the untagged point.
        parsePointCoordinates(line, dimension, p_point);
        // Tag the point according to it's coordinates and the separator's.
        tagOfPoint = tagPoint(dimension, p_point, separator);
        printf("%d\n", tagOfPoint);
    }
}

/**
 * @brief Compares an example point with the separator vector using dot product
 * in order to increase the precision of the vector.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_examplePoint pointer to the current example point we read.
 * @param p_separator pointer to the separator vector to be updated.
 */
void updateSeparator(const int dimension, Point *p_examplePoint, Vector *p_separator)
{
    // Pointer to the coordinates of the example point.
    double *pointCoordinates = p_examplePoint -> _coordinates;
    // Pointer to the coordinates of the separator vector.
    double *vectorCoordinates = p_separator -> _coordinates;
    // The tag of the example point.
    int pointTag = p_examplePoint -> _tag;
    // Calculate the dot product of the separator and the example point.
    double dotProduct = getDotProduct(vectorCoordinates, pointCoordinates, dimension);
    // The separator needs to be updated.
    if ((dotProduct >= EPSILON) && (pointTag == NEGATIVE_SIDE))
    {
        // Update the coordinates of the point by multiplying by the tag of the point.
        pointCoordinates = scalarMultiplication(NEGATIVE_SIDE, pointCoordinates, dimension);
    }
    // The separator needs to be updated.
    else if ((dotProduct < EPSILON) && (pointTag == POSITIVE_SIDE))
    {
        // Update the coordinates of the point by multiplying by the tag of the point.
        pointCoordinates = scalarMultiplication(POSITIVE_SIDE, pointCoordinates, dimension);
    }
        // The separator does not need to be updated. Moving on.
    else
    {
        return;
    }
    // Update the separator by adding the revised point coordinates.
    vectorCoordinates = vectorAddition(vectorCoordinates, pointCoordinates, dimension);

}

/**
 * @brief defines an addition between 2 vectors in the space.
 * @param firstVecCoordinates the coordinates of the first vector to add.
 * @param secondVecCoordinates the coordinates of the second vector to add.
 * @param dimension the number of coordinates of the vectors.
 * @return the coordinates of the result vector, being saved in the FIRST coordinates
 * array (overriding the previous coordinates it had, thus saving memory space).
 */
double* vectorAddition(double firstVecCoordinates[], const double secondVecCoordinates[],
                       const int dimension)
{
    int i;
    for (i = 0; i < dimension; i++)
    {
        firstVecCoordinates[i] += secondVecCoordinates[i];
    }
    return firstVecCoordinates;
}

/**
 * @brief defines an multiplication of vector by scalar in the space.
 * @param scalar the value to multiply the vector by.
 * @param vecCoordinates the coordinates of the vector to be multiplied.
 * @param dimension the number of coordinates of the vector.
 * @return the coordinates of the result vector, being saved in the coordinates
 * array (overriding the previous coordinates it had, thus saving memory space).
 */
double* scalarMultiplication(const int scalar, double vecCoordinates[], const int dimension)
{
    int i;
    for (i = 0; i < dimension; i++)
    {
        vecCoordinates[i] *= scalar;
    }
    return vecCoordinates;
}

/**
 * @brief defines a dot product between 2 vectors.
 * @param firstVecCoordinates the coordinates of the first vector.
 * @param secondVecCoordinates the coordinates of the second vector.
 * @param dimension the number of coordinates of the vectors.
 * @return the dot product of the 2 vectors.
 */
double getDotProduct(const double firstVecCoordinates[], const double secondVecCoordinates[],
                     const int dimension)
{
    double result = 0;
    int i;
    for (i = 0; i < dimension; i++)
    {
        result += firstVecCoordinates[i] * secondVecCoordinates[i];
    }
    return result;
}

/**
 * @brief Classifies a point by using dot product between it and the separator.
 * The resulted tag can be 1 or -1 depending of the result of the dot product:
 * value smaller than 0.00001 will result in -1 while bigger or equal to 0.00001
 * will result in 1. Values between +-0.00001 will be classified as -1 even though
 * the point belongs on neither side of the separator.
 * @param dimension the dimension of the space = the number of coordinates
 * Point holds.
 * @param p_point pointer to the current point we read.
 * @param p_separator pointer to the separator vector.
 * return the resulted tag of the point to be printed.
 */
int tagPoint(int dimension, Point *p_point, Vector *p_separator)
{
    // Pointer to the coordinates of the point to be tagged.
    double *pointCoordinates = p_point -> _coordinates;
    // Pointer to the coordinates of the separator.
    double *vectorCoordinates = p_separator -> _coordinates;
    // The tag of the point to be tagged.
    int pointTag = p_point -> _tag;
    // Calculate dot product of the separator and the point to be tagged.
    double dotProduct = getDotProduct(vectorCoordinates, pointCoordinates, dimension);
    // The point belongs in the positive side of the separator.
    if (dotProduct >= EPSILON)
    {
        pointTag = POSITIVE_SIDE;
    }
        // The point belongs in the negative side of the separator. An exception is
        // if dotProduct < 0.00001 && dotProduct > -0.00001 then the point belongs
        // on the separator itself, however the tag will still be negative.
    else
    {
        pointTag = NEGATIVE_SIDE;
    }
    // The tag can be only 1 or -1.
    assert(pointTag == NEGATIVE_SIDE || pointTag == POSITIVE_SIDE);
    return pointTag;
}
