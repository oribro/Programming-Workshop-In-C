/**
 * @file MyString.c
 * @author  orib
 * @version 1.0
 * @date 12 Aug 2015
 *
 * @brief This file is an implementation of library for functions that support
 * a structure that represents a string which is allocated dynamically.
 *
 *
 * @section DESCRIPTION
 * I chose to implement MyString memory allocations in the following manner:
 * The struct MyString composes the chars of the string: it points to a certain
 * char and we can iterate the rest of the chars using pointers since they are
 * located one after another in the heap. The chars do not include '\0' and thus
 * they are different from a regular C String. In the struct we also have
 * the length of the string, meaning the number of chars the struct wraps.
 * Every time we change the amount of chars in the struct, we update the
 * length accordingly.
 * Memory to the heap is allocated as we initialize a pointer to the struct,
 * which is initialized with a char '\0' (which will be overridden at any future
 * reallocation) and the length of the empty string.
 * Every time we change the string we use reallocation which takes advantage of the
 * block of memory we already allocated at first and overrides it. This way
 * we use the same block and avoid using new blocks of memory, saving memory.
 * When our program stops using a block of memory and moves to another one such as
 * in myStringClone, we free the old block of memory.
 *
 * I used quick sort to sort the array of MyStrings,
 * memcpy and memcmp to change the string in the struct and to compare it to
 * another string, by first calculating the size i would allocate to the future string
 * and then use reallocation, and then memcpy to change the string.
 * I used fprintf and sscanf to parse the string and perform actions such as
 * converting it to int and vice versa.
 */

// ------------------------------ includes ------------------------------
#include "MyString.h"



// ------------------------------ structures -----------------------------

typedef struct _MyString
{
    char *_chars;
    unsigned long _length;
}MyString;
// ------------------------------ functions -----------------------------

/**
 * @brief Change the length of the string in the heap.
 * We use realloc to reuse the same address in the heap each time we change
 * the size of the string. This way we save lots of memory.
 * @param chars the string we change the length of.
 * @param length pointer to the length we will change in the struct.
 * @param newSize the new length to assign to the string.
 *
 * @return MYSTRING_ERROR if the reallocation failed,
 * 			MYSTRING_SUCCESS if the reallocation succeeded.
 */
static MyStringRetVal adjustMyStringLength(char **chars, unsigned long *length,
										   unsigned long newSize);

/**
 * @brief Calculate the length of a cString.
 * @param str the string to calculate the length of.
 *
 * @return the length of the string.
  */
static unsigned long getCStringLength(const char *str);
/**
 * @brief Get the number of digits of a given integer number.
 * @param n the number we get the digits of.
 *
 * @return the number of digits.
 */
static int getNumOfDigits(int n);
/**
 * @brief Calculate the length of the filtered string to be created. we do this
 * in order to pre-determine the size we need for the string in the heap.
 * @param str the source string we would like to filter.
 * @param filt pointer to a function that filters in some manner.
 * @return the length of the filtered string.
 *
 */
static unsigned long getFilteredStringLength(const char *str, bool (*filt)(const char *));
/**
 * @brief Filter the chars of a string according to a filter and create a new string.
 * @param filteredString the new filtered string.
 * @param unfilteredString the source string we would like to filter.
 * @param filt pointer to a function that filters in some manner.
 *
 */
static void createFilteredString(char *filteredString, const char *unfilteredString,
								 bool (*filt)(const char *));
/**
 * @brief Creates an empty string with length 0 in the heap.
 * @param p_myString The string we alloc.
 * @return Pointer to the chars of the string if we succeeded, NULL otherwise.
 *
 */
static char* emptyStringAlloc(MyString *p_myString);

/**
 * @brief Wrapper function of a comparator for the qsort function.
 * @param str1
 * @param str2
 * @return The value the default comparator returned (1/0/-1).
 *
 */
static int myStringComparator(const void *str1, const void *str2);



// ------------------------------ implementation -----------------------------


/**
 * @brief Allocates a new MyString and sets its value to "" (the empty string).
 * 			It is the caller's responsibility to free the returned MyString.
 *
 * RETURN VALUE:
 * @return a pointer to the new string, or NULL if the allocation failed.
 *
 *
 * Complexity: O(1) since we only initialize the struct with an empty string
 * (one char).
 */
MyString * myStringAlloc()
{
    // Attempt to allocate memory for myString on the heap.
    MyString *p_myString = (MyString *) malloc(sizeof(MyString));
    if (p_myString == NULL)
    {
        return NULL;
    }
    // The struct on the heap now wraps an empty string.
    if (emptyStringAlloc(p_myString) == NULL)
    {
    	return NULL;
    }

    return p_myString;
}


/**
 * @brief Frees the memory and resources allocated to str.
 * @param str the MyString to free.
 * If str is NULL, no operation is performed.
 *
 * Complexity: O(n) since str -> _chars points to the address on the heap
 * we allocated before, and each time we realloc size of n bytes we take
 * O(n) memory space.
 */
void myStringFree(MyString *str)
{
	if (str == NULL)
	{
		return;
	}
    free(str -> _chars);
    str -> _chars = NULL;
    free(str);
}


/**
 * @brief Allocates a new MyString with the same value as str. It is the caller's
 * 			responsibility to free the returned MyString.
 * @param str the MyString to clone.
 * RETURN VALUE:
 *   Complexity: O(n) since we create a new struct in the memory with size of
 *   n bytes as chars.
 */
MyString * myStringClone(const MyString *str)
{
	if (str == NULL)
	{
		return NULL;
	}
    // Indicator of the cloning success or failure.
    MyStringRetVal retVal = MYSTRING_SUCCESS;
    // The cloned string to be returned.
    MyString *newMyString = myStringAlloc();
    // Attempt to clone the string.
    retVal = myStringSetFromMyString(newMyString, str);
    if (retVal == MYSTRING_ERROR)
    {
        return NULL;
    }
    return newMyString;
}


