/**
 * LineSeparator.h
 *
 *  Created on: Aug 3, 2015
 *      Author: orib
 */

#ifndef LINESEPARATOR_H_
#define LINESEPARATOR_H_


// ------------------------------ includes ------------------------------

#include <stdio.h>
#include <string.h>

// -------------------------- const definitions -------------------------
/**
 * @def MAX_CHARS_IN_LINE 152
 * @brief The max length of a line is 150 chars and we account for the '\n' for
 * the 151st char and '\0' for the 152nd.
 */
#define MAX_CHARS_IN_LINE 152

/**
 * @def MAX_DIMENSION 74
 * @brief The maximal dimension = number of coordinates is at most 74 because
 * in every line with n coordinates there are n ',' and the last char represents
 * the tag of the point. 74+74+1 = 149
 */
#define MAX_DIMENSION 74

/**
 * @def NUM_OF_ARGS 2
 * @brief The legal number of arguments to the program.
 */
#define NUM_OF_ARGS 2

/**
 * @def FILE_INDEX 1
 * @brief The index of the input file in the args array.
 */
#define FILE_INDEX 1

/**
 * @def MIN_DIMENSION 1
 * @brief The value that values above it are legal dimension values.
 */
#define MIN_DIMENSION 1

/**
 * @def POSITIVE_SIDE 1
 * @brief Tag for a point so it belongs on the positive side of the separator.
 */
#define POSITIVE_SIDE 1

/**
 * @def NEGATIVE_SIDE -1
 * @brief Tag for a point so it belongs on the negative side of the separator.
 */
#define NEGATIVE_SIDE -1

/**
 * @def EPSILON 0.00001
 * @brief Precision indicator for tagging points. Dot product of a point and the
 * separator that is lower than this value will tag the point in the negative side,
 * otherwise it will be tagged on the positive side.
 */
#define EPSILON 0.00001

/**
 * @def COMMA ","
 * @brief Macro that separates between numerical values in the text.
 */
#define COMMA ","



// ------------------------------ structs -----------------------------

/**
 * @brief A point in the space. It should extends Vector by inheritance
 * from OOP but since C is a procedural language I preferred not to implement
 * the classes using is-a relation even though it could make things simpler..
 * A point is a vector that can be tagged.
 */
typedef struct Point
{
	int _tag; /** Classifies the point as negative or positive compared to the separator. */
	double _coordinates[MAX_DIMENSION]; /** The coordinates of the point in the space */
}Point;

/**
 * @brief A vector in the space. I used this struct to represent the separator
 * in order to wrap it with a unique name and increase readability,
 * even though it has only one field.
 */
typedef struct Vector
{
	double _coordinates[MAX_DIMENSION]; /** The coordinates of the vector in the space */
}Vector;

// ------------------------------ functions -----------------------------


/**
 * @brief The chief method of the program. Reads the file and uses it's data to
 * create a separator and tag the new points according to it.
 * @param p_file pointer to the file to parse.
 */
void parseFile(FILE* p_file);

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
                                      Vector *separator);

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
                              Point *p_point, Vector *separator);



#endif /* LINESEPARATOR_H_ */



