#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Amit Patel

// All names have been converted to integers by converting each character to their ASCII value, multiplying them and adding each value together

// Keeps track of the temporary variables and their values
// Whenever a new temporary variable is made, it is inserted to the end of this linked list
struct tempVariableNode{
    int name;
    int value;
    struct tempVariableNode* next;
}tempVariableNode;

// The struct that will help us keep track of which gates are being used
// Inputs keeps track of which variables are the inputs to this gate
// Outputs keeps track of which variables are the outputs to get gate
// Type should be ascii value of INPUT, OUTPUT, NOT, AND, OR, NAND, NOR, XOR, DECODER, MULTIPLEXER, or PASS
// Size should vary if the type is a DECODER or a MULTIPLEXER
// Multi keeps track of selectors for multiplexer
// The next field points to which gate is after the current gate
struct gateNode{
    int type;
    int* inputs;
    int* outputs;
    int size;
    int* multi;
    struct gateNode* next;
}gateNode;

// Global Variables

// Names for the input variables
int* inputNames;

// String names for the input variables
char** stringInputNames;

// Names for the output variables
int* outputNames;

// String names for the output variables
char** stringOutputNames;

// Values for the input variables
int* inputValues;

// Values for the output variables
int* outputValues;

// Linked list for the circuit
struct gateNode* circuitBeginning = NULL;

// Linked list for the temporary variables
struct tempVariableNode* tempVarBeginning = NULL;

// Create a new gate node and insert it into the circuit linked list
void insertGateNode(int command, int* inputs, int* outputs, int size, int* multi)
{
    // Check if the circuit is currently empty
    if (circuitBeginning == NULL)
    {
        struct gateNode* tempPointer = malloc(sizeof(gateNode));
        tempPointer->type = command;
        tempPointer->inputs = inputs;
        tempPointer->outputs = outputs;
        tempPointer->size = size;
        tempPointer->multi = multi;
        tempPointer->next = NULL;

        circuitBeginning = tempPointer;
    }
    else
    {
        struct gateNode *current = circuitBeginning;
        struct gateNode *previous = circuitBeginning;

        while(current != NULL)
        {
            previous = current;
            current = current->next;
        }

        struct gateNode* tempPointer = malloc(sizeof(gateNode));
        tempPointer->type = command;
        tempPointer->inputs = inputs;
        tempPointer->outputs = outputs;
        tempPointer->size = size;
        tempPointer->multi = multi;
        tempPointer->next = NULL;
        
        previous->next = tempPointer;
    }
}

// Get the value of name
// Check if it is an input variable
// If so, return its value, otherwise check the temporary variable linked list and then return that value
int getValueOfVariable(int name, int amountOfInputs)
{
    // First check if it is an input variable
    for (int i = 0; i < amountOfInputs; i++)
    {
        if (inputNames[i] == name)
        {
            return inputValues[i];
        }
    }

    // If unsuccessful, look through the temporary variable linked list
    struct tempVariableNode* tempVarFinder = tempVarBeginning;
    while(tempVarFinder != NULL)
    {
        if (tempVarFinder->name == name)
        {
            return tempVarFinder->value;
        }
        tempVarFinder = tempVarFinder->next;
    }

    // Check if input is a constant 0 or 1
    if (name == 48)
    {
        return 0;
    }
    if (name == 49)
    {
        return 1;
    }

    // If not found, something went wrong, return 0
    return 0;
}

// Set the value for the variable with the given name
// Check if it is an output variable
// If so, set the value, otherwise check the temporary linked list and then set the value
void setValueofVariable(int name, int value, int amountOfOutputs)
{
    // First check if it is an output variable
    for (int i = 0; i < amountOfOutputs; i++)
    {
        if (outputNames[i] == name)
        {
            outputValues[i] = value;
            return;
        }
    }

    // If unsuccessful, look through the temporary variable linked list
    struct tempVariableNode* tempVarFinder = tempVarBeginning;
    while(tempVarFinder != NULL)
    {
        if (tempVarFinder->name == name)
        {
            tempVarFinder->value = value;
            return;
        }
        tempVarFinder = tempVarFinder->next;
    }
}

