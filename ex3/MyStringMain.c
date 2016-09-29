#include "MyString.h"
#include <stdio.h>
#include <string.h>

#define MAX_STRING_LENGTH 502
#define ERROR_ALLOCATING_STRING "Error Occured in allocating memory!"
#define ERROR_SETTING_STRING "Error Occured in setting String"
#define ERROR_OPENING_FILE "Couldn't open file!"
#define ERROR_IN_COMPARE "Error in comparing!"
#define SMALLER "%s is smaller than %s\n"
#define OUTPUT_FILE "test.out"
#define END_OF_LINE '\0'
#define NEW_LINE '\n'

/**
 * @brief Calculate the length of the filtered string to be created. we do this
 * in order to pre-determine the size we need for the string in the heap.
 * @param str the source string we would like to filter.
 * @param filt pointer to a function that filters in some manner.
 * @return the length of the filtered string.
 *
 */

/**
 * @brief function to get input string from the user.
 * @param str
 *
 */
void getStringFromUser(char *str)
{
	char *endLine;
	printf("Please enter String:\n");
	fgets(str, MAX_STRING_LENGTH, stdin);
	if ((endLine = strchr(str, NEW_LINE)) != NULL)
	{
      *endLine = END_OF_LINE;
	}
}

/**
 * @brief the main function.
 */
int main()
{
	
	char str1[MAX_STRING_LENGTH];
	char str2[MAX_STRING_LENGTH];
	getStringFromUser(str1);
	getStringFromUser(str2);
	MyString * myStr1 = myStringAlloc();
	MyString * myStr2 = myStringAlloc();
	if (myStr1 == NULL || myStr2 == NULL)
	{
		fprintf(stderr, ERROR_ALLOCATING_STRING);

	}
	else
	{
		MyStringRetVal retVal1 = myStringSetFromCString(myStr1, str1);
		MyStringRetVal retVal2 = myStringSetFromCString(myStr2, str2);
		
		if (retVal1 == MYSTRING_ERROR || retVal2 == MYSTRING_ERROR)
		{
			fprintf(stderr, ERROR_SETTING_STRING);
		}
		else
		{
		
			FILE * pFile = fopen (OUTPUT_FILE, "w");
			if (pFile != NULL)
			{

				int cmpResult = myStringCompare(myStr1, myStr2);
				if (cmpResult == MYSTR_ERROR_CODE)
				{
					fprintf(stderr, ERROR_IN_COMPARE);

				}
				else if (cmpResult <= 0)
				{
					fprintf(pFile, SMALLER, str1, str2);
				}
				else
				{
					fprintf(pFile, SMALLER, str2, str1);

				}
				fclose (pFile);

			}
			else
			{
				fprintf(stderr, ERROR_OPENING_FILE);
				
			}
		}
		myStringFree(myStr1);
		myStringFree(myStr2);

	}


	return 0;


}