/**
 * @brief Sets the value of str to the value of other.
 * @param str the MyString to set
 * @param other the MyString to set from
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *   Complexity: O(n) since we allocate memory as the size of the other string.
 */
MyStringRetVal myStringSetFromMyString(MyString *str, const MyString *other)
{
	if (str == NULL || other == NULL)
	{
		return MYSTRING_ERROR;
	}
	// Empty string set.
	if (other -> _length == EMPTY_STRING_LENGTH)
	{
		if (emptyStringAlloc(str) == NULL)
		{
			return MYSTRING_ERROR;
		}
		return MYSTRING_SUCCESS;
	}
    // The length of the other string.
    unsigned long otherStringLength = myStringLen(other);
    // Change the size of the string to the size of the other string.
    MyStringRetVal adjustLengthRetVal = adjustMyStringLength(&str -> _chars,
    		&str -> _length, otherStringLength);
    if (adjustLengthRetVal == MYSTRING_ERROR)
    {
        return MYSTRING_ERROR;
    }
    memcpy(str -> _chars, other -> _chars, otherStringLength);
    return MYSTRING_SUCCESS;
}

/**
 * @brief filter the value of str acording to a filter.
 * 	remove from str all the occurrence of chars that are filtered by filt
 *	(i.e. filr(char)==true)
 * @param str the MyString to filter
 * @param filt the filter
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *   Complexity: O(n) since we allocate a new block in the memory
 *   for the filtered string with a size of n bytes at most.
 *   */
MyStringRetVal myStringFilter(MyString *str, bool (*filt)(const char *))
{
	if (str == NULL || filt == NULL)
	{
		return MYSTRING_ERROR;
	}
	if (str -> _length == EMPTY_STRING_LENGTH)
	{
		return MYSTRING_SUCCESS;
	}
    unsigned long filteredStringLength = getFilteredStringLength(str -> _chars, filt);
    // Allocate a new block of memory heap for the filtering procedure.
    MyString *filteredString = myStringAlloc();
    // Allocate size of the filtered string length in the heap.
    MyStringRetVal adjustLengthRetVal = adjustMyStringLength(&filteredString -> _chars,
    		&filteredString -> _length, filteredStringLength);
    if (adjustLengthRetVal == MYSTRING_ERROR)
    {
        return MYSTRING_ERROR;
    }
    // Create the new filtered string.
    createFilteredString(filteredString -> _chars, str -> _chars, filt);
    // Update the original string to be the filtered one.
    str -> _chars = filteredString ->_chars;
    str -> _length = filteredString ->_length;
    myStringFree(filteredString);
    return MYSTRING_SUCCESS;
}


/**
 * @brief Sets the value of str to the value of the given C string.
 * 			The given C string must be terminated by the null character.
 * 			Checking will not use a string without the null character.
 * @param str the MyString to set.
 * @param cString the C string to set from.
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *   Complexity: O(n) since we allocate memory of the length of the
 *   cString.
 */
MyStringRetVal myStringSetFromCString(MyString *str, const char * cString)
{
	if (str == NULL || cString == NULL)
	{
		return MYSTRING_ERROR;
	}
    unsigned long cStringLength = getCStringLength(cString);
    // Checking that the cString is legal.
    if (*(cString + cStringLength) != END_OF_C_STRING)
    {
    	return MYSTRING_ERROR;
    }

    // Empty string case.
    if (cStringLength == 0)
    {
        if( emptyStringAlloc(str) == NULL)
        {
        	return MYSTRING_ERROR;
        }
    }
    // Change the size of the string to the size of the cString.
    MyStringRetVal adjustLengthRetVal = adjustMyStringLength(&str -> _chars,
    		&str -> _length, cStringLength);
    if (adjustLengthRetVal == MYSTRING_ERROR)
    {
        return MYSTRING_ERROR;
    }
    memcpy(str -> _chars, cString, cStringLength);
    return MYSTRING_SUCCESS;
}


/**
 * @brief Sets the value of str to the value of the integer n.
 *	(i.e. if n=7 than str should contain �7�)
 * 	You are not allowed to use itoa (or a similar functions) here but
 * 	 must code your own conversion function.
 * @param str the MyString to set.
 * @param n the int to set from.
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *   Complexity: O(k) where k is the number of digits since we allocate
 *   a block of the length of the number.
 */
MyStringRetVal myStringSetFromInt(MyString *str, int n)
{
	if (str == NULL)
	{
		return MYSTRING_ERROR;
	}
    unsigned long numOfDigits = (unsigned long) getNumOfDigits(n);
    // Indicator for the success of the action.
    int setIndicator = 0;
    // Change the size of the string to the number of digits.
    MyStringRetVal adjustLengthRetVal = adjustMyStringLength(&str -> _chars,
    		                                                 &str -> _length, numOfDigits);
    if (adjustLengthRetVal == MYSTRING_ERROR)
    {
        return MYSTRING_ERROR;
    }
    // Attempt to write the number to the string.
    setIndicator = sprintf(str -> _chars, "%d", n);
    if (setIndicator < 0)
    {
        return MYSTRING_ERROR;
    }
    return MYSTRING_SUCCESS;
}


/**
 * @brief Returns the value of str as an integer.
 * 	If str cannot be parsed as an integer,
 * 	the return value should be MYSTR_ERROR_CODE
 * 	NOTE: positive and negative integers should be supported.
 * @param str the MyString
 * @return an integer
 *
 *  Complexity: O(k) where k is the length of the number since we iterate
 *   over the digits of the number.
 */