// This method will check if an output from a circuit is a temporary variable when the circuit is being built
// If it is, it is added to the temporary variable linked list
// If it is not, then means it is an output and we do nothing with it
void checkIfTempVariable(int name, int amountOfOutputs)
{
    // Check if the name is a final output and not a temporary variable
    for (int i = 0; i < amountOfOutputs; i++)
    {
        if (outputNames[i] == name)
        {
            return;
        }
    }

    // If the name is not found in the output names, that means it is a temporary variable
    // Check if the temporary variable linked list is currently empty
    if (tempVarBeginning == NULL)
    {
        struct tempVariableNode* tempPointer = malloc(sizeof(gateNode));
        tempPointer->name = name;
        tempPointer->value = 0;
        tempPointer->next = NULL;

        tempVarBeginning = tempPointer;
    }
    else
    {
        struct tempVariableNode* current = tempVarBeginning;
        struct tempVariableNode* previous = tempVarBeginning;

        while(current != NULL)
        {
            previous = current;
            current = current->next;
        }

        struct tempVariableNode* tempPointer = malloc(sizeof(gateNode));
        tempPointer->name = name;
        tempPointer->value = 0;
        tempPointer->next = NULL;
        
        previous->next = tempPointer;
    }
}

// This method will be used to help us create a binary respreentation used for decoder and multiplexer gates
int concatInts(int inputA, int inputB)
{
    char inputAString[20];
    char inputBString[20];

    sprintf(inputAString, "%d", inputA);
    sprintf(inputBString, "%d", inputB);

    strcat(inputAString, inputBString);

    return atoi(inputAString);
}

// This method turns a binary number into a decimal number
int binaryToDecimal(int binaryNumber)
{
    int decimalValue = 0;
    int i = 0;
    int remainder;
    int power;

    while(binaryNumber != 0)
    {
        power = 1;
        remainder = binaryNumber % 10;
        binaryNumber /= 10;
        for (int j = 0; j < i; j++)
        {
            power *= 2;
        }
        decimalValue += remainder * power;
        i++;
    }

    return decimalValue;
}

