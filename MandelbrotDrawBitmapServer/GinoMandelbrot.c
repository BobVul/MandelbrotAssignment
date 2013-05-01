/*
* bmpServer.c
* 1917 serve that 3x3 bmp from lab3 Image activity
*
* Created by Tim Lambert on 02/04/12.
* Containing code created by Richard Buckland on 28/01/11.
* Copyright 2012 Licensed under Creative Commons SA-BY-NC 3.0.
*
*/

/* LOCATIONS TO CONSIDER:
 * http://localhost:1917/X-1.7384986877441406_Y0.000003814697265625_Z21.bmp
 * http://localhost:1917/X-1.7381986877441406_Y0.000003814697265625_Z21.bmp
 * http://localhost:1917/X0.14359283447265625_Y0.6518898010253906_Z18.bmp
 * http://localhost:1917/X0.26531982421875_Y-0.0005340576171875_Z15.bmp
 * http://localhost:1917/X-1.4473838806152344_Y-0.000102996826171875_Z18.bmp
 */
 
 
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mandelbrot.h"
#include "pixelColor.h"

#define TRUE 1
#define FALSE 0

#define MAX_ITERATIONS 256

#define BITMAP_HEIGHT 512
// Because the width * bytes per pixel is divisible by 4, we don't
// have to worry about rounding up.
#define BITMAP_WIDTH 512
#define BITMAP_BYTES_PER_PIXEL 3

int waitForConnection (int serverSocket);
int makeServerSocket (int portno);
void serveViewer(int socket);
void serveBMP (int socket, double centreX, double centreY, int zoom);
void generateBitmapImage(unsigned char bmpData[],
                         double centreX, double centreY, int zoom);
// dealing with non-integer exponents is difficult and unnecessary
double power(int base, int exponent);

int getValuesFromConnectionString(char* connectionString,
                                    double* x, double* y, int* z);
double readDoubleFromString(char* string);
int readIntFromString(char* string);
static void testEscapeSteps(void);

#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 1917
#define NUMBER_OF_PAGES_TO_SERVE 1000000
// after serving this many pages the server will halt