int myStringToInt(const MyString *str)
{
	if (str == NULL)
	{
		return MYSTRING_ERROR;
	}
    int value = 0;
    int numberOfDigits = 0;
    // Attempt to parse the string as an integer.
    numberOfDigits = sscanf(str -> _chars, "%d", &value);
    printf("%d\n", numberOfDigits);
    if(numberOfDigits <= 0)
    {
        return MYSTR_ERROR_CODE;
    }
    return value;
}


/**
 * @brief Returns the value of str as a C string, terminated with the
 * 	null character. It is the caller's responsibility to free the returned
 * 	string by calling free().
 * @param str the MyString
 * RETURN VALUE:
 *  @return the new string, or NULL if the allocation failed.
 *
 *   Complexity: O(n) since we allocate a new block of memory as the length
 *   of the string.
 */
char * myStringToCString(const MyString *str)
{
	if (str == NULL)
	{
		return NULL;
	}
	// Get the length of the string before change.
    unsigned long oldLength = myStringLen(str);
    // Allocate memory for the string along with '\0'.
    char *newString = (char *) malloc(oldLength + 1);
    if (newString == NULL)
    {
        return NULL;
    }
    // Create the new CString.
    memcpy(newString, str -> _chars, oldLength);
    newString[oldLength] = END_OF_C_STRING;
    return newString;
}


/**
 * @brief Appends a copy of the source MyString src to the destination MyString dst.
 * @param dest the destination
 * @param src the MyString to append
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *   Complexity: O(m) where m  is the size of src
 *   since we cat the src string and the dest is already allocated.
 *   n bytes as chars.
 */
MyStringRetVal myStringCat(MyString * dest, const MyString * src)
{
	if (dest == NULL || src == NULL)
	{
		return MYSTRING_ERROR;
	}
	// Empty string, nothing to cat.
	if (src -> _length == EMPTY_STRING_LENGTH)
	{
		return MYSTRING_SUCCESS;
	}
    unsigned long srcLength = myStringLen(src);
    unsigned long destLength = myStringLen(dest);
    // Allocate memory of the summed length of the strings.
    MyStringRetVal adjustLengthRetVal = adjustMyStringLength(&dest -> _chars,
    														 &dest -> _length,
    														 srcLength + destLength);
    if (adjustLengthRetVal == MYSTRING_ERROR)
    {
        return MYSTRING_ERROR;
    }
    // Cat the strings.
    memcpy(dest -> _chars + destLength, src -> _chars, srcLength);
    return MYSTRING_SUCCESS;

}

/**
 * @brief Sets result to be the concatenation of str1 and str2.
 * 	result should be initially allocated by the caller.
 * 	result shouldn't be the same struct as str1 or str2.
 * @param str1
 * @param str2
 * @param result
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *   Complexity: O(n+m) since we copy both of the strings to a new location
 *   so we iterate over the sum of their lengths.
 */
MyStringRetVal myStringCatTo(const MyString *str1, const MyString *str2, MyString *result)
{
	if (str1 == NULL || str2 == NULL || result == NULL)
	{
		return MYSTRING_ERROR;
	}
	// Empty string cat case.
	if (str1 -> _length == EMPTY_STRING_LENGTH && str2 -> _length == EMPTY_STRING_LENGTH)
	{
		if(emptyStringAlloc(result) != NULL)
		{
			return MYSTRING_SUCCESS;
		}
		return MYSTRING_ERROR;
	}
    unsigned long strLength1 = myStringLen(str1);
    unsigned long strLength2 = myStringLen(str2);
    // Allocate memory of the summed length of the strings.
    MyStringRetVal adjustLengthRetVal = adjustMyStringLength(&result -> _chars,
    														 &result -> _length,
    														 strLength1 + strLength2);
    if (adjustLengthRetVal == MYSTRING_ERROR)
    {
        return MYSTRING_ERROR;
    }
    // Create the first string in the new memory address.
    memcpy(result -> _chars, str1 -> _chars, strLength1);
    // Cat the second string to the first string in the new address.
    memcpy(result -> _chars + strLength1, str2 -> _chars, strLength2);
    return MYSTRING_SUCCESS;
}

/**
 * @brief Compare str1 and str2.
 * @param str1
 * @param str2
 *
 * RETURN VALUE:
 * @return an integral value indicating the relationship between the strings:
 * 	A zero value indicates that the strings are equal.
 * 	A value greater than zero indicates that the first character that does not match
 * 	 has a greater ASCII value in str1 than in str2;
 * 	And a value less than zero indicates the opposite.
 * 	If strings cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	 Complexity: O(n) where n is the length of the shorter string since at the worst
 * 	 case we will reach the end of the shorter string and then the longer is bigger.
 *
 */
int myStringCompare(const MyString *str1, const MyString *str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        return MYSTR_ERROR_CODE;
    }

    unsigned long strLength1 = myStringLen(str1);
    unsigned long strLength2 = myStringLen(str2);
    // str1 has a greater ascii value.
    if (strLength1 > strLength2)
    {
        int equalUntilLength2 = memcmp(str1 -> _chars, str2 -> _chars, strLength2);
        return equalUntilLength2 >= EQUAL_STRINGS ? STR1_BIGGER : STR2_BIGGER;
    }
      // str2 has a greater ascii value.
    else if (strLength1 < strLength2)
    {
        int equalUntilLength1 = memcmp(str1 -> _chars, str2 -> _chars, strLength1);
        return equalUntilLength1 <= EQUAL_STRINGS ? STR2_BIGGER : STR1_BIGGER;
    }
        // The strings have equal lengths.
    else
    {
        return memcmp(str1 -> _chars, str2 -> _chars, strLength1);
    }
}


