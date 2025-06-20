/****************************************************************

    pestcontrol.c (Python output only)

    =============================================================

 Copyright 1996-2025. Tom Barbalet. All rights reserved.

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

#include <stdio.h>

#define LINELENGTH  (200)

int outOfMain = 1;
int beforeFunctions = 1;

int lineCompare(char * line, char * checkFor) {
    int loop = 0;
    while (checkFor[loop] == line[loop]) {
        loop++;
        if (checkFor[loop] == 0) {
            return 1;
        }
    }
    return 0;
}

int containsValue(char * line, char contains) {
    int loop = 0;
    do {
        if (line[loop] == contains) {
            return 1;
        }
        loop++;
    } while (line[loop] != 0);
    return 0;
}

void addASpace(char * line) {
    int loop = 0;
    do {
        loop++;
    } while (line[loop] != 0);
    line[loop] = ' ';
    line[loop+1] = 0;
}

int findFloat(char * line) {
    return ((line[0] == 'f') && (line[1] == 'l') && (line[2] == 'o') && (line[3] == 'a') && (line[4] == 't'));
}

int findInt(char * line) {
    return ((line[0] == 'i') && (line[1] == 'n') && (line[2] == 't'));
}

void findVariableNumberArray(char * line, char * number, char * array, char * type) {
    int loop = 4;
    int numberCount = 0;
    int arrayCount = 0;
    if (line[0] == 'f') {
        loop = 6;
    }
    if (findInt(line)) {
        type[0] = 'i';
        type[1] = 'n';
        type[2] = 't';
        type[3] = 0;
    }
    if (findFloat(line)) {
        type[0] = 'f';
        type[1] = 'l';
        type[2] = 'o';
        type[3] = 'a';
        type[4] = 't';
        type[5] = 0;
    }
    
    if (findInt(line) || findFloat(line)) {
        do {
            array[arrayCount] = line[loop];
            loop++;
            arrayCount++;
        } while (line[loop] != '[');
        loop++;
        do {
            number[numberCount] = line[loop];
            loop++;
            numberCount++;
        } while (line[loop] != ']');
    }
}

void removeReplace(char * line, char * newLine, char * remove, char * replace) {
    int loop = 0;
    int newLoop = 0;
    int removeLoop = 0;
    int replaceLoop = 0;
    
    int resetLoop = -1;
    int resetNewLoop = -1;
    
    addASpace(line);
    
    while(line[loop] != 0) {
        char value = line[loop];
        char removePart = remove[removeLoop];
        
        if (removePart == 0) {
            if (replace) {
                replaceLoop = 0;
                do {
                    if (replace[replaceLoop] != 0) {
                        newLine[newLoop] = replace[replaceLoop];
                    }
                    newLoop++;
                    replaceLoop++;
                } while (replace[replaceLoop]);
            }
            resetLoop = -1;
            resetNewLoop = -1;
            removeLoop = 0;
        } else if (removePart != value) {
            if (resetLoop != -1) {
                loop = resetLoop;
                newLoop = resetNewLoop;
            }
            resetLoop = -1;
            resetNewLoop = -1;
            removeLoop = 0;
            newLine[newLoop] = line[loop];
            newLoop++;
            loop++;
            newLine[newLoop] = 0;
        } else {
            if (resetLoop == -1) {
                resetLoop = loop;
                resetNewLoop = newLoop;
            }
            loop++;
            removeLoop++;
        }
    }
}

void clearLineArray(char * data) {
    int loop = 0;
    while (loop < LINELENGTH) {
        data[loop++] = 0;
    }
}

void copyLineArray(char * dataTo, char * dataFrom) {
    int loop = 0;
    while (loop < LINELENGTH) {
        dataTo[loop] = dataFrom[loop];
        loop++;
    }
}

int combineStrings(int inLocation, char * in, char * add) {
    int localLocation = inLocation;
    int outLocation = 0;
    while (add[outLocation]) {
        in[localLocation] = add[outLocation];
        localLocation++;
        outLocation++;
    }
    return localLocation;
}

int nothingToPrintPython(char * line, char * newLine, int tabs, int noPrint) {
    if (noPrint && !outOfMain) {
        return 0;
    }
    
    if (lineCompare(line, "#")) {
        return 0;
    }
    if (lineCompare(line, "int main")) {
        outOfMain = 0;
        return 0;
    }
    
    removeReplace(line, newLine, ";", 0L);
    copyLineArray(line, newLine);
    
    if (lineCompare(line, "return")) {
        if (outOfMain)
        {
            return 1;
        }
        return 0;
    }
    if (lineCompare(line, "printf(\"")) {
        if (noPrint) {
            return 0;
        }
        
        char tempLine[LINELENGTH] = {0};

        if (containsValue(line,'%')) {
            char tempLine2[LINELENGTH] = {0};
            char tempLine3[LINELENGTH] = {0};
            removeReplace(line, tempLine, "printf(", "print(");
            removeReplace(tempLine, tempLine2, "\"%d\\n\", ", 0L);
            removeReplace(tempLine2, tempLine3, "\"%f\\n\", ", 0L);
            removeReplace(tempLine3, newLine, "\\n", 0L);
        } else {
            removeReplace(line, tempLine, "printf(", "print(");
            removeReplace(tempLine, newLine, "\\n", 0L);
        }
        return 2;
    }
    
    if (lineCompare(line, "//")) {
        removeReplace(line, newLine, "// ", "#");
        return 2;
    }
    if (lineCompare(line, "if")) {
        removeReplace(line, newLine, ") {", "):");
        return 2;
    }
    if (lineCompare(line, "}")) {
        return 0;
    }
    if (lineCompare(line, "while")) {
        removeReplace(line, newLine, ") {", "):");
        return 2;
    }
    if (lineCompare(line, "const int ")) {
        removeReplace(line, newLine, "const int ", 0L);
        return 2;
    }
    if (lineCompare(line, "const float ")) {
        removeReplace(line, newLine, "const float ", 0L);
        return 2;
    }
    
    if (lineCompare(line, "int")) {
        if (containsValue(line, '(')) {
            char tempLine[LINELENGTH] = {0};
            char tempLine2[LINELENGTH] = {0};
            char tempLine3[LINELENGTH] = {0};
            char tempLine4[LINELENGTH] = {0};

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnz", "def");
            removeReplace(tempLine, tempLine2, "float ", 0L);
            removeReplace(tempLine2, tempLine3, "int ", 0L);
            removeReplace(tempLine3, tempLine4, "void", 0L);
            removeReplace(tempLine4, newLine, ") {", "):");

        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};

                findVariableNumberArray(line, number, array, type);
                sprintf(newLine, "%s = [0] * %s", array, number);
                return 2;
            } else {
                char tempLine[LINELENGTH] = {0};
                char tempLine2[LINELENGTH] = {0};
                removeReplace(line, tempLine, "int ", 0L);
                removeReplace(tempLine, tempLine2, "float ", 0L);
                removeReplace(tempLine2, newLine, ") {", "):");
                return 2;
            }
        }
        return 2;
    }
    if (lineCompare(line, "float")) {
        if (containsValue(line, '(')) {
            char tempLine[LINELENGTH] = {0};
            char tempLine2[LINELENGTH] = {0};
            char tempLine3[LINELENGTH] = {0};
            char tempLine4[LINELENGTH] = {0};

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnzat ", "def ");
            removeReplace(tempLine, tempLine2, "float ", 0L);
            removeReplace(tempLine2, tempLine3, "int ", 0L);
            removeReplace(tempLine3, tempLine4, "void", 0L);
            removeReplace(tempLine4, newLine, ") {", "):");
            return 2;
        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};
                findVariableNumberArray(line, number, array, type);
                sprintf(newLine, "%s = [0] * %s", array, number);
                return 2;
            } else {
                char tempLine[LINELENGTH] = {0};
                char tempLine2[LINELENGTH] = {0};
                removeReplace(line, tempLine, "int ", 0L);
                removeReplace(tempLine, tempLine2, "float ", 0L);
                removeReplace(tempLine2, newLine, ") {", "):");
                return 2;
            }

        }
        return 2;
    }
    
    
    if (lineCompare(line, "void")) {
        char tempLine[LINELENGTH] = {0};
        char temp2Line[LINELENGTH] = {0};
        char temp3Line[LINELENGTH] = {0};
        char temp4Line[LINELENGTH] = {0};

        line[1] = 'i';
        line[2] = 'z';

        beforeFunctions = 0;

        removeReplace(line, tempLine, "vizd", "def");
        removeReplace(tempLine, temp2Line, "int ", 0L);
        removeReplace(temp2Line, temp3Line, "float ", 0L);
        removeReplace(temp3Line, temp4Line, "void", 0L);
        removeReplace(temp4Line, newLine, ") {", "):");
    }
    return 2;
}

void translateFile(char* filename, char* writefilename, int noPrint) {
    int tabs;
    int loop = 0;
    char lineArray[LINELENGTH];
    int lineArrayFound = 0;
    
    FILE* reading = fopen(filename, "rb");
    FILE* writing = fopen(writefilename, "w");

    clearLineArray(lineArray);
    tabs = 0;
    outOfMain = 1;
    beforeFunctions = 1;
    
    if (reading) {
        do {
            if (!feof(reading)) {
                char byte;
                fread(&byte, 1, 1, reading);
                if (feof(reading)) {
                    lineArrayFound = 1;
                } else {
                    if (byte == 9) {
                        tabs++;
                    } else if (byte == 10) {
                        lineArrayFound = 1;
                    } else {
                        lineArray[loop++] = byte;
                    }
                }
            } else {
                lineArrayFound = 1;
            }
            
            if (lineArrayFound) {
                char newLine[LINELENGTH];
                int value = nothingToPrintPython(lineArray, newLine, tabs, noPrint);
                
                if (writing) {
                    if (value != 0) {
                        if (outOfMain) {
                            int tabsLoop = 0;
                            while (tabsLoop < tabs) {
                                fprintf(writing, "\t");
                                tabsLoop++;
                            }
                        }
                    }
                    
                    if (value == 2) {
                        fprintf(writing, "%s\n", newLine);
                    } else if (value == 1) {
                        fprintf(writing, "%s\n", lineArray);
                    }
                }

                tabs = 0;
                clearLineArray(lineArray);
                clearLineArray(newLine);
                lineArrayFound = 0;
                loop = 0;
            }
        } while (!feof(reading));

        fclose(reading);
        fclose(writing);
    }
}

int parseArgs(int argc, const char * argv[], char** csource, char** python, int * noPrint) {
    int loop = 1;
    int returnValue = 0;
    
    *noPrint = 0;
    
    while (loop < argc) {
        const char* row = argv[loop];
        if (row[0] == '-') {
            if (row[1] == 'h') {
                printf("Usage: ./pc csourcefile [-p pythonout]\n");
            }
            
            if (row[1] == 'v') {
                printf("v1.00.00 (%s)\n", __DATE__);
            }
            
            if (row[1] == 'p') {
                loop++;
                *python = (char*)argv[loop];
                returnValue = 1;
            }
            if (row[1] == 'n') {
                *noPrint = 1;
            }
        } else {
            *csource = (char*)argv[loop];
        }
        loop++;
    }
    return returnValue;
}

int main(int argc, const char * argv[]) {
    char* python = 0L;
    char* csource = 0;
    int noPrint;
    
    if (parseArgs(argc, argv, &csource, &python, &noPrint)) {
        if (python) {
            printf("python : %s\n", python);
        }
        if (csource) {
            printf("csource : %s\n", csource);
        }
        
        if (noPrint) {
            printf("No Print ON\n");
        }
        
        if (csource && python) {
            translateFile(csource, python, noPrint);
        }
    }
    return 0;
}
