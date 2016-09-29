.PHONY: clean main tests myString

CC = c99
OBJECTS = -Wvla -Wall -Wextra -g -lm
REMOVE_FILES = MyStringMain MyString test.out *.a *.o

myString: MyString.c MyString.h 
	  $(CC) -c $(OBJECTS) -DNDEBUG MyString.c -o MyString.o 
	  ar rcs libmyString.a MyString.o

tests: MyString.h 
	$(CC) $(OBJECTS) MyString.c -o MyString MyString 

main: myString 
	$(CC) -c $(OBJECTS) MyStringMain.c -o MyStringMain.o
	$(CC) MyStringMain.o -lm -L. libmyString.a -o MyStringMain MyStringMain 

clean: 
	rm -f $(REMOVE_FILES)