/**
 * @brief Compares str1 and str2.
 * @param str1
 * @param str2
 * @param comparator
 * RETURN VALUE:
 * @return an integral value indicating the relationship between the strings:
 * 	A zero value indicates that the strings are equal according to the
 * 	 custom comparator (3rd parameter).
 * 	A value greater than zero indicates that the first
 * 	 MyString is bigger according to the comparator.
 * 	And a value less than zero indicates the opposite.
 * 	If strings cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	 Complexity: O(n) where n is the length of the shorter string since at the worst
 * 	 case we will reach the end of the shorter string and then the longer is bigger.
 */

int myStringCustomCompare(const MyString *str1, const MyString *str2,
						  int (*comparator) (const MyString *, const MyString *))
{
    if (str1 == NULL || str2 == NULL || comparator == NULL)
    {
        return MYSTR_ERROR_CODE;
    }
    // The length of the strings.
    unsigned long strLength1 = myStringLen(str1);
    unsigned long strLength2 = myStringLen(str2);
    unsigned long i = 0;
    // The substrings we iterate are in the same length.
    while (i < strLength1 && i < strLength2)
    {
        // The first char is bigger than the second char compared by the comparator.
        if (comparator(str1, str2) > EQUAL_STRINGS)
        {
            return STR1_BIGGER;
        }
        // The second char is bigger than the first char compared by the comparator.
        if (comparator(str1, str2) < EQUAL_STRINGS)
        {
            return STR2_BIGGER;
        }
        i++;
    }
    // The strings are of different lengths and the common substrings are equal.
    if (strLength1 > strLength2)
    {
        return STR1_BIGGER;
    }
    if (strLength1 < strLength2)
    {
        return STR2_BIGGER;
    }
    // The strings are truly identical.
    return EQUAL_STRINGS;
}



/**
 * @brief Check if str1 is equal to str2.
 * @param str1
 * @param str2
 *
 * RETURN VALUE:
 * @return an integral value indicating the equality of the strings (logical equality -
 *  that they are composed of the very same characters):
 * 	A zero value indicates that the strings are not equal.
 * 	A greater than zero value indicates that the strings are equal.
 * 	If strings cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	 Complexity: O(n) where n is the length of the shorter string since at the worst
 * 	 case we will reach the end of the shorter string and then we will know if the strings
 * 	 are equal or not.
 *
  */
int myStringEqual(const MyString *str1, const MyString *str2)
{
    // The strings cannot be compared.
    if (str1 == NULL || str2 == NULL)
    {
        return MYSTR_ERROR_CODE;
    }
    // Compare the strings to see if they're equal.
    int areEqual = myStringCompare(str1, str2);
    return areEqual == EQUAL_STRINGS ? TRUE : FALSE;
}

/**
 * @brief Check if str1 is equal to str2.
 * @param str1
 * @param str2
 * @param comparator
 *
 * RETURN VALUE:
 * @return an integral value indicating the equality of the strings using a
 * custom comparator (3rd parameter):
 * 	A zero value indicates that the strings are not equal.
 * 	A greater than zero value indicates that the strings are equal.
 * 	If string cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	 Complexity: O(n) where n is the length of the shorter string since at the worst
 * 	 case we will reach the end of the shorter string and then we will know if the strings
 * 	 are equal or not.
 */
int myStringCustomEqual(const MyString *str1, const MyString *str2,
						int (*comparator) (const MyString *, const MyString *))
{
    // The strings cannot be compared.
    if (str1 == NULL || str2 == NULL)
    {
        return MYSTR_ERROR_CODE;
    }
    // Compare the strings to see if they're equal by the comparator.
    int areEqual = myStringCustomCompare(str1, str2, comparator);
    return areEqual == EQUAL_STRINGS ? TRUE : FALSE;
}


/**
 * @return the amount of memory (all the memory that used by the MyString object
 *  itself and its allocations), in bytes, allocated to str1.
 *
 *  Complexity: O(1) because the length is kept in the structure .
 */
unsigned long myStringMemUsage(const MyString *str1)
{
	if (str1 == NULL)
	{
		return NO_MEMORY_USAGE;
	}
	return (sizeof(MyString) + str1 -> _length);
}

/**
 * @return the length of the string in str1.
 *
 * Complexity: O(1) because the length is kept in the structure .
 */
unsigned long myStringLen(const MyString *str1)
{
	if (str1 == NULL)
	{
		return MYSTRING_ERROR;
	}
    return str1 -> _length;
}

/**
 * Writes the content of str to stream. (like fputs())
 *
 * RETURNS:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *  Complexity: O(n) because we iterate over the C String until '\0'
 *  is reached - as done by fprintf.
 */
MyStringRetVal myStringWrite(const MyString *str, FILE *stream)
{
    if (stream == NULL || str == NULL)
    {
        return MYSTRING_ERROR;
    }
    char * cStr = myStringToCString(str);
    // Write to the file.
    int writingIndicator = fprintf(stream, "%s", cStr);
    // Checks if the writing process was successful.
	if (writingIndicator < 0)
	{
		return MYSTRING_ERROR;
	}
	free(cStr);
	cStr = NULL;
	fclose(stream);
    return MYSTRING_SUCCESS;
}

/**
 * @brief sort an array of MyString pointers
 * @param arr
 * @param len
 * @param comparator custom comparator
 *
 * RETURN VALUE:none
 *
 *  Complexity: O(n^2) because quick sort performs this way at the worst case.
 *  On average we will acheive O(nlogn)
  */

void myStringCustomSort(MyString *arr[], unsigned long len,
						int (*comparator) (const void *, const void *))
{
	if (arr == NULL || comparator == NULL)
	{
		return;
	}
    qsort(arr, len, sizeof(MyString*), comparator);
}

/**
 * @brief sorts an array of MyString pointers according to the default comparison
 *  (like in myStringCompare)
 * @param arr
 * @param len
 *
 * RETURN VALUE: none
 *
 * *  Complexity: O(n^2) because quick sort performs this way at the worst case.
 *  On average we will acheive O(nlogn)
  */