// This is the main method
int main(int argc, char **argv)
{
    // Only need name of the file with the circuits
    if (argc != 2)
    {
        exit(1);
    }

    // Open file, check if it exists
    FILE *circuitFile = fopen(argv[1], "r");

    if (circuitFile == NULL)
    {
        exit(1);
    }

    char command[20];
    int amountOfInputs;
    int amountOfOutputs;

    /* ================================================ Scanning Section ================================================ */
    // Begin scanning the file
    // Scan inputs
    if (!fscanf(circuitFile, "%16s", command))
    {
        exit(1);
    }
    if(!fscanf(circuitFile, "%d", &amountOfInputs))
    {
        exit(1);
    }

    inputNames = malloc(amountOfInputs*sizeof(int));
    inputValues = malloc(amountOfInputs*sizeof(int));
    stringInputNames = malloc(amountOfInputs*20);
    
    // Scan through the input variables
    for (int i = 0; i < amountOfInputs; i++)
    {
        char inputVariable[20];
        fscanf(circuitFile, "%16s", inputVariable);
        stringInputNames[i] = malloc(20);
        strcpy(stringInputNames[i], inputVariable);

        inputNames[i] = 0;
        for (int j = 0; inputVariable[j] != '\0'; j++)
        {
            inputNames[i] += inputVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
        }

        inputNames[i] *= 20;

        inputValues[i] = 0;
    }

    // Scan outputs
    if (!fscanf(circuitFile, "%16s", command))
    {
        exit(1);
    }
    if(!fscanf(circuitFile, "%d", &amountOfOutputs))
    {
        exit(1);
    }

    outputNames = malloc(amountOfOutputs*sizeof(int));
    outputValues = malloc(amountOfOutputs*sizeof(int));
    stringOutputNames = malloc(amountOfOutputs*20);
    
    // Scan through the output variables
    for (int i = 0; i < amountOfOutputs; i++)
    {
        char outputVariable[20];
        fscanf(circuitFile, "%16s", outputVariable);
        stringOutputNames[i] = malloc(20);
        strcpy(stringOutputNames[i], outputVariable);

        outputNames[i] = 0;
        for (int j = 0; outputVariable[j] != '\0'; j++)
        {
            outputNames[i] += outputVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
        }

        outputValues[i] = 0;
    }

    /* ================================================ Building Circuit Section ================================================ */
    // Begin scanning the gates and building the circuit
    while (fscanf(circuitFile, "%16s", command) == 1)
    {
        // Gate is a AND, OR, NAND, NOR or XOR gate
        // 2 inputs, 1 output
        if (strcmp(command,"AND") == 0 || strcmp(command,"OR") == 0 || strcmp(command,"NAND") == 0 || strcmp(command,"NOR") == 0 || strcmp(command,"XOR") == 0)
        {
            // Get inputs
            int* inputs = calloc(2*sizeof(int), sizeof(int));
            for (int i = 0; i < 2; i++)
            {
                char inputVariable[20];
                if(!fscanf(circuitFile, "%16s", inputVariable))
                {
                    exit(1);
                }

                for (int j = 0; inputVariable[j] != '\0'; j++)
                {
                    inputs[i] += inputVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
                }

                // Check if the input scanned in is an input variable, a temporary variable, or a 0 or 1
                if (strcmp(inputVariable, "0") == 0 || strcmp(inputVariable, "1") == 0)
                {
                    inputs[i] /= 2;
                    continue;
                }
                else
                {
                    int isTemporaryVariable = 0;
                    for (int j = 0; j < amountOfInputs; j++)
                    {
                        if (strcmp(inputVariable, stringInputNames[j]) == 0)
                        {
                            isTemporaryVariable = 1;
                            inputs[i] *= 20;
                            break;
                        }
                    }
                    if (isTemporaryVariable == 0)
                    {
                        inputs[i] *= -1;
                    }
                }
            }

            // Get output
            int* outputs = calloc(sizeof(int), sizeof(int));
            char outputVariable[20];
            if(!fscanf(circuitFile, "%16s", outputVariable))
            {
                exit(1);
            }
            for (int i = 0; outputVariable[i] != '\0'; i++)
            {
                outputs[0] += outputVariable[i] * ((i+1)*(i+2)*(i+1)*(i+1));
            }

            // Check to see if the output is an output or temporary variable and scale the value accordingly
            int isTemporaryVariable = 0;
            for (int i = 0; i < amountOfOutputs; i++)
            {
                if (strcmp(outputVariable, stringOutputNames[i]) == 0)
                {
                    isTemporaryVariable = 1;
                    break;
                }
            }
            if (isTemporaryVariable == 0)
            {
                outputs[0] *= -1;
            }

            int size = 2;
            int* multi = NULL;

            // Convert command into an ascii value that will be used in the switch statement
            int asciiValue = 0;
            for (int j = 0; command[j] != '\0'; j++)
            {
                asciiValue += command[j];
            }

            // Build gate and add it to gate linked list
            insertGateNode(asciiValue, inputs, outputs, size, multi);

            // Check if any of the outputs are temporary variables and add them to the temporary linked list if they are
            checkIfTempVariable(outputs[0], amountOfOutputs);
        }
        else if (strcmp(command,"NOT") == 0 || strcmp(command,"PASS") == 0)
        {
            // Get input
            int* inputs = calloc(sizeof(int), sizeof(int));
            char inputVariable[20];
            if(!fscanf(circuitFile, "%16s", inputVariable))
            {
                exit(1);
            }
            for (int i = 0; inputVariable[i] != '\0'; i++)
            {
                inputs[0] += inputVariable[i] * ((i+1)*(i+2)*(i+1)*(i+1));
            }

            // Check if the input scanned in is an input variable, a temporary variable, or a 0 or 1
            if (strcmp(inputVariable, "0") == 0|| strcmp(inputVariable, "1") == 0)
            {
                inputs[0] /= 2;
                continue;
            }
            else
            {
                int isTemporaryVariable = 0;
                for (int j = 0; j < amountOfInputs; j++)
                {
                    if (strcmp(inputVariable, stringInputNames[j]) == 0)
                    {
                        isTemporaryVariable = 1;
                        inputs[0] *= 20;
                        break;
                    }
                }
                if (isTemporaryVariable == 0)
                {
                    inputs[0] *= -1;
                }
            }

            // Get output
            int* outputs = calloc(sizeof(int), sizeof(int));
            char outputVariable[20];
            if(!fscanf(circuitFile, "%16s", outputVariable))
            {
                exit(1);
            }
            for (int i = 0; outputVariable[i] != '\0'; i++)
            {
                outputs[0] += outputVariable[i] * ((i+1)*(i+2)*(i+1)*(i+1));
            }

            // Check to see if the output is an output or temporary variable and scale the value accordingly
            int isTemporaryVariable = 0;
            for (int i = 0; i < amountOfOutputs; i++)
            {
                if (strcmp(outputVariable, stringOutputNames[i]) == 0)
                {
                    isTemporaryVariable = 1;
                    break;
                }
            }
            if (isTemporaryVariable == 0)
            {
                outputs[0] *= -1;
            }

            int size = 1;
            int* multi = NULL;

            // Convert command into an ascii value that will be used in the switch statement
            int asciiValue = 0;
            for (int j = 0; command[j] != '\0'; j++)
            {
                asciiValue += command[j];
            }

            // Build gate and add it to gate linked list
            insertGateNode(asciiValue, inputs, outputs, size, multi);

            // Check if any of the outputs are temporary variables and add them to the temporary linked list if they are
            checkIfTempVariable(outputs[0], amountOfOutputs);
        }
        /* ================================================ Building Circuit Decoder Section ================================================ */
        else if (strcmp(command,"DECODER") == 0)
        {
            // Get size of the decoder
            int size;
            if(!fscanf(circuitFile, "%d", &size))
            {
                exit(1);
            }

            // Get inputs of the decoder
            int* inputs = calloc(size*sizeof(int), sizeof(int));
            for (int i = 0; i < size; i++)
            {
                char inputVariable[20];
                if(!fscanf(circuitFile, "%16s", inputVariable))
                {
                    exit(1);
                }

                for (int j = 0; inputVariable[j] != '\0'; j++)
                {
                    inputs[i] += inputVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
                }

                // Check if the input scanned in is an input variable, a temporary variable, or a 0 or 1
                if (strcmp(inputVariable, "0") == 0|| strcmp(inputVariable, "1") == 0)
                {
                    inputs[i] /= 2;
                    continue;
                }
                else
                {
                    int isTemporaryVariable = 0;
                    for (int j = 0; j < amountOfInputs; j++)
                    {
                        if (strcmp(inputVariable, stringInputNames[j]) == 0)
                        {
                            isTemporaryVariable = 1;
                            inputs[i] *= 20;
                            break;
                        }
                    }
                    if (isTemporaryVariable == 0)
                    {
                        inputs[i] *= -1;
                    }
                }
            }

            // Get outputs of the decoder
            int decoderOutputAmount = 1;
            for(int i = 0; i < size; i++)
            {
                decoderOutputAmount *= 2;
            }

            int* outputs = calloc(decoderOutputAmount*sizeof(int), sizeof(int));
            for (int i = 0; i < decoderOutputAmount; i++)
            {
                char outputVariable[20];
                if(!fscanf(circuitFile, "%16s", outputVariable))
                {
                    exit(1);
                }
                for (int j = 0; outputVariable[j] != '\0'; j++)
                {
                    outputs[i] += outputVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
                }

                int isTemporaryVariable = 0;
                for (int j = 0; j < amountOfOutputs; j++)
                {
                    if (strcmp(outputVariable, stringOutputNames[j]) == 0)
                    {
                        isTemporaryVariable = 1;
                        break;
                    }
                }
                if (isTemporaryVariable == 0)
                {
                    outputs[i] *= -1;
                }
            }

            int* multi = NULL;

            // Convert command into an ascii value that will be used in the switch statement
            int asciiValue = 0;
            for (int j = 0; command[j] != '\0'; j++)
            {
                asciiValue += command[j];
            }

            // Build gate and add it to gate linked list
            insertGateNode(asciiValue, inputs, outputs, size, multi);

            // Check if any of the outputs are temporary variables and add them to the temporary linked list if they are
            for (int i = 0; i < decoderOutputAmount; i++)
            {
                checkIfTempVariable(outputs[i], amountOfOutputs);
            }
        }
        /* ================================================ Building Circuit Multiplexer Section ================================================ */
        else if (strcmp(command,"MULTIPLEXER") == 0)
        {
            // Get size of the multiplexer
            int size;
            if(!fscanf(circuitFile, "%d", &size))
            {
                exit(1);
            }

            // Get inputs of the multiplexer
            int multiplexerInputAmount = 1;
            for(int i = 0; i < size; i++)
            {
                multiplexerInputAmount *= 2;
            }

            int* inputs = calloc(multiplexerInputAmount*sizeof(int), sizeof(int));
            for (int i = 0; i < multiplexerInputAmount; i++)
            {
                char inputVariable[20];
                if(!fscanf(circuitFile, "%16s", inputVariable))
                {
                    exit(1);
                }

                for (int j = 0; inputVariable[j] != '\0'; j++)
                {
                    inputs[i] += inputVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
                }

                // Check if the input scanned in is an input variable, a temporary variable, or a 0 or 1
                if (strcmp(inputVariable, "0") == 0|| strcmp(inputVariable, "1") == 0)
                {
                    inputs[i] /= 2;
                    continue;
                }
                else
                {
                    int isTemporaryVariable = 0;
                    for (int j = 0; j < amountOfInputs; j++)
                    {
                        if (strcmp(inputVariable, stringInputNames[j]) == 0)
                        {
                            isTemporaryVariable = 1;
                            inputs[i] *= 20;
                            break;
                        }
                    }
                    if (isTemporaryVariable == 0)
                    {
                        inputs[i] *= -1;
                    }
                }
            }

            // Get selectors of the multiplexer
            int* multi = calloc(size*sizeof(int), sizeof(int));
            for (int i = 0; i < size; i++)
            {
                char multiVariable[20];
                if(!fscanf(circuitFile, "%16s", multiVariable))
                {
                    exit(1);
                }

                for (int j = 0; multiVariable[j] != '\0'; j++)
                {
                    multi[i] += multiVariable[j] * ((j+1)*(j+2)*(j+1)*(j+1));
                }

                // Check if the input scanned in is an input variable, a temporary variable, or a 0 or 1
                if (strcmp(multiVariable, "0") == 0|| strcmp(multiVariable, "1") == 0)
                {
                    multi[i] /= 2;
                    continue;
                }
                else
                {
                    int isTemporaryVariable = 0;
                    for (int j = 0; j < amountOfInputs; j++)
                    {
                        if (strcmp(multiVariable, stringInputNames[j]) == 0)
                        {
                            isTemporaryVariable = 1;
                            multi[i] *= 20;
                            break;
                        }
                    }
                    if (isTemporaryVariable == 0)
                    {
                        multi[i] *= -1;
                    }
                }
            }

            // Get outputs of the multiplexer
            int* outputs = calloc(sizeof(int), sizeof(int));
            char outputVariable[20];
            if(!fscanf(circuitFile, "%16s", outputVariable))
            {
                exit(1);
            }
            for (int i = 0; outputVariable[i] != '\0'; i++)
            {
                outputs[0] += outputVariable[i] * ((i+1)*(i+2)*(i+1)*(i+1));
            }

            // Check to see if the output is an output or temporary variable and scale the value accordingly
            int isTemporaryVariable = 0;
            for (int i = 0; i < amountOfOutputs; i++)
            {
                if (strcmp(outputVariable, stringOutputNames[i]) == 0)
                {
                    isTemporaryVariable = 1;
                    break;
                }
            }
            if (isTemporaryVariable == 0)
            {
                outputs[0] *= -1;
            }

            // Convert command into an ascii value that will be used in the switch statement
            int asciiValue = 0;
            for (int j = 0; command[j] != '\0'; j++)
            {
                asciiValue += command[j];
            }

            // Build gate and add it to gate linked list
            insertGateNode(asciiValue, inputs, outputs, size, multi);

            // Check if any of the outputs are temporary variables and add them to the temporary linked list if they are
            checkIfTempVariable(outputs[0], amountOfOutputs);
        }
        else
        {
            // Command is invalid, exit program
            exit(1);
        }
    }

    /* ================================================ Printing Truth Table Section ================================================ */

    // Calculate how many rows we will need to print, which should be 2 ^ amountOfInputs
    int numberOfRows = 1;
    for(int i = 0; i < amountOfInputs; i++)
    {
        numberOfRows *= 2;
    }

    // These variables are for decoder and multiplexer gates
    int inputsDecoder[500];
    int selectorsMultiplexer[500];
    int decoderWhichGateIsValue1 = 0;
    int multiplexerWhichInputIsOutput = 0;

    for (int i = 0; i < numberOfRows; i++)
    {
        // Print the values of the inputs for the current row
        for (int j = 0; j < amountOfInputs; j++)
        {
            printf("%d ", inputValues[j]);
        }

        // Print the column divider
        printf("|");

        // Begin going through the gate
        struct gateNode* currentGate = circuitBeginning;
        int input1;
        int input2;
        int output1;

        int binaryNumber = 0;

        while(currentGate != NULL)
        {
            // Get value of first input
            input1 = getValueOfVariable(currentGate->inputs[0], amountOfInputs);

            // Using switch instead of if/else for faster speed
            switch(currentGate->type)
            {
                /* ====================================== AND Gate ====================================== */
                case 211:
                    // Similar procedure for second input
                    input2 = getValueOfVariable(currentGate->inputs[1], amountOfInputs);

                    if (input1 == 1 && input2 == 1)
                    {
                        output1 = 1;
                    }
                    else
                    {
                        output1 = 0;
                    }

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== OR Gate ====================================== */
                case 161:
                    // Similar procedure for second input
                    input2 = getValueOfVariable(currentGate->inputs[1], amountOfInputs);

                    if (input1 == 1 || input2 == 1)
                    {
                        output1 = 1;
                    }
                    else
                    {
                        output1 = 0;
                    }

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== NOT Gate ====================================== */
                case 241:
                    if (input1 == 1)
                    {
                        output1 = 0;
                    }
                    else
                    {
                        output1 = 1;
                    }

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== NAND Gate ====================================== */
                case 289:
                    // Similar procedure for second input
                    input2 = getValueOfVariable(currentGate->inputs[1], amountOfInputs);

                    if (input1 == 1 && input2 == 1)
                    {
                        output1 = 0;
                    }
                    else
                    {
                        output1 = 1;
                    }

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== NOR Gate ====================================== */
                case 239:
                    // Similar procedure for second input
                    input2 = getValueOfVariable(currentGate->inputs[1], amountOfInputs);

                    if (input1 == 0 && input2 == 0)
                    {
                        output1 = 1;
                    }
                    else
                    {
                        output1 = 0;
                    }

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== XOR Gate ====================================== */
                case 249:
                    // Similar procedure for second input
                    input2 = getValueOfVariable(currentGate->inputs[1], amountOfInputs);

                    if (input1 == input2)
                    {
                        output1 = 0;
                    }
                    else
                    {
                        output1 = 1;
                    }

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== PASS Gate ====================================== */
                case 311:
                    output1 = input1;

                    setValueofVariable(currentGate->outputs[0], output1, amountOfOutputs);
                    break;
                /* ====================================== DECODER Gate ====================================== */
                case 502:
                    for (int k = 1; k < currentGate->size; k++)
                    {
                        inputsDecoder[k] = currentGate->inputs[k];
                    }

                    if (currentGate->size == 1)
                    {
                        decoderWhichGateIsValue1 = input1;
                    }
                    else
                    {
                        binaryNumber = concatInts(input1, getValueOfVariable(inputsDecoder[1], amountOfInputs));

                        for (int k = 2; k < currentGate->size; k++)
                        {
                            binaryNumber = concatInts(binaryNumber, getValueOfVariable(inputsDecoder[k], amountOfInputs));
                        }

                        decoderWhichGateIsValue1 = binaryToDecimal(binaryNumber);
                    }

                    for (int k = 0; k < (2 << (currentGate->size - 1)); k++)
                    {
                        setValueofVariable(currentGate->outputs[k], 0, amountOfOutputs);
                    }

                    setValueofVariable(currentGate->outputs[decoderWhichGateIsValue1], 1, amountOfOutputs);

                    break;
                /* ====================================== MULTIPLEXER Gate ====================================== */
                case 859:
                    for (int k = 0; k < currentGate->size; k++)
                    {
                        selectorsMultiplexer[k] = currentGate->multi[k];
                    }

                    if (currentGate->size == 1)
                    {
                        multiplexerWhichInputIsOutput = getValueOfVariable(selectorsMultiplexer[0], amountOfInputs);
                    }
                    else
                    {
                        binaryNumber = concatInts(getValueOfVariable(selectorsMultiplexer[0], amountOfInputs), getValueOfVariable(selectorsMultiplexer[1], amountOfInputs));

                        for (int k = 2; k < currentGate->size; k++)
                        {
                            binaryNumber = concatInts(binaryNumber, getValueOfVariable(selectorsMultiplexer[k], amountOfInputs));
                        }

                        multiplexerWhichInputIsOutput = binaryToDecimal(binaryNumber);
                    }

                    setValueofVariable(currentGate->outputs[0], getValueOfVariable(currentGate->inputs[multiplexerWhichInputIsOutput], amountOfInputs), amountOfOutputs);

                    break;
            }

            currentGate = currentGate->next;
        }

        //Print the output values
        for (int j = 0; j < amountOfOutputs; j++)
        {
            printf(" %d", outputValues[j]);
        }

        // Print a new line
        printf("\n");

        // Add 1 to the last index of the input values
        // This will help us print more rows
        inputValues[amountOfInputs-1]++;

        // Check to see if any of the values became 2
        // If so, set the index to 0 and add 1 to the index before it
        // No need to check the first index, that should never become 2
        int counter = 0;
        while(counter != amountOfInputs-1)
        {
            for (int j = 1; j < amountOfInputs; j++)
            {
                if (inputValues[j] == 2)
                {
                    inputValues[j] = 0;
                    inputValues[j-1]++;
                    counter = 0;
                    break;
                }
                else
                {
                    counter++;
                }
            }
        }
    }

    /* ================================================ Free Memory Section ================================================ */
    
    for (int i = 0; i < amountOfInputs; i++)
    {
        free(stringInputNames[i]);
    }

    for (int i = 0; i < amountOfOutputs; i++)
    {
        free(stringOutputNames[i]);
    }

    free(stringInputNames);
    free(stringOutputNames);
    free(inputNames);
    free(outputNames);
    free(inputValues);
    free(outputValues);

    // Free gate nodes
    struct gateNode* current = circuitBeginning;

    while (current != NULL)
    {
        struct gateNode* previous = current;
        current = current->next;

        if (previous->type == 859)
        {
            free(previous->multi);
        }

        free(previous->inputs);
        free(previous->outputs);
        free(previous);
    }

    // Free temporary variable nodes
    struct tempVariableNode* currentTemp = tempVarBeginning;

    while (currentTemp != NULL)
    {
        struct tempVariableNode* previous = currentTemp;
        currentTemp = currentTemp->next;
        free(previous);
    }

    exit(0);
}