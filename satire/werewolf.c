/****************************************************************

    werewolf.c

    =============================================================

 Copyright 1996-2024 Tom Barbalet. All rights reserved.

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

// This should be dynamic in the future. For now a fixed number
unsigned long globals[LINELENGTH];
unsigned long numberGlobals;

typedef int ( fileHandler )(char * line, char * newLine, int tabs, int noPrint);

typedef void ( openEndFile )(FILE * file, int open, char * className);

int outOfMain = 1;
int beforeFunctions = 1;

void clearGlobals() {
    int loop = 0;
    while (loop < LINELENGTH) {
        globals[loop] = 0;
        loop++;
    }
    numberGlobals = 0;
}

int firstValue(char value) {
    if (value == '_') return 1;
    if ((value >= 'A') && (value <= 'Z')) return 1;
    if ((value >= 'a') && (value <= 'z')) return 1;
    return 0;
}

int remainingValues(char value) {
    if ((value >= '0') && (value <= '9')) return 1;
    if (firstValue(value)) {
        return 1;
    }
    return 0;
}

unsigned long mathHashFnv(char * key) {
    unsigned long hash = 2166136261;
     if (firstValue( *key )) {
        hash = (( 8494653 * hash ) ^ ( unsigned long )( *key++ ));
   } else {
        return 0;
    }
    
    while (remainingValues( *key )) {
        hash = (( 8494653 * hash ) ^ ( unsigned long )( *key++ ));
    }
    return hash;
}

int globalFound(char * key) {
    unsigned long hash = mathHashFnv(key);
    if (hash == 0) {
        printf("no text found - investigate issue");
        return 0;
    }
    int loop = 0;
    while (loop < LINELENGTH) {
        unsigned long localHash = globals[loop];
        if (localHash != 0) {
            if (hash == localHash) {
                return 1;
            }
        }
        loop++;
    }
    return 0;
}

void addGlobal(char* key) {
    unsigned long hash = mathHashFnv(key);
    globals[numberGlobals] = hash;
    numberGlobals++;
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

int copyUntilZero(char * in, char * out) {
    int loop = 0;
    while (out[loop] != 0){
        in[loop] = out[loop];
        loop++;
    }
    return loop;
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

int parseStringForGlobals(char * in, char * out) {
    int somethingHasChanged = 0;
    int locationIn = 0;
    int locationInternal = 0;
    int locationOut = 0;
    int alphaSet = firstValue(in[0]);
    char internal[LINELENGTH];
    int outLocation = 0;
    
    clearLineArray(internal);
    
    while (in[locationIn]) {
        if (alphaSet != remainingValues(in[locationIn]))
        {
            if (alphaSet) {
                if (globalFound(internal)){
                    outLocation = combineStrings(outLocation, out, "$");
                    somethingHasChanged = 1;
                }
            }
            outLocation = combineStrings(outLocation, out, internal);


            alphaSet = firstValue(in[locationIn]);
            locationInternal = 0;
            clearLineArray(internal);
        }
        internal[locationInternal] = in[locationIn];
        locationIn++;
        locationInternal++;
    }
    if (alphaSet) {
        if (globalFound(internal)){
            outLocation = combineStrings(outLocation, out, "$");
            somethingHasChanged = 1;
        }
    }
    outLocation = combineStrings(outLocation, out, internal);
    return somethingHasChanged;
}

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

void openEndJava(FILE * file, int open, char * className) {
    if (file) {
        if (open) {
            fprintf(file, "public class %s {\n", className);
        } else {
            fprintf(file, "}\n");
        }
    }
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

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnz", "def");
            removeReplace(tempLine, tempLine2, "float ", 0L);
            removeReplace(tempLine2, tempLine3, "int ", 0L);
            removeReplace(tempLine3, newLine, ") {", "):");

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

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnzat ", "def ");
            removeReplace(tempLine, tempLine2, "float ", 0L);
            removeReplace(tempLine2, tempLine3, "int ", 0L);
            removeReplace(tempLine3, newLine, ") {", "):");
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
        
        beforeFunctions = 0;

        removeReplace(line, tempLine, "void", "def");
        removeReplace(tempLine, temp2Line, "int ", 0L);
        removeReplace(temp2Line, temp3Line, "float ", 0L);
        removeReplace(temp3Line, newLine, ") {", "):");
    }
    return 2;
}

int nothingToPrintRubyPostProcess(char * line, char * newLine, int tabs, int noPrint) {
    clearLineArray(newLine);
    if (parseStringForGlobals(line, newLine)){
        return 2;
    } else {
        return 0;
    }
}

int nothingToPrintRuby(char * line, char * newLine, int tabs, int noPrint) {
    
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
        char tempLine2[LINELENGTH] = {0};

        if (containsValue(line,'%')) {
            char tempLine3[LINELENGTH] = {0};
            char tempLine4[LINELENGTH] = {0};
            removeReplace(line, tempLine, "printf(", "puts ");
            removeReplace(tempLine, tempLine2, "\"%d\\n\", ", 0L);
            removeReplace(tempLine2, tempLine3, "\"%f\\n\", ", 0L);
            removeReplace(tempLine3, tempLine4, ")", 0L);
            removeReplace(tempLine4, newLine, "\\n", 0L);
        } else {
            removeReplace(line, tempLine, "printf(", "puts ");
            removeReplace(tempLine, tempLine2, ")", 0L);
            removeReplace(tempLine2, newLine, "\\n", 0L);
        }
        return 2;
    }
    
    if (lineCompare(line, "//")) {
        removeReplace(line, newLine, "// ", "#");
        return 2;
    }
    if (lineCompare(line, "if")) {
        removeReplace(line, newLine, "{", 0L);
        return 2;
    }
    if (lineCompare(line, "}")) {
        if (outOfMain) {
            removeReplace(line, newLine, "}", "end");
        } else {
            removeReplace(line, newLine, "}", 0L);
        }
        return 2;
    }
    if (lineCompare(line, "while")) {
        removeReplace(line, newLine, "{", 0L);
        return 2;
    }
    if (lineCompare(line, "const int ")) {
        removeReplace(line, newLine, "const int ", 0L);
        if (beforeFunctions) {
            addGlobal(newLine);
        }
        return 2;
    }
    if (lineCompare(line, "const float ")) {
        removeReplace(line, newLine, "const float ", 0L);
        if (beforeFunctions) {
            addGlobal(newLine);
        }
        return 2;
    }
    
    if (lineCompare(line, "int")) {
        if (containsValue(line, '(')) {
            char tempLine[LINELENGTH] = {0};
            char tempLine2[LINELENGTH] = {0};
            char tempLine3[LINELENGTH] = {0};

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnz", "def");
            removeReplace(tempLine, tempLine2, "float ", 0L);
            removeReplace(tempLine2, tempLine3, "int ", 0L);
            removeReplace(tempLine3, newLine, ") {", ")");

        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};

                findVariableNumberArray(line, number, array, type);
                sprintf(newLine, "%s = Array.new(%s)", array, number);
                if (beforeFunctions) {
                    addGlobal(array);
                }
                return 2;
            } else {
                char tempLine[LINELENGTH] = {0};
                char tempLine2[LINELENGTH] = {0};
                removeReplace(line, tempLine, "int ", 0L);
                removeReplace(tempLine, tempLine2, "float ", 0L);
                removeReplace(tempLine2, newLine, ") {", ")");
                if (beforeFunctions) {
                    addGlobal(tempLine2);
                }
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

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnzat ", "def ");
            removeReplace(tempLine, tempLine2, "float ", 0L);
            removeReplace(tempLine2, tempLine3, "int ", 0L);
            removeReplace(tempLine3, newLine, ") {", ")");
            return 2;
        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};
                findVariableNumberArray(line, number, array, type);
                sprintf(newLine, "%s = Array.new(%s)", array, number);
                if (beforeFunctions) {
                    addGlobal(array);
                }
                return 2;
            } else {
                char tempLine[LINELENGTH] = {0};
                char tempLine2[LINELENGTH] = {0};
                removeReplace(line, tempLine, "int ", 0L);
                removeReplace(tempLine, tempLine2, "float ", 0L);
                removeReplace(tempLine2, newLine, ") {", ")");
                if (beforeFunctions) {
                    addGlobal(tempLine2);
                }
                return 2;
            }

        }
        return 2;
    }
    
    
    if (lineCompare(line, "void")) {
        char tempLine[LINELENGTH] = {0};
        char temp2Line[LINELENGTH] = {0};
        char temp3Line[LINELENGTH] = {0};
        
        beforeFunctions = 0;

        removeReplace(line, tempLine, "void", "def");
        removeReplace(tempLine, temp2Line, "int ", 0L);
        removeReplace(temp2Line, temp3Line, "float ", 0L);
        removeReplace(temp3Line, newLine, ") {", ")");
    }
    return 2;
}


int nothingToPrintJava(char * line, char * newLine, int tabs, int noPrint) {
    
    if (noPrint && !outOfMain) {
        return 0;
    }
    
    if (lineCompare(line, "#")) {
        return 0;
    }
    if (lineCompare(line, "int main")) {
        outOfMain = 0;
        if (noPrint) {
            return 0;
        }
        removeReplace(line, newLine, "int main (void) ", "public static void main(String[] args) ");
        return 2;
    }
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
            removeReplace(line, tempLine, "printf(", "System.out.println(");
            removeReplace(tempLine, tempLine2, "\"%d\\n\", ", 0L);
            removeReplace(tempLine2, tempLine3, "\"%f\\n\", ", 0L);
            removeReplace(tempLine3, newLine, "\\n", 0L);
        } else {
            removeReplace(line, tempLine, "printf(", "System.out.println(");
            removeReplace(tempLine, newLine, "\\n", 0L);
        }
        return 2;
    }
    
    if (lineCompare(line, "//")) {
        return 1;
    }
    if (lineCompare(line, "if")) {
        return 1;
    }
    if (lineCompare(line, "}")) {
        return 1;
    }
    if (lineCompare(line, "while")) {
        return 1;
    }
    if (lineCompare(line, "const")) {
        if (beforeFunctions) {
            removeReplace(line, newLine, "const ", "public static final ");
        } else {
            removeReplace(line, newLine, "const ", "final ");
        }
        return 2;
    }
    if (lineCompare(line, "int")) {
        if (containsValue(line, '(')) {
            
            beforeFunctions = 0;

            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, newLine, "fnz ", "static int ");
            return 2;
        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};
                findVariableNumberArray(line, number, array, type);
                if (beforeFunctions) {
                    sprintf(newLine, "public static %s[] %s = new %s[%s];", type, array, type, number);
                } else {
                    sprintf(newLine, "%s[] %s = new %s[%s];", type, array, type, number);
                }
                return 2;
            }
            return 1;
        }
    }
    if (lineCompare(line, "float")) {
        if (containsValue(line, '(')) {
            
            beforeFunctions = 0;

            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, newLine, "fnzat ", "static float ");
            return 2;
        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};
                findVariableNumberArray(line, number, array, type);
                if (beforeFunctions) {
                    sprintf(newLine, "public static %s[] %s = new %s[%s];", type, array, type, number);
                } else {
                    sprintf(newLine, "%s[] %s = new %s[%s];", type, array, type, number);
                }
                return 2;
            }
            return 1;
        }
    }
    if (lineCompare(line, "void"))
    {
        beforeFunctions = 0;

        removeReplace(line, newLine, "void ", "static void ");
        return 2;
    }
    return 1;
}

int nothingToPrintJavaScript(char * line, char * newLine, int tabs, int noPrint) {
    
    if (noPrint && !outOfMain)
    {
        return 0;
    }
    
    if (lineCompare(line, "#"))
    {
        return 0;
    }
    if (lineCompare(line, "int main"))
    {
        outOfMain = 0;
        return 0;
    }
    if (lineCompare(line, "return"))
    {
        if (outOfMain)
        {
            return 1;
        }
        return 0;
    }
    if (lineCompare(line, "printf(\""))
    {
        if (noPrint) {
            return 0;
        }
        
        char tempLine[LINELENGTH] = {0};
        char tempLine2[LINELENGTH] = {0};
        char tempLine3[LINELENGTH] = {0};

        removeReplace(line, tempLine, "printf(", "console.log(");
        removeReplace(tempLine, tempLine2, "\"%d\\n\", ", 0L);
    	removeReplace(tempLine2, tempLine3, "\"%f\\n\", ", 0L);
        removeReplace(tempLine3, newLine, "\\n", 0L);
        return 2;
    }
    
    if (lineCompare(line, "//"))
    {
        return 1;
    }
    if (lineCompare(line, "if"))
    {
        return 1;
    }
    if (lineCompare(line, "}"))
    {
        if (outOfMain)
        {
            return 1;
        }
        return 0;
    }
    if (lineCompare(line, "while"))
    {
        return 1;
    }
    if (lineCompare(line, "const"))
    {
        char tempLine[LINELENGTH] = {0};
        removeReplace(line, tempLine, "float ", 0L);
        removeReplace(tempLine, newLine, "int ", 0L);
        return 2;
    }
    if (lineCompare(line, "int"))
    {
        if (containsValue(line, '(')) {
            char tempLine[LINELENGTH] = {0};
            
            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnz", "function");
            removeReplace(tempLine, newLine, "int ", 0L);
        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};
                findVariableNumberArray(line, number, array, type);
                sprintf(newLine, "var %s = [%s];", array, number);
            } else {
                int loop = 0;
                while (loop < LINELENGTH) {
                    newLine[loop] = line[loop];
                    loop++;
                }
                newLine[0] = 'v';
                newLine[1] = 'a';
                newLine[2] = 'r';
            }
        }
        return 2;
    }
    if (lineCompare(line, "float"))
    {
        if (containsValue(line, '(')) {
            char tempLine[LINELENGTH] = {0};
            char tempLine2[LINELENGTH] = {0};

            beforeFunctions = 0;
            
            line[0] = 'f';
            line[1] = 'n';
            line[2] = 'z';
            
            removeReplace(line, tempLine, "fnzat", "function");
            removeReplace(tempLine, tempLine2, "int ", 0L);
            removeReplace(tempLine2, newLine, "float ", 0L);
        } else {
            if (containsValue(line, '[')) {
                char array[LINELENGTH] = {0};
                char number[LINELENGTH] = {0};
                char type[LINELENGTH] = {0};
                findVariableNumberArray(line, number, array, type);
                sprintf(newLine, "var %s = [%s];", array, number);
            } else {
                removeReplace(line, newLine, "float ", "var ");
            }
        }
        return 2;
    }
    if (lineCompare(line, "void"))
    {
        char tempLine[LINELENGTH] = {0};
        char tempLine2[LINELENGTH] = {0};

        beforeFunctions = 0;
        
        removeReplace(line, tempLine, "void", "function");
        removeReplace(tempLine, tempLine2, "int ", 0L);
        removeReplace(tempLine2, newLine, "float ", 0L);
        return 2;
    }
    return 1;
}


void translateFile(char* filename, char* writefilename, int noPrint, fileHandler * fileHander, fileHandler * postProcess, char * className, openEndFile * openEnd) {
    int tabs;
    int loop = 0;
    char lineArray[LINELENGTH];
    int lineArrayFound = 0;
    
    FILE* reading = fopen(filename, "rb");
    FILE* writing = fopen(writefilename, "w");

    clearLineArray(lineArray);
    
    if (className) {
        tabs = 1;
    } else {
        tabs = 0;
    }
    
    if (openEnd) {
        (*openEnd)(writing, 1, className);
    }
    
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
                int value = (*fileHander)(lineArray, newLine, tabs, noPrint);
                
                if (postProcess) {
                    copyLineArray(lineArray, newLine);
                    int newValue = (*postProcess)(lineArray, newLine, tabs, noPrint);
                    if ((value == 2) || (newValue == 2)) {
                        value = 2;
                    }
                }
                
                if (writing) {
                    if (value != 0) {
                        if (outOfMain || className) {
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

                if (className) {
                    tabs = 1;
                } else {
                    tabs = 0;
                }
                
                clearLineArray(lineArray);
                clearLineArray(newLine);
                lineArrayFound = 0;
                loop = 0;
            }
        } while (!feof(reading));
        
        if (openEnd) {
            (*openEnd)(writing, 0, className);
        }

        fclose(reading);
        fclose(writing);
    }
}


int parseArgs(int argc, const char * argv[], char** csource, char** python, char** javascript, char** java, char** ruby, int * noPrint) {
    int loop = 1;
    int returnValue = 0;
    
    *noPrint = 0;
    
    while (loop < argc) {
        const char* row = argv[loop];
        if (row[0] == '-') {
            if (row[1] == 'j' && row[2] == 's') {
                loop++;
                *javascript = (char*)argv[loop];
                returnValue = 1;
            }
            if (row[1] == 'j' && row[2] == 0) {
                loop++;
                *java = (char*)argv[loop];
                returnValue = 1;
            }
            if (row[1] == 'p') {
                loop++;
                *python = (char*)argv[loop];
                returnValue = 1;
            }
            if (row[1] == 'r') {
                loop++;
                *ruby = (char*)argv[loop];
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

void getClassName(char * java, char * className) {
    int loop = 0;
    do {
        className[loop] = java[loop];
        loop++;
    } while (java[loop] != '.');
}

int main(int argc, const char * argv[]) {
    char* python = 0L;
    char* javascript = 0L;
    char* csource = 0;
    char* java = 0L;
    char* ruby = 0L;
    char className[LINELENGTH] = {0};

    int noPrint;
    if (parseArgs(argc, argv, &csource, &python, &javascript, &java, &ruby, &noPrint)) {
        if (python) {
            printf("python : %s\n", python);
        }
        if (csource) {
            printf("csource : %s\n", csource);
        }
        if (javascript) {
            printf("javascript : %s\n", javascript);
        }
        
        if (ruby) {
            printf("ruby : %s\n", ruby);
        }

        if (java) {
            printf("java : %s\n", java);
            getClassName(java, className);
            printf("class name : %s\n", className);
        }
        
        if (noPrint) {
            printf("No Print ON\n");
        }
        
        
        if (csource && javascript) {
            translateFile(csource, javascript, noPrint, &nothingToPrintJavaScript, 0L, 0L, 0L);
        }
        if (csource && python) {
            translateFile(csource, python, noPrint, &nothingToPrintPython, 0L, 0L, 0L);
        }
        if (csource && java) {
            translateFile(csource, java, noPrint, &nothingToPrintJava, 0L, className, &openEndJava);
        }
        if (csource && ruby) {
            clearGlobals();
            translateFile(csource, ruby, noPrint, &nothingToPrintRuby, &nothingToPrintRubyPostProcess, 0L, 0L);
        }
    }
    return 0;
}