void myStringSort(MyString *arr[], unsigned long len)
{
	if (arr == NULL)
	{
		return;
	}
    qsort(arr, len, sizeof(MyString*), myStringComparator);
}

/**
 * @brief Calculate the length of a cString.
 * @param str the string to calculate the length of.
 *
 * @return the length of the string.
 *
  */

static unsigned long getCStringLength(const char *str)
{
    unsigned long length = 0;
    while (str[length] != END_OF_C_STRING)
    {
        length++;
    }
    return length;
}

/**
 * @brief Change the length of the string in the heap.
 * We use realloc to reuse the same address in the heap each time we change
 * the size of the string. This way we save lots of memory.
 * @param chars the string we change the length of.
 * @param length pointer to the length we will change in the struct.
 * @param newSize the new length to assign to the string.
 *
 * @return MYSTRING_ERROR if the reallocation failed,
 * 			MYSTRING_SUCCESS if the reallocation succeeded.
 */
static MyStringRetVal adjustMyStringLength(char **chars, unsigned long *length,
										   unsigned long newSize)
{
    // Resize the string and check if a problem occurred.
    *chars = (char *) realloc(*chars, newSize);
    if (*chars == NULL)
    {
        return MYSTRING_ERROR;
    }
    // Update the size of the string in the struct.
    *length = newSize;
    return MYSTRING_SUCCESS;
}

/**
 * @brief Get the number of digits of a given integer number.
 * @param n the number we get the digits of.
 *
 * @return the number of digits.
 */
static int getNumOfDigits(int n)
{
    int numOfDigits = 0;
    // Minus sign or zero cases.
    if (n <= 0)
    {
        numOfDigits++;
    }
    // The number is zero with 1 digit.
    if (n == 0)
    {
        return numOfDigits;
    }
    // Go over the number's digits by division to calculate them.
    while (n != 0)
    {
        n = n / DIGIT_DIVIDER;
        numOfDigits++;
    }
    return numOfDigits;
}
/**
 * @brief Wrapper function of a comparator for the qsort function.
 * @param str1
 * @param str2
 * @return The value the default comparator returned (1/0/-1).
 *
 */
static int myStringComparator(const void *str1, const void *str2)
{
	MyString** temp1 = (MyString **) str1;
	MyString** temp2 = (MyString **) str2;
	return myStringCompare(*temp1, *temp2);
}

/**
 * @brief Filter the chars of a string according to a filter and create a new string.
 * @param filteredString the new filtered string.
 * @param unfilteredString the source string we would like to filter.
 * @param filt pointer to a function that filters in some manner.
 *
 */

static void createFilteredString(char *filteredString, const char *unfilteredString,
								 bool (*filt)(const char *))
{

    unsigned long unfilteredStringLength = getCStringLength(unfilteredString);
    unsigned long i = 0;
    for (i = 0; i < unfilteredStringLength; i++)
    {
        if (filt(unfilteredString))
        {
            *filteredString = *unfilteredString;
            filteredString++;
        }
        unfilteredString++;
    }

}

/**
 * @brief Calculate the length of the filtered string to be created. we do this
 * in order to pre-determine the size we need for the string in the heap.
 * @param str the source string we would like to filter.
 * @param filt pointer to a function that filters in some manner.
 * @return the length of the filtered string.
 *
 */
static unsigned long getFilteredStringLength(const char *str, bool (*filt)(const char *))
{
    unsigned long i = 0;
    unsigned long unfilteredLength = getCStringLength(str);
    unsigned long filteredLength = 0;
    for (i = 0; i < unfilteredLength; i++)
    {
        // The char remained after the filtering.
        if (filt(str))
        {
            filteredLength++;
        }
        str++;
    }

    return filteredLength;

}

/**
 * @brief Creates an empty string with length 0 in the heap.
 * @param p_myString The string we alloc.
 * @return Pointer to the chars of the string if we succeeded, NULL otherwise.
 *
 */
static char* emptyStringAlloc(MyString *p_myString)
{
    p_myString -> _chars = (char *) malloc(sizeof(char));
    if (p_myString -> _chars == NULL)
    {
    	return NULL;
    }
    // Assign empty string.
    *(p_myString -> _chars) = END_OF_C_STRING;
    p_myString -> _length = EMPTY_STRING_LENGTH;
    return p_myString -> _chars;
}






#ifndef NDEBUG

static void exitBad(char* testName);
/**
 * @brief Comparator of the reversed order of the default comparator.
 * @param str1
 * @param str2
 * @return 1 if comparator returned -1 and the opposite,
 * 		 0 if it returned 0.
 *
 */
static int reverseMyStringCompare(const MyString *str1, const MyString *str2)
{
	 if (str1 == NULL || str2 == NULL)
	 {
		 return MYSTR_ERROR_CODE;
	 }
	 int result = myStringCompare(str1, str2);
	 if (result == STR1_BIGGER)
	 {
		 return STR2_BIGGER;
	 }
	 if (result == STR2_BIGGER)
	 {
		 return STR1_BIGGER;
	 }
	 return EQUAL_STRINGS;
}

/**
 * @brief Wrapper function of a reversed comparator for the qsort function.
 * @param str1
 * @param str2
 * @return 1 if comparator returned -1 and the opposite,
 * 		 0 if it returned 0.
 *
 */
static int myStringCustomComparator(const void *str1, const void *str2)
{
	MyString** temp1 = (MyString **) str1;
	MyString** temp2 = (MyString **) str2;
	return reverseMyStringCompare(*temp1, *temp2);
}

// ------------------------------ Unit Tests -----------------------------

// ------------------------------ myStringAlloc -----------------------------

