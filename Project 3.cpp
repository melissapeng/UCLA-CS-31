//
//  main.cpp
//  sdf
//
//  Created by Melissa Peng on 4/18/16.
//  Copyright © 2016 Melissa Peng. All rights reserved.
//

#include <iostream>
#include <cctype>
#include <string>
#include <cassert>
using namespace std;

// declaring the functions
bool hasCorrectSyntax(string dance);
int convertDance(string dance, string& instructions, int& badBeat);
bool isDirectionSlash (int i, string s);
bool isDirection (int i, string s);
int badbeat (string badDance);

int main()
{
    for (;;)
    {
        cout << "Enter dance: ";
        string d;
        getline(cin, d);
        string ins = "Wow!";
        int bb = 99;
        if (d == "quit")
            break;
        cout << "hasCorrectSyntax returns ";
        if (hasCorrectSyntax(d))
            cout << "true" << endl;
        else
            cout << "false" << endl;
        cout << convertDance(d, ins, bb) << endl;
        cout << ins << endl << bb << endl;
    }

}

int convertDance(string dance, string& instructions, int& badBeat)
{
    // syntactically correct error, return 1
    if (! hasCorrectSyntax(dance) )
        return 1;
    int k = 0;
    string buildingInstructions = "";
    while (k != dance.size())
    {
        switch (dance[k]){
            case 'L':
            case 'l':
            case 'R':
            case 'r':
            case 'U':
            case 'u':
            case 'D':
            case 'd':
            {
                char step = tolower(dance[k]); // translates LRUDlrud single steps
                                                // moves k one over, past / that concludes the beat
                                                // letters following digits (freeze instructions) are protected by default case
                buildingInstructions += step;
                k += 2;
                break;
            }
            case '/': // translates '/' to '.'
                        // slashes during freezes are protected by default case
            {
                buildingInstructions += '.';
                k++;
                break;
            }
            
                // freeze related errors and translating freezes
            default: // only digits will fall into this case, digit character = dance[k]
            {
                int freezeLength;
                if ((isdigit(dance[k+1])))
                    freezeLength = (dance[k] - '0') * 10 + (dance[k+1] - '0');
                else
                    freezeLength = dance[k] - '0';
                
                // premature ending error, return 2
                int lastDirection = 0;
                for (int i = 0; i != dance.size(); i++) // finds position of the last direction in the string
                    if (isDirection(i, dance))
                        lastDirection = i;
                if ((isdigit(dance[k+1])))
                {
                    if (lastDirection == k+2) // if last direction is part of a freeze
                    {
                        int nSlashesInFreeze = 0;
                        for (int i = lastDirection; i != dance.size(); i++) // counts number of slashes after a freeze like 3u
                            if (dance[i] == '/')
                                nSlashesInFreeze++;
                        if (nSlashesInFreeze < freezeLength) // if premature, set badBeat and return 2
                        {
                            int nSlashesTotal = 0;
                            for (int i=0; i != dance.size(); i++)
                                if (dance[i] == '/')
                                    nSlashesTotal++;
                            badBeat = nSlashesTotal + 1;
                            return 2;
                        }
                    }
                }
                else
                {
                    if (lastDirection == k+1) // if last direction is part of a freeze
                    {
                        int nSlashesInFreeze = 0;
                        for (int i = lastDirection; i != dance.size(); i++) // counts number of slashes after a freeze like 3u
                            if (dance[i] == '/')
                                nSlashesInFreeze++;
                        if (nSlashesInFreeze < freezeLength) // if premature, set badBeat and return 2
                        {
                            int nSlashesTotal = 0;
                            for (int i=0; i != dance.size(); i++)
                                if (dance[i] == '/')
                                    nSlashesTotal++;
                            badBeat = nSlashesTotal + 1;
                            return 2;
                        }
                    }
                }
                
                // short freeze error, return 3
                if (freezeLength < 2)
                {
                    badBeat = 1;
                    for (int i = 0; i != k; i++) // will scan string and count how many slashes until dance[k]
                        if (dance[i] == '/')
                            badBeat++;
                    return 3;
                }
                
                // beat not consisting of only a slash during a freeze, return 4
                if (isdigit(dance[k+1]))
                {
                    for (int i = k + 3; i != k + 3 + freezeLength; i++) {
                        if (dance[i] != '/') // i is the offender
                        {
                            badBeat = 1;
                            for (int j = 0; j != i; j++)
                            {
                                if (dance[j] == '/')
                                    badBeat++;
                            }
                            return 4;
                        }
                    }
                }
                else
                {
                    for (int i = k + 2; i != k + 2 + freezeLength; i++) {
                        if (dance[i] != '/') // i is the offender
                        {
                            badBeat = 1;
                            for (int j = 0; j != i; j++)
                            {
                                if (dance[j] == '/')
                                    badBeat++;
                            }
                            return 4;
                        }
                    }
                }
                
                // translates freezes, e.g. 3u/// to UUU and moves k (2+freeze length) or (3+freeze length) further down the string
                // this way letters and slashes that occur during freezes will not be "counted twice" by previous cases
                char freezeStep;
                if (isdigit(dance[k+1]))
                {
                    freezeStep = dance[k+2];
                    for (int i = 0; i < freezeLength; i++)
                    {
                        buildingInstructions += toupper(freezeStep);
                        k++;
                    }
                    k += 3;
                }
                else
                {
                    freezeStep = dance[k+1];
                    for (int i = 0; i < freezeLength; i++)
                    {
                        buildingInstructions += toupper(freezeStep);
                        k++;
                    }
                    k += 2;
                }
            }
        }
    }
    instructions = buildingInstructions;
    return 0; // only occurs if entire string has been processed without running into any problems
}

bool hasCorrectSyntax(string dance)
{
    for (int k = 0; k != dance.size(); k++) // if any substring is not a beat, return false. if the loop finishes without detecting this, return true.
    {
        if (dance[k] == '/') // '/' is a beat
            k += 0;
        else if (isDirectionSlash(k, dance)) // direction plus / is a beat
            k++;
        else if (isdigit(dance[k]) && isDirectionSlash((k+1), dance)) // digit plus direction plus / is a beat
            k += 2;
        else if (isdigit(dance[k]) && isdigit(dance[k+1]) && isDirectionSlash(k+2, dance)) // two digits plus direction plus / is a beat
            k += 3;
        else
            return false; // any substring that is not one of aforementioned valid beats will cause hasCorrectSyntax to return false
    }
    return true; // only happens if you go through the entire string and don't find anything that isn't a beat, i.e. string is only valid beats
}

bool isDirectionSlash (int i, string s)
{
    switch(s[i]){
        case 'L':
        case 'l':
        case 'R':
        case 'r':
        case 'U':
        case 'u':
        case 'D':
        case 'd':
            if (s[i+1] == '/')
            return true;
        default:
            return false;
    }
}

bool isDirection (int i, string s)
{
    switch(s[i]){
        case 'L':
        case 'l':
        case 'R':
        case 'r':
        case 'U':
        case 'u':
        case 'D':
        case 'd':
            return true;
        default:
            return false;
    }
}