int main (int argc, char *argv[]) {

   printf ("\nTesting mandelbrot.c\n");
   testEscapeSteps();
   printf ("All tests passed.  You are Awesome!\n\n");
   
   
    printf ("************************************\n");
    printf ("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
    printf ("Serving bmps since 2012\n");
    int serverSocket = makeServerSocket (DEFAULT_PORT);
    printf ("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
    printf ("************************************\n");
    char request[REQUEST_BUFFER_SIZE];
    int numberServed = 0;
    while (numberServed < NUMBER_OF_PAGES_TO_SERVE) {
        printf ("*** So far served %d pages ***\n", numberServed);
        int connectionSocket = waitForConnection (serverSocket);
        // wait for a request to be sent from a web browser, open a new
        // connection for this conversation
        // read the first line of the request sent by the browser
        int bytesRead;
        bytesRead = read (connectionSocket, request, (sizeof request)-1);
        assert (bytesRead >= 0);
        // were we able to read any data from the connection?
        // print entire request to the console
        printf (" *** Received http request ***\n %s\n", request);
        //send the browser a simple html page using http
        printf (" *** Sending http response ***\n");

        double x, y;
        int z;
        if (getValuesFromConnectionString(request, &x, &y, &z) == TRUE) {
            serveBMP(connectionSocket, x, y, z);
        } else {
            serveViewer(connectionSocket);
        }

        // close the connection after sending the page- keep aust beautiful
        close(connectionSocket);
        numberServed++;
    }
    // close the server connection after we are done- keep aust beautiful
    printf ("** shutting down the server **\n");
    close (serverSocket);
    return EXIT_SUCCESS;
}

void serveViewer(int socket) {
    char* message = "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<!DOCTYPE html>"
        "<script src=\"https://almondbread.openlearning.com/tileviewer.js\"></script>";

    write(socket, message, strlen(message));
}

void serveBMP (int socket, double centreX, double centreY, int zoom) {
    char* message;
    // first send the http response header
    // (if you write stings one after another like this on separate
    // lines the c compiler kindly joins them togther for you into
    // one long string)
    message = "HTTP/1.0 200 OK\r\n"
              "Content-Type: image/bmp\r\n"
              "\r\n";
    printf ("about to send=> %s\n", message);
    write (socket, message, strlen (message));

    // Send the header for a bitmap of size 512x512
    unsigned char bmpHeader[] = {
        0x42, 0x4D, 0x36, 0x00, 0x0C, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x01, 0x00, 0x18, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x13, 0x0B,
        0x00, 0x00, 0x13, 0x0B, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    write (socket, bmpHeader, sizeof(bmpHeader));

    // Generate image data
    int bitmapSize;
    bitmapSize = BITMAP_HEIGHT * BITMAP_WIDTH;
    bitmapSize *= BITMAP_BYTES_PER_PIXEL;
    unsigned char bmpData[bitmapSize];
    generateBitmapImage(bmpData, centreX, centreY, zoom);

    write (socket, bmpData, bitmapSize);
}


// start the server listening on the specified port number
int makeServerSocket (int portNumber) {
    // create socket
    int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
    assert (serverSocket >= 0);
    // error opening socket
    // bind socket to listening port
    struct sockaddr_in serverAddress;
    bzero ((char *) &serverAddress, sizeof (serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons (portNumber);
    // let the server start immediately after a previous shutdown
    int optionValue = 1;
    setsockopt (
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &optionValue,
        sizeof(int)
    );

    int bindSuccess =
        bind (
            serverSocket,
            (struct sockaddr *) &serverAddress,
            sizeof (serverAddress)
        );
    assert (bindSuccess >= 0);
    // if this assert fails wait a short while to let the operating
    // system clear the port before trying again
    return serverSocket;
}

// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
int waitForConnection (int serverSocket) {
    // listen for a connection
    const int serverMaxBacklog = 10;
    listen (serverSocket, serverMaxBacklog);
    // accept the connection
    struct sockaddr_in clientAddress;
    socklen_t clientLen = sizeof (clientAddress);
    int connectionSocket =
        accept (
            serverSocket,
            (struct sockaddr *) &clientAddress,
            &clientLen
        );
    assert (connectionSocket >= 0);
    // error on accept
    return (connectionSocket);
}

void generateBitmapImage(unsigned char bmpData[],
                         double centreX, double centreY, int zoom) {
    int drawingX;
    int drawingY;

    // The index of the desired row
    int bitmapRow;
    // The index of the desired column
    int bitmapColumn;

    double scaledX, scaledY;
    double scale;

    int iterations;

    scale = 1 / power(2, zoom);

    drawingY = 0;
    while (drawingY < BITMAP_HEIGHT) {
        drawingX = 0;
        while (drawingX < BITMAP_WIDTH) {
            // centering
            scaledX = centreX -(BITMAP_WIDTH / 2) * scale;
            scaledY = centreY -(BITMAP_HEIGHT / 2) * scale;

            // calculate current point
            scaledX += drawingX * scale;
            scaledY += drawingY * scale;

  		// generate pixel colouring data
            bitmapRow = drawingY *
                            (BITMAP_WIDTH * BITMAP_BYTES_PER_PIXEL);
            bitmapColumn = drawingX * BITMAP_BYTES_PER_PIXEL;
            iterations = escapeSteps(scaledX, scaledY);
            bmpData[bitmapRow + bitmapColumn] = stepsToBlue(iterations);
            bmpData[bitmapRow + bitmapColumn + 1] = stepsToGreen(iterations);;
            bmpData[bitmapRow + bitmapColumn + 2] = stepsToRed(iterations);;

            drawingX++;
        }
        drawingY++;
    }
}

double power(int base, int exponent) {
	// function to calculate powers given an exponent
    int i = 0;
    double calcResult = 1;
    while (i < exponent) {
        calcResult *= base;
        i++;
    }

    return calcResult;
}

int getValuesFromConnectionString(char* connectionString,
                                    double* x, double* y, int* z) {
    int index = 0;
    int isXSet = FALSE;
    int isYSet = FALSE;
    int isZSet = FALSE;

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
    // If it didn't stop on null
    if (connectionString[index] == 'X') {
        isXSet = TRUE;
        index++;
        // Read the X value
        *x = readDoubleFromString(&connectionString[index]);
    }

    // Skip to the character after the Y
    while (connectionString[index] != 'Y' &&
              connectionString[index] != '\0') {
        index++;
    }
    // If it didn't stop on null
    if (connectionString[index] == 'Y') {
        isYSet = TRUE;
        index++;
        // Read the Y value
        *y = readDoubleFromString(&connectionString[index]);
    }


    // Skip to the character after the Z
    while (connectionString[index] != 'Z' &&
              connectionString[index] != '\0') {
        index++;
    }
    // If it didn't stop on null
    if (connectionString[index] == 'Z') {
        isZSet = TRUE;
        index++;
        // Read the Z value
        *z = readIntFromString(&connectionString[index]);
    }

    return isXSet && isYSet && isZSet;
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

int escapeSteps (double x, double y) {

    int iteration;
    double calcX, calcY, calcTempX;

    calcX = 0;
    calcY = 0;
    iteration = 0;
    while ((iteration < MAX_ITERATIONS) &&
            (calcX * calcX + calcY * calcY < 2 * 2)) {
        calcTempX = (calcX * calcX - calcY * calcY) + x;
        calcY = (2 * calcX * calcY) + y;
        calcX = calcTempX;

        iteration++;
    }

    return iteration;
}

static void testEscapeSteps(void) {
   printf (" Testing escapeTests ...\n");
   printf (
     "  (using a few sample unit tests only - the automarker "
     "will use more tests)\n"
   );
 
   assert (escapeSteps (100.0, 100.0) == 1);
   assert (escapeSteps (0.0, 0.0)     == 256);
 
   assert (escapeSteps (-1.5000000000000, -1.5000000000000) == 1);
   assert (escapeSteps (-1.4250000000000, -1.4250000000000) == 1);
   assert (escapeSteps (-1.3500000000000, -1.3500000000000) == 2);
   assert (escapeSteps (-1.2750000000000, -1.2750000000000) == 2);
   assert (escapeSteps (-1.2000000000000, -1.2000000000000) == 2);
   assert (escapeSteps (-1.1250000000000, -1.1250000000000) == 3);
   assert (escapeSteps (-1.0500000000000, -1.0500000000000) == 3);
   assert (escapeSteps (-0.9750000000000, -0.9750000000000) == 3);
   assert (escapeSteps (-0.9000000000000, -0.9000000000000) == 3);
   assert (escapeSteps (-0.8250000000000, -0.8250000000000) == 4);
   assert (escapeSteps (-0.7500000000000, -0.7500000000000) == 4);
   assert (escapeSteps (-0.6750000000000, -0.6750000000000) == 6);
   assert (escapeSteps (-0.6000000000000, -0.6000000000000) == 12);
   assert (escapeSteps (-0.5250000000000, -0.5250000000000) == 157);
   assert (escapeSteps (-0.4500000000000, -0.4500000000000) == 256);
   assert (escapeSteps (-0.3750000000000, -0.3750000000000) == 256);
   assert (escapeSteps (-0.3000000000000, -0.3000000000000) == 256);
   assert (escapeSteps (-0.2250000000000, -0.2250000000000) == 256);
   assert (escapeSteps (-0.1500000000000, -0.1500000000000) == 256);
   assert (escapeSteps (-0.0750000000000, -0.0750000000000) == 256);
   assert (escapeSteps (-0.0000000000000, -0.0000000000000) == 256);
 
   assert (escapeSteps (-0.5400000000000, 0.5600000000000) == 256);
   assert (escapeSteps (-0.5475000000000, 0.5650000000000) == 58);
   assert (escapeSteps (-0.5550000000000, 0.5700000000000) == 28);
   assert (escapeSteps (-0.5625000000000, 0.5750000000000) == 22);
   assert (escapeSteps (-0.5700000000000, 0.5800000000000) == 20);
   assert (escapeSteps (-0.5775000000000, 0.5850000000000) == 15);
   assert (escapeSteps (-0.5850000000000, 0.5900000000000) == 13);
   assert (escapeSteps (-0.5925000000000, 0.5950000000000) == 12);
   assert (escapeSteps (-0.6000000000000, 0.6000000000000) == 12);
 
   assert (escapeSteps (0.2283000000000, -0.5566000000000) == 20);
   assert (escapeSteps (0.2272500000000, -0.5545000000000) == 19);
   assert (escapeSteps (0.2262000000000, -0.5524000000000) == 19);
   assert (escapeSteps (0.2251500000000, -0.5503000000000) == 20);
   assert (escapeSteps (0.2241000000000, -0.5482000000000) == 20);
   assert (escapeSteps (0.2230500000000, -0.5461000000000) == 21);
   assert (escapeSteps (0.2220000000000, -0.5440000000000) == 22);
   assert (escapeSteps (0.2209500000000, -0.5419000000000) == 23);
   assert (escapeSteps (0.2199000000000, -0.5398000000000) == 26);
   assert (escapeSteps (0.2188500000000, -0.5377000000000) == 256);
   assert (escapeSteps (0.2178000000000, -0.5356000000000) == 91);
   assert (escapeSteps (0.2167500000000, -0.5335000000000) == 256);
 
   assert (escapeSteps (-0.5441250000000, 0.5627500000000) == 119);
   assert (escapeSteps (-0.5445000000000, 0.5630000000000) == 88);
   assert (escapeSteps (-0.5448750000000, 0.5632500000000) == 83);
   assert (escapeSteps (-0.5452500000000, 0.5635000000000) == 86);
   assert (escapeSteps (-0.5456250000000, 0.5637500000000) == 74);
   assert (escapeSteps (-0.5460000000000, 0.5640000000000) == 73);
   assert (escapeSteps (-0.5463750000000, 0.5642500000000) == 125);
   assert (escapeSteps (-0.5467500000000, 0.5645000000000) == 75);
   assert (escapeSteps (-0.5471250000000, 0.5647500000000) == 60);
   assert (escapeSteps (-0.5475000000000, 0.5650000000000) == 58);
 
   assert (escapeSteps (0.2525812510000, 0.0000004051626) == 60);
   assert (escapeSteps (0.2524546884500, 0.0000004049095) == 61);
   assert (escapeSteps (0.2523281259000, 0.0000004046564) == 63);
   assert (escapeSteps (0.2522015633500, 0.0000004044033) == 65);
   assert (escapeSteps (0.2520750008000, 0.0000004041502) == 67);
   assert (escapeSteps (0.2519484382500, 0.0000004038971) == 69);
   assert (escapeSteps (0.2518218757000, 0.0000004036441) == 72);
   assert (escapeSteps (0.2516953131500, 0.0000004033910) == 74);
   assert (escapeSteps (0.2515687506000, 0.0000004031379) == 77);
   assert (escapeSteps (0.2514421880500, 0.0000004028848) == 81);
   assert (escapeSteps (0.2513156255000, 0.0000004026317) == 85);
   assert (escapeSteps (0.2511890629500, 0.0000004023786) == 89);
   assert (escapeSteps (0.2510625004000, 0.0000004021255) == 94);
   assert (escapeSteps (0.2509359378500, 0.0000004018724) == 101);
   assert (escapeSteps (0.2508093753000, 0.0000004016193) == 108);
   assert (escapeSteps (0.2506828127500, 0.0000004013662) == 118);
   assert (escapeSteps (0.2505562502000, 0.0000004011132) == 131);
   assert (escapeSteps (0.2504296876500, 0.0000004008601) == 150);
   assert (escapeSteps (0.2503031251000, 0.0000004006070) == 179);
   assert (escapeSteps (0.2501765625500, 0.0000004003539) == 235);
   assert (escapeSteps (0.2500500000000, 0.0000004001008) == 256);
 
   assert (escapeSteps (0.3565670191423, 0.1094322101123) == 254);
   assert (escapeSteps (0.3565670191416, 0.1094322101120) == 255);
   assert (escapeSteps (0.3565670191409, 0.1094322101118) == 256);
   assert (escapeSteps (0.3565670950000, 0.1094322330000) == 222);
   assert (escapeSteps (0.3565670912300, 0.1094322318625) == 222);
   assert (escapeSteps (0.3565670874600, 0.1094322307250) == 222);
   assert (escapeSteps (0.3565670836900, 0.1094322295875) == 222);
   assert (escapeSteps (0.3565670799200, 0.1094322284500) == 222);
   assert (escapeSteps (0.3565670761500, 0.1094322273125) == 222);
   assert (escapeSteps (0.3565670723800, 0.1094322261750) == 222);
   assert (escapeSteps (0.3565670686100, 0.1094322250375) == 223);
   assert (escapeSteps (0.3565670648400, 0.1094322239000) == 223);
   assert (escapeSteps (0.3565670610700, 0.1094322227625) == 224);
   assert (escapeSteps (0.3565670573000, 0.1094322216250) == 225);
   assert (escapeSteps (0.3565670535300, 0.1094322204875) == 256);
   assert (escapeSteps (0.3565670497600, 0.1094322193500) == 256);
   assert (escapeSteps (0.3565670459900, 0.1094322182125) == 237);
   assert (escapeSteps (0.3565670422200, 0.1094322170750) == 233);
   assert (escapeSteps (0.3565670384500, 0.1094322159375) == 232);
   assert (escapeSteps (0.3565670346800, 0.1094322148000) == 232);
   assert (escapeSteps (0.3565670309100, 0.1094322136625) == 232);
   assert (escapeSteps (0.3565670271400, 0.1094322125250) == 233);
   assert (escapeSteps (0.3565670233700, 0.1094322113875) == 234);
   assert (escapeSteps (0.3565670196000, 0.1094322102500) == 243);
 
 
   printf (" ... escapeSteps tests passed!\n");
}