static void myStringAllocNormalAllocation()
	{
		MyString* result = myStringAlloc();
		if (result == NULL)
		{
			printf("Expected result : MyString*\n");
			printf("Actual result : NULL\n");
			myStringFree(result);
			exitBad("myStringAllocNormalAllocation");
		}
		printf("PASS\n");
		myStringFree(result);
	}

// ------------------------------ myStringFree -----------------------------

static void myStringFreeNormalFree()
	{
		char *testName = "myStringFreeNormalFree";
		printf("Running %s\n", testName);
		MyString* result = myStringAlloc();
		myStringFree(result);
		result = NULL;
		if (result != NULL)
		{
			printf("Expected result : NULL\n");
			printf("Actual result : MyString*\n");
			exitBad(testName);
		}
		printf("PASS\n");
	}

static void myStringFreeNullFree()
	{
		char *testName = "myStringFreeNullFree";
		printf("Running %s\n", testName);
		MyString* result = NULL;
		myStringFree(result);
		printf("PASS\n");
	}





// ------------------------------ myStringCompare -----------------------------

static void myStringCompareNullCheck()
{
	char *testName = "myStringCompareNullCheck";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();
	int result = myStringCompare(NULL, NULL);
	if (result != MYSTR_ERROR_CODE)
	{
		printf("Expected result : -999\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad(testName);
	}
	result = myStringCompare(NULL, str2);
	if (result != MYSTR_ERROR_CODE)
	{
		printf("Expected result : -999\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad(testName);
	}
	result = myStringCompare(str1, NULL);
	if (result != MYSTR_ERROR_CODE)
	{
		printf("Expected result : -999\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}

static void myStringCompareEmptyCheck()
{
	char *testName = "myStringCompareEmptyCheck";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();
	int result = myStringCompare(str1, str2);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad("myStringCompareEmptyCheck");
	}
	myStringSetFromCString(str1, "Some String");
	result = myStringCompare(str1, str2);
	if (result != 1)
	{
		printf("Expected result : 1\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad("myStringCompareEmptyCheck");
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}

static void myStringCompareNormalCheck()
{
	char *testName = "myStringCompareNormalCheck";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str1, "Some String");
	myStringSetFromCString(str2, "Some String");
	int result = myStringCompare(str1, str2);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad("myStringCompareEmptyCheck");
	}

	myStringSetFromCString(str2, "Some");
	result = myStringCompare(str1, str2);
	if (result != 1)
	{
		printf("Expected result : 1\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad("myStringCompareEmptyCheck");
	}
	myStringSetFromCString(str1, "Som");
	result = myStringCompare(str1, str2);
	if (result != -1)
	{
		printf("Expected result : -1\n");
		printf("Actual result : %d\n", result);
		myStringFree(str1);
		myStringFree(str2);
		exitBad("myStringCompareEmptyCheck");
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}

// ------------------------------ myStringSort -----------------------------

static void myStringSortThreeSort()
{
	char *testName = "myStringSortThreeSort";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();
	MyString *str3 = myStringAlloc();
	myStringSetFromCString(str1, "bbc");
	myStringSetFromCString(str2, "cds");
	myStringSetFromCString(str3, "abc");
	MyString *arr[3] = {str1, str2, str3};
    myStringSort(arr, 3);
    char* cStr1 = myStringToCString(arr[0]);
    char* cStr2 = myStringToCString(arr[1]);
    char* cStr3 = myStringToCString(arr[2]);
    int retVal1 = strcmp(cStr1, "abc");
    int retVal2 = strcmp(cStr2, "bbc");
    int retVal3 = strcmp(cStr3, "cds");
    if (retVal1 != 0 || retVal2 != 0 || retVal3 != 0)
    {
    	printf("Expected result : abc bbc cds\n");
    	printf("Actual result : %s %s %s\n", cStr1, cStr2, cStr3);
    	myStringFree(str1);
    	myStringFree(str2);
    	myStringFree(str3);
    	exitBad(testName);
    }
    printf("PASS\n");
    free(cStr1);
    free(cStr2);
    free(cStr3);
    cStr1 = NULL;
    cStr2 = NULL;
    cStr3 = NULL;
    myStringFree(str1);
    myStringFree(str2);
    myStringFree(str3);
}

// ------------------------------ myStringCustomSort -----------------------------

static void myStringCustomSortThreeSort()
{
	char *testName = "myStringCustomSortThreeSort";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();
	MyString *str3 = myStringAlloc();
	myStringSetFromCString(str1, "bbc");
	myStringSetFromCString(str2, "cds");
	myStringSetFromCString(str3, "abc");
	MyString *arr[3] = {str1, str2, str3};
    myStringCustomSort(arr, 3, myStringCustomComparator);
    char* cStr1 = myStringToCString(arr[0]);
    char* cStr2 = myStringToCString(arr[1]);
    char* cStr3 = myStringToCString(arr[2]);
    int retVal1 = strcmp(cStr1, "cds");
    int retVal2 = strcmp(cStr2, "bbc");
    int retVal3 = strcmp(cStr3, "abc");
    if (retVal1 != 0 || retVal2 != 0 || retVal3 != 0)
    {
    	printf("Expected result : cds bbc abc\n");
    	printf("Actual result : %s %s %s\n", cStr1, cStr2, cStr3);
    	myStringFree(str1);
    	myStringFree(str2);
    	myStringFree(str3);
    	exitBad(testName);
    }
    printf("PASS\n");
    free(cStr1);
    free(cStr2);
    free(cStr3);
    cStr1 = NULL;
    cStr2 = NULL;
    cStr3 = NULL;
    myStringFree(str1);
    myStringFree(str2);
    myStringFree(str3);
}

// ------------------------------ myStringWrite -----------------------------

static void myStringWriteNormal()
{
	char *testName = "myStringWriteNormal";
	printf("Running %s\n", testName);
	FILE *stream = fopen("/cs/stud/orib/Desktop/test", "w");
	if (stream == NULL)
	{
		exit(MYSTR_ERROR_CODE);
	}
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "abcdefghijklmnop");
	MyStringRetVal retVal = myStringWrite(str, stream);
	if (retVal == MYSTRING_ERROR)
	{
		printf("Expected result : 0\n");
		printf("Actual result : -1\n");
		fclose(stream);
		myStringFree(str);
		exitBad(testName);
	}

    printf("PASS\n");
    myStringFree(str);
}

// ------------------------------ myStringClone -----------------------------

static void myStringCloneNormal()
{
	char *testName = "myStringCloneNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "Hey there");
	MyString *clonedStr = myStringClone(str);
	char *clonedChars = myStringToCString(clonedStr);
	char *oldChars = myStringToCString(str);
	int result = strcmp(clonedChars, oldChars);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		myStringFree(str);
		myStringFree(clonedStr);
		exitBad(testName);
	}
	printf("PASS\n");
	free(clonedChars);
	free(oldChars);
	clonedChars = NULL;
	oldChars = NULL;
	myStringFree(str);
	myStringFree(clonedStr);
}

// ------------------------------ myStringSetFromMyString -----------------------------
static void myStringSetFromMyStringEmpty()
{
	char *testName = "myStringSetFromMyStringEmpty";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str2, "Hey there");
	myStringSetFromMyString(str1, str2);
	char *newChars = myStringToCString(str1);
	char *oldChars = myStringToCString(str2);
	int result = strcmp(newChars, oldChars);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : -1\n");
		myStringFree(str1);
		myStringFree(str2);
		exitBad(testName);
	}
	printf("PASS\n");
	free(newChars);
	free(oldChars);
	newChars = NULL;
	oldChars = NULL;
	myStringFree(str1);
	myStringFree(str2);
}

static void myStringSetFromMyStringNormal()
{
	char *testName = "myStringSetFromMyStringNormal";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	MyString *str2 = myStringAlloc();

	int retVal = myStringSetFromMyString(str1, str2);
	if (retVal != MYSTRING_SUCCESS)
	{
		printf("Expected result : 0\n");
		printf("Actual result : -1\n");
		exitBad(testName);
	}
	printf("PASS\n");

	myStringFree(str1);
	myStringFree(str2);
}

// ------------------------------ myStringFilter -----------------------------

static bool filt(const char *c)
{
	if (*c < 'g' && 'a' < *c)
	{
		return true;
	}
	return false;
}

static void myStringFilterNormal()
{
	char *testName = "myStringFilterNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "abcz");
	myStringFilter(str, filt);
	char *newChars = myStringToCString(str);
	int result = strcmp(newChars, "bc");
	if (result != 0)
	{
		printf("Expected result : bc\n");
		printf("Actual result : %s\n", newChars);
		myStringFree(str);
		exitBad(testName);
	}
	printf("PASS\n");
	free(newChars);
	newChars = NULL;
	myStringFree(str);
}
// ------------------------------ myStringSetFromCString -----------------------------

static void myStringSetFromCStringNormal()
{
	char *testName = "myStringSetFromCStringNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "abcde");
	char *newChars = myStringToCString(str);
	int result = strcmp(newChars, "abcde");
	if (result != 0)
	{
		printf("Expected result : abcde\n");
		printf("Actual result : %s\n", newChars);
		exitBad(testName);
	}
	printf("PASS\n");
	free(newChars);
	newChars = NULL;
	myStringFree(str);
}

// ------------------------------ myStringSetFromInt -----------------------------
static void myStringSetFromIntNormal()
{
	char *testName = "myStringSetFromIntNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromInt(str, 123456789);
	char *chars = myStringToCString(str);
	int result = strcmp(chars, "123456789");
	if (result != 0)
	{
		printf("Expected result : 123456789\n");
		printf("Actual result : %s\n", chars);
		exitBad(testName);
	}
	free(chars);
	myStringSetFromInt(str, -123456789);
	chars = myStringToCString(str);
	result = strcmp(chars, "-123456789");
	if (result != 0)
	{
		printf("Expected result : -123456789\n");
		printf("Actual result : %s\n", chars);
		myStringFree(str);
		exitBad(testName);
	}
	printf("PASS\n");
	free(chars);
	chars = NULL;
	myStringFree(str);
}

// ------------------------------ myStringToInt -----------------------------
static void myStringToIntNormal()
{
	char *testName = "myStringToIntNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "123456");
	int num = myStringToInt(str);
	if (num != 123456)
	{
		printf("Expected result : 123456\n");
		printf("Actual result : %d\n", num);
		exitBad(testName);
	}
	myStringSetFromCString(str, "-123456");
	num = myStringToInt(str);
	if (num != -123456)
	{
		printf("Expected result : -123456\n");
		printf("Actual result : %d\n", num);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str);
}

// ------------------------------ myStringToCString -----------------------------

static void myStringToCStringNormal()
{
	char *testName = "myStringToCStringNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "Hey there");
	char* cStr = myStringToCString(str);
	int result = strcmp(cStr, "Hey there");
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");
	free(cStr);
	cStr = NULL;
	myStringFree(str);
}

// ------------------------------ myStringCat -----------------------------

static void myStringCatNormal()
{
	char *testName = "myStringCatNormal";
	printf("Running %s\n", testName);
	MyString *dest = myStringAlloc();
	myStringSetFromCString(dest, "Hey there");
	MyString *src = myStringAlloc();
	myStringSetFromCString(src, " Delilah don't you worry about the distance");
	int result = myStringCat(dest, src);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");

	myStringFree(dest);
	myStringFree(src);
}

// ------------------------------ myStringCatTo -----------------------------

static void myStringCatToNormal()
{
	char *testName = "myStringCatToNormal";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	myStringSetFromCString(str1, "Hey there");
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str2, " Delilah don't you worry about the distance");
	MyString *str3 = myStringAlloc();
	int result = myStringCatTo(str1, str2, str3);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");

	myStringFree(str1);
	myStringFree(str2);
	myStringFree(str3);
}
// ------------------------------ myStringCompare -----------------------------
static void myStringCompareNormal()
{
	char *testName = "myStringCompare";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	myStringSetFromCString(str1, "abc");
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str2, "abcd");
	int result = myStringCompare(str1, str2);
	if (result != STR2_BIGGER)
	{
		printf("Expected result : -1\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "ab");
	result = myStringCompare(str1, str2);
	if (result != STR1_BIGGER)
	{
		printf("Expected result : 1\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "abc");
	result = myStringCompare(str1, str2);
	if (result != EQUAL_STRINGS)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}
// ------------------------------ myStringCustomCompare -----------------------------

static void myStringCustomCompareNormal()
{
	char *testName = "myStringCustomCompareNormal";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	myStringSetFromCString(str1, "abc");
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str2, "abcd");
	int result = myStringCustomCompare(str1, str2, reverseMyStringCompare);
	if (result != STR1_BIGGER)
	{
		printf("Expected result : 1\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "ab");
	result = myStringCustomCompare(str1, str2, reverseMyStringCompare);
	if (result != STR2_BIGGER)
	{
		printf("Expected result : -1\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "abc");
	result = myStringCustomCompare(str1, str2, reverseMyStringCompare);
	if (result != EQUAL_STRINGS)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}
// ------------------------------ myStringEqual -----------------------------

static void myStringEqualNormal()
{
	char *testName = "myStringEqualNormal";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	myStringSetFromCString(str1, "abc");
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str2, "abcd");
	int result = myStringEqual(str1, str2);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "ab");
	result = myStringEqual(str1, str2);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "abc");
	result = myStringEqual(str1, str2);
	if (result != 1)
	{
		printf("Expected result : 1\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}

// ------------------------------ myStringCustomEqual -----------------------------

static void myStringCustomEqualNormal()
{
	char *testName = "myStringCustomEqualNormal";
	printf("Running %s\n", testName);
	MyString *str1 = myStringAlloc();
	myStringSetFromCString(str1, "abc");
	MyString *str2 = myStringAlloc();
	myStringSetFromCString(str2, "abcd");
	int result = myStringCustomEqual(str1, str2, reverseMyStringCompare);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "ab");
	result = myStringCustomEqual(str1, str2, reverseMyStringCompare);
	if (result != 0)
	{
		printf("Expected result : 0\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	myStringSetFromCString(str2, "abc");
	result = myStringCustomEqual(str1, str2, reverseMyStringCompare);
	if (result != 1)
	{
		printf("Expected result : 1\n");
		printf("Actual result : %d\n", result);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str1);
	myStringFree(str2);
}
// ------------------------------ myStringMemUsage -----------------------------

static void myStringMemUsageNormal()
{
	char *testName = "myStringMemUsageNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "abcde");
	unsigned long result = myStringMemUsage(str);
	if (result != 21)
	{
		printf("Expected result : 21\n");
		printf("Actual result : %lu\n", result);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str);
}

// ------------------------------ myStringLen -----------------------------

static void myStringLenNormal()
{
	char *testName = "myStringLenNormal";
	printf("Running %s\n", testName);
	MyString *str = myStringAlloc();
	myStringSetFromCString(str, "abcde");
	unsigned long length = myStringLen(str);
	if (length != 5)
	{
		printf("Expected result : 5\n");
		printf("Actual result : %lu\n", length);
		exitBad(testName);
	}
	printf("PASS\n");
	myStringFree(str);
}



int main()
{

	printf("Testing myStringAlloc:\n");
	myStringAllocNormalAllocation();
	printf("Testing myStringFree:\n");
	myStringFreeNormalFree();
	myStringFreeNullFree();
	printf("Testing myStringSetFromCString:\n");
	myStringSetFromCStringNormal();
	printf("Testing myStringClone:\n");
	myStringCloneNormal();
	printf("Testing myStringSetFromMyString:\n");
	myStringSetFromMyStringEmpty();
	myStringSetFromMyStringNormal();
	printf("Testing myStringSetFromIntNormal:\n");
	myStringSetFromIntNormal();
	printf("Testing myStringToIntNormal:\n");
	myStringToIntNormal();
	printf("Testing myStringFilter:\n");
	myStringFilterNormal();
	////////TODO/////////////
	printf("Testing myStringCat:\n");
	myStringCatNormal();
	printf("Testing myStringCatTo:\n");
	myStringCatToNormal();
	printf("Testing myStringToCString:\n");
	myStringToCStringNormal();
	printf("Testing myStringCompare:\n");
	myStringCompareNormal();
	printf("Testing myStringCustomCompare:\n");
	myStringCustomCompareNormal();
	printf("Testing myStringEqual:\n");
	myStringEqualNormal();
	printf("Testing myStringCustomEqual:\n");
	myStringCustomEqualNormal();
	printf("Testing myStringMemUsage:\n");
	myStringMemUsageNormal();
	printf("Testing myStringLen:\n");
	myStringLenNormal();
	printf("Testing myStringWrite:\n");
	myStringWriteNormal();
	printf("Testing myStringCompare:\n");
	myStringCompareNullCheck();
	myStringCompareEmptyCheck();
	myStringCompareNormalCheck();
	printf("Testing myStringSort:\n");
	myStringSortThreeSort();
	printf("Testing myStringCustomSort:\n");
	myStringCustomSortThreeSort();

	return 0;

}

static void exitBad(char* testName)
{
   printf("Error in test %s.\n", testName);
   fprintf(stderr, "%s", "ERROR");
   exit(EXIT_FAILURE);
}




#endif
