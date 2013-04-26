// request should be of the format GET /X<value>_Y<value>_Z<value>.bmp

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void getValuesFromConnectionString(char* connectionString,
                                    double* x, double* y, int* z);
double readDoubleFromString(char* string);
int readIntFromString(char* string);

int main(int argc, char* argv[]) {
    char testString[] = "GET localhost:1917/X-0.044921875_Y-0.0869140625_Z10.bmp HTTP/1.1";

    double x, y;
    int z;

    getValuesFromConnectionString(testString, &x, &y, &z);

    printf("%s\n", testString);
    printf("x: %lf\n", x);
    printf("y: %lf\n", y);
    printf("z: %d\n", z);

    return EXIT_SUCCESS;
}

void getValuesFromConnectionString(char* connectionString,
                                    double* x, double* y, int* z) {
    int index = 0;

    // Skip the GET command and the hostname
    while (connectionString[index] != '/' &&
              connectionString[index] != '\0') {
        index++;
    }

    // Skip to the character after the X
    while (connectionString[index] != 'X' &&
              connectionString[index] != '\0') {
        index++;
    }
    index++;

    // Read the X value
    *x = readDoubleFromString(&connectionString[index]);


    // Skip to the character after the Y
    while (connectionString[index] != 'Y' &&
              connectionString[index] != '\0') {
        index++;
    }
    index++;

    // Read the Y value
    *y = readDoubleFromString(&connectionString[index]);


    // Skip to the character after the Z
    while (connectionString[index] != 'Z' &&
              connectionString[index] != '\0') {
        index++;
    }
    index++;

    // Read the Z value
    *z = readIntFromString(&connectionString[index]);
}

double readDoubleFromString(char* str) {
    int index = 0;
    double value = 0;
    int afterDecimalPoint = FALSE;
    int moreThanOneDecimalPoint = FALSE;
    int isNegative = FALSE;
    int digitsAfterDecimalPoint = 0;
    int digitCounter;

    // The value is negative if the first character is a minus
    if (str[index] == '-') {
        isNegative = TRUE;
        index++;
    }

    // Read until end of string or non-numerical character.
    // The first decimal point counts as a numerical character.
    while (str[index] != '\0' && moreThanOneDecimalPoint == FALSE &&
              ((str[index] >= '0' && str[index] <= '9') ||
                  str[index] == '.'
              )
          ) {

        if (str[index] == '.') {
            if (afterDecimalPoint == TRUE) {
                moreThanOneDecimalPoint = TRUE;
            } else {
                afterDecimalPoint = TRUE;
            }
        } else {
            if (afterDecimalPoint == FALSE) {
                // Shift previous digits left one to add new one
                // to ones column
                value *= 10;
                value += str[index] - '0';
            } else {
                // Shift previous digits left before the decimal
                // point to add new one
                // to ones column, then shift all right one to
                // move it after the decimal point
                digitCounter = 0;
                while (digitCounter <= digitsAfterDecimalPoint) {
                    value *= 10;
                    digitCounter++;
                }
                
                value += str[index] - '0';
                
                while (digitCounter > 0) {
                    value /= 10;
                    digitCounter--;
                }
                digitsAfterDecimalPoint++;
                printf("%0.10lf\n", value);
            }
        }

        index++;
    }

    if (isNegative == TRUE) {
        value = -value;
    }

    return value;
}

int readIntFromString(char* str) {
    int index = 0;
    int value = 0;
    int isNegative = FALSE;

    // The value is negative if the first character is a minus
    if (str[index] == '-') {
        isNegative = TRUE;
        index++;
    }

    // Read until end of string or non-numerical character
    while (str[index] != '\0' &&
              ((str[index] >= '0' && str[index] <= '9'))
          ) {

        // Shift previous digits left one to add new one
        // to ones column
        value *= 10;
        value += str[index] - '0';

        index++;
    }

    if (isNegative == TRUE) {
        value = -value;
    }

    return value;
}
