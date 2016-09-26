//
//  main.cpp
//  Project 4
//
//  Created by Melissa Peng on 5/4/16.
//  Copyright © 2016 Melissa Peng. All rights reserved.
//

#include <iostream>
#include <string>
#include <cctype>
#include <cassert>
using namespace std;

int tally(const string a[], int n, string target);
int findFirst(const string a[], int n, string target);
bool findFirstSequence(const string a[], int n, string target, int& begin, int& end);
int positionOfMin(const string a[], int n);
int moveToEnd(string a[], int n, int pos);
int moveToBeginning(string a[], int n, int pos);
int disagree(const string a1[], int n1, const string a2[], int n2); // done
int removeDups(string a[], int n); //
bool subsequence(const string a1[], int n1, const string a2[], int n2); //
int mingle(const string a1[], int n1, const string a2[], int n2, string result[], int max);
bool hasNondecreasingOrder (const string a[], int n);
int divide(string a[], int n, string divider);


int main(){

    string e[5] = { "daenerys", "daenerys", "daenerys", "jon", "jon" };
    assert(removeDups(e, 5) == 2 && e[1] == "jon");
    
    cout << "All tests succeeded" << endl;
}

int tally(const string a[], int n, string target){
    if ( n < 0)
        return -1;
    int nStrings = 0;
    for (int i = 0; i < n; i++)
        if (a[i] == target)
            nStrings++;
    return nStrings;
}

int findFirst(const string a[], int n, string target){
    if (n < 0)
        return -1;
    for (int i = 0; i < n; i++)
        if (a[i] == target)
            return i;
    return -1;
}

bool findFirstSequence(const string a[], int n, string target, int& begin, int& end){
    int last = 0;
    int first = 0;
    if (n < 0)
        return false;
    else if (findFirst(a, n, target) == -1) // if the string is not in the array
        return false;
    else
        for (int i = 0; i < n; i++) // find first element equal to target
            if (a[i] == target)
            {
                first = i;
                break;
            }
        for (int i = first; i < n; i++)
        {
            if (a[i] == target)
                last = i;
            else
                break;
        }
    end = last;
    begin = first;
    return true;
}

int positionOfMin(const string a[], int n){
    if (n <= 0)
        return -1;
    string min = a[0];
    int i;
    int k = 0;
    for (i = 0; i < n; i++) // scan string. if any element is less than min, it replaces min.
        if (a[i] < min)
            k = i;
            min = a[k];
    return k;
}

int moveToEnd(string a[], int n, int pos){
    if (n < 0)
        return -1;
    if (pos >= n || pos < 0)
        return -1;
    string posString = a[pos];
    for (int i = pos + 1; i < n; i++)
        a[i-1] = a[i];
    a[n-1] = posString;
    return pos;
}

int moveToBeginning(string a[], int n, int pos){
    if (n < 0)
        return -1;
    if (pos >= n || pos < 0)
        return -1;
    string posString = a[pos];
    for (int i = pos - 1; i >= 0; i--)
        a[i+1] = a[i];
    a[0] = posString;
    return pos;
}

int disagree(const string a1[], int n1, const string a2[], int n2){
    if (n1 < 0 || n2 < 0)
        return -1;
    int smallerN;
    if (n1 <= n2)
        smallerN = n1;
    else
        smallerN = n2;
    for (int i = 0; i < smallerN; i++)
        if (a1[i] != a2[i])
            return i;
    return smallerN;
}

int removeDups(string a[], int n){
    if (n < 0)
        return -1;
    int last = 0;
    int first = 0;
    int nRetained = n;
    for (int i=0; i != nRetained; i++)
    {
        for (int j = i; j < nRetained; j++)
            if (a[j] == a[i])
            {
                first = j;
                break;
            }
        for (int k = first; k < nRetained; k++)
        {
            if (a[k] == a[i])
                last = k;
            else
                break;
        }

        for (int j = first + 1; j <= last; j++)
        {
            moveToEnd(a, n, j);
            nRetained--;
            last--;
            j--;
        }
    }
    return nRetained;
}

bool subsequence(const string a1[], int n1, const string a2[], int n2){
    if (n1 < 0 || n2 < 0 || n2 > n1)
        return false;
    if (n2 == 0)
        return true;
    int nStrings = 0;
    int positionLastString = -1; // makes sure that in inner for loop i is initialized to zero for k = 0
    for (int k = 0; k < n2; k++) // for every string in a2, is it in a1? how can you tell if it isn't in a1
    {
        for (int i = positionLastString + 1; i < n1; i++) // check if a2 string is in specified range of a1
        {
            if (a1[i] == a2[k])
            {
                positionLastString = i; // if the string IS in a1, set positionLastString accordingly and break
                nStrings++;
                break;
            }
        }
    }
    if (nStrings != n2) // returns false if number of strings in common is not n2
        return false;
    else
        return true;
}

int mingle(const string a1[], int n1, const string a2[], int n2, string result[], int max){
    if (n1 < 0 || n2 < 0)
        return -1;
    if (n1 + n2 > max)
        return -1;
    
    if (n1 == 0)
    {
        for (int i=0; i < n2; i++)
            result[i] = a2[i];
        return n2;
    }
    
    if (n2 == 0)
    {
        for (int i=0; i < n1; i++)
            result[i] = a1[i];
        return n1;
    }
        
    if (hasNondecreasingOrder(a1, n1) && hasNondecreasingOrder(a2, n2))
    {
        int i = 0;
        int j = 0;
        int k = 0;
        while (i < n1 + n2)
        {
            if (j == n1) // if we already used up all elements of a1
            {
                for (i=i; k < n2; i++)
                {
                    result[i] = a2[k];
                    k++;
                }
                break;
            }
            
            if (k == n2) // if we already used up all elements of a2
            {
                for (i=i; j < n1; i++)
                {
                    result[i] = a1[j];
                    j++;
                }
                break;
            }
            
            if (a1[j] <= a2[k]) // pick smallest value from a1[j] and a2[k] to add to result
            {
                result[i] = a1[j];
                j++;
            }
            else
            {
                result[i] = a2[k];
                k++;
            }
            
            i++;
        }
        return n1 + n2;
    }
    else
        return -1;
}

bool hasNondecreasingOrder(const string a[], int n){
    for (int i = 0; i + 1 < n; i++)
        if (a[i] > a[i + 1])
            return false;
    return true;
}

int divide(string a[], int n, string divider){
    if (n < 0)
        return -1;
    string lessThan[999];
    string equalTo[999];
    string greaterThan[999];
    int nLessThan = 0, nEqualTo = 0, nGreaterThan = 0;
    int j = 0, k = 0, l = 0;
    for (int i = 0; i < n; i++) // put every string in array into one of three smaller arrays
    {
        if (a[i] < divider)
        {
            lessThan[j] = a[i];
            j++;
            nLessThan++; // number of elements in lessThan
        }
        if (a[i] == divider)
        {
            equalTo[k] = a[i];
            k++;
            nEqualTo++;
        }
        if (a[i] > divider)
        {
            greaterThan[l] = a[i];
            l++;
            nGreaterThan++;
        }
    }

    // add nLessThan to a, then nEqualTo, then nGreaterThan
    int i;
    k = 0, l = 0;
    for (i = 0; i < nLessThan; i++)
        a[i] = lessThan[i];
    for (i = i; k < nEqualTo; i++)
    {
        a[i] = equalTo[k];
        k++;
    }
    for (i=i; l < nGreaterThan; i++)
    {
        a[i] = greaterThan[l];
        l++;
    }
    
    if (nLessThan == n)
        return n;
    else
        return nLessThan;
}
