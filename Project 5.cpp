//
//  main.cpp
//  Project 5
//
//  Created by Melissa Peng on 5/11/16.
//  Copyright © 2016 Melissa Peng. All rights reserved.
//

#include <iostream>
#include <cctype>
#include <fstream>
#include <string.h>
#include <stdio.h>
using namespace std;

int format(int lineLength, istream& inf, ostream& outf);
bool isWhiteSpace(char c);
bool isPunctuation(char c);
bool getToken(char token[], istream& inf, int& nChar);
bool tooLong(int portionLength, int lineLength);

int main(){
    ifstream infile("/Users/melissapeng/Documents/2015-2016/CS 31/Project 5/data.txt");
    if (! infile)
    {
        cerr << "Error: Cannot open data.txt!" << endl;
    }
    
    ofstream outfile("/Users/melissapeng/Documents/2015-2016/CS 31/Project 5/results.txt");
    if (! outfile)
        cerr << "Error: Cannot open results.txt!" << endl;
    
    cerr << format(401, infile, outfile);
}

int format(int lineLength, istream& inf, ostream& outf){
    if (lineLength < 1)
        return 2;
    
    char previousToken[151] = "";
    int previousTokenChars = 0;
    int lineChars = 0;
    int longestChars = 0;
    int nTokens = 0;
    
    for (;;)
    {
        // Attempt to get the next token
        char token[151] = "";
        int nChars = 0;
        
        if (! (getToken(token, inf, nChars))) // exit loop if reach end of file
            break;
        if (strcmp(token, "") == 0)
            break;
        if (nChars > longestChars) // by the end of the loop, will find nChars for longest word portion in input
            longestChars = nChars;
        
        // paragraph breaks
        if (strcmp(token, "#P#") == 0)
        {
            outf << '\n' << '\n';
            lineChars = 0;
            
            while (strcmp(token, "#P#") == 0) // consumes consecutive p breaks without outputting anything
            {
                getToken(token, inf, nChars);
            }
            
            //print last token consumed and stored
            int charsSoFar = 0; // how many chars of token have already been output?
            int k = 0;
            if (nChars > lineLength)
            {
                while ((nChars - charsSoFar) > lineLength)
                {
                    k = 0;
                    for (int i = charsSoFar; i < charsSoFar + lineLength; i++)
                    {
                        outf << token[i];
                        k++;
                    }
                    charsSoFar += k;
                    outf << '\n';
                }
                
                for (int i = charsSoFar; i < nChars; i++)
                    outf << token[i];
                
                lineChars = nChars - charsSoFar;
            }
            else
            {
                outf << token;
                lineChars += nChars;
            }
        }
        
        // an ordinary word
        else
        {
            if ((lineChars + nChars + 1 > lineLength) && (strcmp(previousToken, "") != 0))
            {
                outf << '\n';
                lineChars = 0;
            }
            
            int charsSoFar = 0; // how many chars of token have already been output?
            int k = 0;
            if (nChars > lineLength)
            {
                while ((nChars - charsSoFar) > lineLength)
                {
                    k = 0;
                    for (int i = charsSoFar; i < charsSoFar + lineLength; i++)
                    {
                        outf << token[i];
                        k++;
                    }
                    charsSoFar += k;
                    outf << '\n';
                }
                
                for (int i = charsSoFar; i < nChars; i++)
                    outf << token[i];
                
                lineChars = nChars - charsSoFar;
            }
            else
            {
                if ((previousTokenChars != 0) && (previousToken[previousTokenChars-1] != '-') && (lineChars != 0)) // if end of previous token (word portion) is end of word
                {
                    outf << ' ';
                    lineChars++;
                }
                if ((previousTokenChars != 0) && (isPunctuation(previousToken[previousTokenChars-1])) && (lineChars != 0))
                {
                    outf << ' ';
                    lineChars++;
                }
                outf << token;
                lineChars += nChars;
            }
        }
        strcpy(previousToken, token);
        previousTokenChars = nChars;
        nTokens++;
    }
    
    if (nTokens > 0)
        outf << '\n'; // output must end with newline, unless the input was empty
    
    if (tooLong(longestChars, lineLength)) // if nChars > lineLength for any word portion
       return 1;
    else
        return 0;
}

// store next word portion as token. a token ends with a space (exclusive) or a hyphen (inclusive)
bool getToken(char token[], istream& inf, int& nChar){
    char c;
    if (inf.eof())
        return false;
    nChar = 0;
    strcpy(token, "");
    char nextChar[2];

    // consume initial white space
    while (inf.get(c))
    {
        if ( ! isWhiteSpace(c))
        {
            nextChar[0] = c; // convert char c to a c string with one interesting element
            nextChar[1] = '\0';
            strcat(token, nextChar);
            nChar++;
            break;
        }
    }

    // read characters into token until first white space or hyphen
    while (inf.get(c))
    {
        if ( isWhiteSpace(c))
            break;
        if (c == '-')
        {
            strcat(token, "-");
            nChar++;
            break;
        }
        nextChar[0] = c;
        nextChar[1] = '\0';
        strcat(token, nextChar);
        nChar++;
    }
    return true;
}

bool isWhiteSpace(char c){
    if (c == '\n' || c == ' '|| c=='\t')
        return true;
    else
        return false;
}

bool isPunctuation(char c){
    if (c == '.' || c == '?')
        return true;
    else
        return false;
}

bool tooLong(int portionLength, int lineLength){
    if (portionLength > lineLength)
        return true;
    else
        return false;
}

/*
void processLine(int lineLength, istream& inf, ostream& outf, char text[]){
    int nChar = 0; // nChar is number of characters in output line
    int k = 0; // k is the position in input line
    char portion[300];
    
    while (text[k] != ' ' && text[k-1] != '-') // reads and prints whole word portion
    {
        int i = 0;
        cerr << text[k];
        portion[i] = text[k];
        outf << text[k];
        k++;
        i++; // i is number of interesting elements in portion
        nChar++;
    }
    if (isWhiteSpace(text[k]))
    {
        outf << ' '; // print space, increment k and nChar
        k++;
        nChar++;
        while (text[k] == ' ') // if there are additional spaces, increment k but not nChar and don't print anything
            k++;
    }
    if (isPunctuation(text[k]) && isWhiteSpace(text[k+1]))
    {
        punctuationSpacing(text, k, nChar, inf, outf);
    }
}*/

/* // only call this function if text[k] = "." or "?"
void punctuationSpacing(char text[], int& position, int& charCounter, istream& inf, ostream& outf){
    outf << text[position]; //prints the . or ?
    outf << '  '; // print space, increment k and nChar
    position += 3;
    charCounter += 3;
    while (text[position] == ' ') // if there are additional spaces, increment k but not nChar and don't print anything
        position++;
}

int wordLength(char text[]){ // words must end with a space, or it'll be an infinite loop
    int wordLength = 0;
    for (int k = 0; ! isEndOfPortion(text, k); k++)
        wordLength++;
    wordLength++;
    return woradLength;
}


int portionLength(string text){ //portion ends with - or end of the word
    int portionLength = 0;
    for (int k = 0; text[k] != '-'; k++)
    {
        portionLength++;
    }
    portionLength++;
    return portionLength;
} */