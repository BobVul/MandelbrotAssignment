/*
*  simpleServer.c
*  1917 lab 5
*
*  Created by Richard Buckland on 28/01/11.
*  Copyright 2011 Licensed under Creative Commons SA-BY-NC 3.0.
*
*/
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #define close closesocket
#else
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#include "mandelbrot.h"

#define DRAWING_WIDTH 80
#define DRAWING_HEIGHT 72
#define DRAWING_ZOOM 5
#define MAX_ITERATIONS 256
 
int waitForConnection (int serverSocket);
int makeServerSocket (int portno);
void serveHTML (int socket);

// destinationString should be of size
// ((DRAWING_WIDTH * 2 + 1) * DRAWING_HEIGHT) + 1
// to account for line breaks and the null character
// Note that width must be multiplied by two since we
// are using double asterisks for better spacing.
void drawMandelbrot (char* destinationString);

// dealing with non-integer exponents is difficult and unecessary
double power(int base, int exponent);
 
#define SIMPLE_SERVER_VERSION 1.0
#define REQUEST_BUFFER_SIZE 1000
#define DEFAULT_PORT 7191
#define NUMBER_OF_PAGES_TO_SERVE 10
// after serving this many pages the server will halt
 
int main (int argc, char* argv[]) {
 
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup((2 << 8) + 2, &wsaData);
#endif
 
    puts("************************************");
    printf("Starting simple server %f\n", SIMPLE_SERVER_VERSION);
    puts("Serving poetry since 2011");
 
    int serverSocket = makeServerSocket(DEFAULT_PORT);
    printf("Access this server at http://localhost:%d/\n", DEFAULT_PORT);
    puts("************************************");
 
    char request[REQUEST_BUFFER_SIZE];
    int numberServed = 0;
    while ( numberServed < NUMBER_OF_PAGES_TO_SERVE) {
        printf("*** So far served %d pages ***\n", numberServed);
 
        int connectionSocket = waitForConnection(serverSocket);
        // wait for a request to be sent from a web browser, open a new
        // connection for this conversation
 
        // read the first line of the request sent by the browser
        int bytesRead = recv (connectionSocket, request, sizeof(request) - 1, 0);
        assert (bytesRead >= 0);
        // were we able to read any data from the connection?
 
        // print entire request to the console
        printf (" *** Received http request ***\n %s\n", request);
 
        //send the browser a simple html page using http
        puts (" *** Sending http response ***");
        serveHTML (connectionSocket);
 
        // close the connection after sending the page- keep aust beautiful
        close (connectionSocket);
        ++numberServed;
    }
 
    // close the server connection after we are done- keep aust beautiful
    puts("** shutting down the server **");
    close (serverSocket);
 
#ifdef _WIN32
    WSACleanup();
#endif
 
    return EXIT_SUCCESS;
}
 
void serveHTML(int socket) {
    const char* htmlBefore =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<!DOCTYPE html>\n"
        "<html>\n"
        " <title>Current Short Message</title>\n"
        " <body><pre>";
        
    char mandelbrotDrawing[((DRAWING_WIDTH * 2 + 1) * DRAWING_HEIGHT) + 1];
    drawMandelbrot(mandelbrotDrawing);
        
    const char* htmlAfter =     
        "</pre></body>\n"
        "</html>\n";
    
    printf ("sending the header");
    send (socket, htmlBefore, strlen(htmlBefore), 0);
    
    printf ("sending the drawing");
    send (socket, mandelbrotDrawing, strlen(mandelbrotDrawing), 0);
    
    printf ("sending the footer");
    send (socket, htmlAfter, strlen(htmlAfter), 0);
}
 
// start the server listening on the specified port number
int makeServerSocket(int portNumber) {
 
    // create socket
    int serverSocket = socket (AF_INET, SOCK_STREAM, 0);
    assert (serverSocket >= 0);
    // error opening socket
 
    // bind socket to listening port
    struct sockaddr_in serverAddress;
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons (portNumber);
 
    // let the server start immediately after a previous shutdown
    int optionValue = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(int));
 
    int bindSuccess = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
 
    assert (bindSuccess >= 0);
    // if this assert fails wait a short while to let the operating
    // system clear the port before trying again
 
    return serverSocket;
}
 
// wait for a browser to request a connection,
// returns the socket on which the conversation will take place
int waitForConnection(int serverSocket) {
 
    // listen for a connection
    const int serverMaxBacklog = 10;
    listen(serverSocket, serverMaxBacklog);
 
    // accept the connection
    struct sockaddr_in clientAddress;
    socklen_t clientLen = sizeof(clientAddress);
    int connectionSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
    assert(connectionSocket >= 0);
    // error on accept
 
    return connectionSocket;
}

void drawMandelbrot(char* destinationString) {
    int currentCharacterPosition = 0;

    int drawingPointX;
    int drawingPointY;
    
    double scaledPointX, scaledPointY;
    double scale;
    
    scale = 1 / power(2, DRAWING_ZOOM);
    
    drawingPointY = 0;
    while (drawingPointY < DRAWING_HEIGHT) {
        drawingPointX = 0;
        while (drawingPointX < DRAWING_WIDTH) {
            // centering
            scaledPointX = -(DRAWING_WIDTH / 2) * scale;
            scaledPointY = -(DRAWING_HEIGHT / 2) * scale;
            
            // calculate current point
            scaledPointX += drawingPointX * scale;
            scaledPointY += drawingPointY * scale;
            
            if (escapeSteps(scaledPointX, scaledPointY) == MAX_ITERATIONS) {
                destinationString[currentCharacterPosition++] = '*';
                destinationString[currentCharacterPosition++] = '*';
            } else {
                destinationString[currentCharacterPosition++] = ' ';
                destinationString[currentCharacterPosition++] = ' ';
            }
            
            drawingPointX++;
        }
        destinationString[currentCharacterPosition++] = '\n';
        drawingPointY++;
    }
    
    // null-terminate the string
    destinationString[currentCharacterPosition++] = '\0';
}


double power(int base, int exponent) {
    int i = 0;
    double calcResult = 1;
    while (i < exponent) {
        calcResult *= base;
        i++;
    }
    
    return calcResult;
}