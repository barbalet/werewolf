/****************************************************************

 werewolf_shared.c

    =============================================================

 Copyright 1996-2025 Tom Barbalet. All rights reserved.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    This software is a continuing work of Tom Barbalet, begun on
    13 June 1996. No apes or cats were harmed in the writing of
    this software.

****************************************************************/

#include "werewolf_shared.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// This should be dynamic in the future. For now a fixed number
static unsigned long globals[LINELENGTH];
static unsigned long numberGlobals;

int outOfMain = 1;
int beforeFunctions = 1;

void clearGlobals(void) {
    memset(globals, 0, sizeof(globals));
    numberGlobals = 0;
}

static int firstValue(char value) {
    if (value == '_') return 1;
    if ((value >= 'A') && (value <= 'Z')) return 1;
    if ((value >= 'a') && (value <= 'z')) return 1;
    return 0;
}

static int remainingValues(char value) {
    if ((value >= '0') && (value <= '9')) return 1;
    return firstValue(value);
}

static unsigned long mathHashFnv(char *key) {
    unsigned long hash = 2166136261;
    if (!firstValue(*key)) {
        return 0;
    }
    
    hash = (8494653 * hash) ^ (unsigned long)(*key++);
    while (remainingValues(*key)) {
        hash = (8494653 * hash) ^ (unsigned long)(*key++);
    }
    return hash;
}

static int globalFound(char *key) {
    unsigned long hash = mathHashFnv(key);
    if (hash == 0) {
        printf("no text found - investigate issue");
        return 0;
    }
    
    for (int i = 0; i < LINELENGTH; i++) {
        if (globals[i] == hash) {
            return 1;
        }
    }
    return 0;
}

void addGlobal(char *key) {
    unsigned long hash = mathHashFnv(key);
    if (numberGlobals < LINELENGTH) {
        globals[numberGlobals++] = hash;
    }
}

void clearLineArray(char *data) {
    memset(data, 0, LINELENGTH);
}

void copyLineArray(char *dataTo, char *dataFrom) {
    strncpy(dataTo, dataFrom, LINELENGTH);
    dataTo[LINELENGTH-1] = '\0'; // Ensure null-termination
}

static int combineStrings(int inLocation, char *in, char *add) {
    int add_len = (int)strlen(add);
    strncpy(in + inLocation, add, add_len);
    return inLocation + add_len;
}

int parseStringForGlobals(char *in, char *out) {
    int somethingHasChanged = 0;
    int locationIn = 0;
    int outLocation = 0;
    char currentToken[LINELENGTH] = {0};
    int tokenPos = 0;
    int inAlpha = firstValue(in[0]);
    
    while (in[locationIn]) {
        if (inAlpha != remainingValues(in[locationIn])) {
            if (inAlpha && globalFound(currentToken)) {
                outLocation = combineStrings(outLocation, out, "$");
                somethingHasChanged = 1;
            }
            outLocation = combineStrings(outLocation, out, currentToken);
            
            inAlpha = firstValue(in[locationIn]);
            tokenPos = 0;
            memset(currentToken, 0, sizeof(currentToken));
        }
        currentToken[tokenPos++] = in[locationIn++];
    }
    
    if (inAlpha && globalFound(currentToken)) {
        outLocation = combineStrings(outLocation, out, "$");
        somethingHasChanged = 1;
    }
    combineStrings(outLocation, out, currentToken);
    return somethingHasChanged;
}

int lineCompare(char *line, char *checkFor) {
    return strncmp(line, checkFor, strlen(checkFor)) == 0;
}

int containsValue(char *line, char contains) {
    return strchr(line, contains) != NULL;
}

static void addASpace(char *line) {
    size_t len = strlen(line);
    if (len < LINELENGTH - 1) {
        line[len] = ' ';
        line[len+1] = '\0';
    }
}

static int findFloat(char *line) {
    return strncmp(line, "float", 5) == 0;
}

static int findInt(char *line) {
    return strncmp(line, "int", 3) == 0;
}

void findVariableNumberArray(char *line, char *number, char *array, char *type) {
    int loop = 4;
    if (findFloat(line)) {
        strcpy(type, "float");
        loop = 6;
    } else if (findInt(line)) {
        strcpy(type, "int");
    } else {
        return;
    }
    
    char *bracket = strchr(line, '[');
    char *endBracket = strchr(line, ']');
    
    if (!bracket || !endBracket) return;
    
    // Copy array name
    strncpy(array, line + loop, bracket - (line + loop));
    array[bracket - (line + loop)] = '\0';
    
    // Copy number
    strncpy(number, bracket + 1, endBracket - (bracket + 1));
    number[endBracket - (bracket + 1)] = '\0';
}

void removeReplace(char *line, char *newLine, char *remove, char *replace) {
    char *pos = line;
    char *found;
    newLine[0] = '\0';
    
    addASpace(line);
    
    while ((found = strstr(pos, remove)) != NULL) {
        // Copy part before the match
        strncat(newLine, pos, found - pos);
        
        // Add replacement if provided
        if (replace) {
            strcat(newLine, replace);
        }
        
        pos = found + strlen(remove);
    }
    
    // Copy remaining part
    strcat(newLine, pos);
}
