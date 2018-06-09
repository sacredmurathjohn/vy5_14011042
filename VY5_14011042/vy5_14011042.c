#include <stdio.h>
#include <stdlib.h>

#define SIZE 500

int expCount;

typedef struct EXPRESSION
{
    char expression[512];
    char alphabetNumber;
    int  value;
} Expression;

typedef struct STACK
{
    char array[SIZE][6];
    int count;
} Stack;

Stack initializeStack();
void push(Stack *, char *);
char * pop(Stack *);
int isEmpty(Stack);
int isFull(Stack);
char * lookAt(Stack);
Expression * loadFromFile(char *);
int computeBinaryOperation(char, int, int);

// Initialize the stack.
Stack initializeStack()
{
    int i;
    Stack s;
    for (i = 0; i < SIZE; i++) strcpy(s.array[i], "\0");
    s.count = 0;
    return s;
}

void push(Stack * s, char * val)
{
    if(!isFull(*s))
        strcpy(s->array[s->count++], val);
    else
        printf("Stack is full.\n");
}

char * pop(Stack * s)
{
    char * val;
    if(!isEmpty(*s))
    {
        val = s->array[--(s->count)];
        return val;
    }
    else
        printf("Stack is empty.\n");
}

char * lookAt(Stack s)
{
    char * val;
    if(!isEmpty(s))
    {
        val = s.array[s.count - 1];
        return val;
    }
    else
    {
        return "";
    }
}

int isEmpty(Stack s)
{
    if(s.count == 0)
        return 1;
    else
        return 0;
}

int isFull(Stack s)
{
    if(s.count == SIZE)
        return 1;
    else
        return 0;
}

// Check that the value is an integer or not.
int isInteger(char * str)
{
    if(strstr(str, "0") != NULL ||
            strstr(str, "1") != NULL ||
            strstr(str, "2") != NULL ||
            strstr(str, "3") != NULL ||
            strstr(str, "4") != NULL ||
            strstr(str, "5") != NULL ||
            strstr(str, "6") != NULL ||
            strstr(str, "7") != NULL ||
            strstr(str, "8") != NULL ||
            strstr(str, "9") != NULL)
    {
        return 1;
    }
    else return 0;
}

Expression * loadFromFile(char * path)
{
    int i;
    Expression * eval;
    eval = (Expression *) malloc(sizeof(Expression));
    FILE * f = fopen(path, "r");
    if(f != NULL)
    {
        // Expression values are stored in a struct type named Expression.
        while(!feof(f))
        {
            fgets(eval[expCount].expression, "%s", f);
            eval[expCount].value = -255;
            printf("%s", eval[(expCount++)].expression);
            eval = (Expression *) realloc(eval, (expCount + 1) * sizeof(Expression));
        }
        for( i = 0; i < expCount; i++ )
        {
            char * token, * token2, * token3;
            token = strtok(eval[i].expression, "=");
            token2 = strtok(NULL, "=");
            token3 = strtok(token, " ");
            eval[i].alphabetNumber = token3[0];
            strcpy(eval[i].expression, token2);
        }

        printf("\nLoaded %d expressions.\n", expCount);
        return eval;
    }
    else
    {
        printf("File not found.\n");
    }
}

void showExpression(Expression e)
{
    printf("%c =%s", e.alphabetNumber, e.expression);
    if(e.value != -255) printf(" (Equals %d)", e.value);
    printf("\n");
}

Expression infixToPostfix(Expression e, Stack s)
{

    // Have some expression values to determine.

    int i;
    int counter = 0;
    //int block = 0;
    char expressionValues[64][64];
    char newExpression[512];
    strcpy(newExpression, " "); // For to equation have a single start of space.
    char * token;
    token = strtok(e.expression, " \r\n\t");
    strcpy(expressionValues[counter++], token);
    while( token != NULL )
    {
        token = strtok(NULL, " \r\n\t");
        if(token != NULL && strstr(token, ";") == NULL)
        {
            strcpy(expressionValues[counter++], token);
        }
    }

    // The transition begins.
    for(i = 0; i < counter; i++)
    {

        if(strcmp(expressionValues[i], "(") == 0)
        {
            push(&s, expressionValues[i]);
        }
        else if(strcmp(expressionValues[i], ")") == 0)
        {
            while(!isEmpty(s) && strcmp(lookAt(s), "(") != 0)
            {
                char * exp = pop(&s);
                if(strcmp(exp, "(") != 0 && strcmp(exp, ")") != 0)
                {
                    strcat(exp, " ");
                    strcat(newExpression, exp);
                }
            }
            if(strcmp(lookAt(s), "(") == 0) pop(&s);
            // Check for higher priorities.
        }
        else if(strcmp(expressionValues[i], "*") == 0 || strcmp(expressionValues[i], "/") == 0)
        {
            char * look = lookAt(s);
            push(&s, expressionValues[i]);
            // Check for lesser priorities.
        }
        else if(strcmp(expressionValues[i], "+") == 0 || strcmp(expressionValues[i], "-") == 0)
        {
            char * look = lookAt(s);
            while(!isEmpty(s) && (strcmp(look, "*") == 0 || strcmp(look, "/") == 0))
            {
                char * exp = pop(&s);
                strcat(exp, " ");
                strcat(newExpression, exp);
                look = lookAt(s);
            }
            push(&s, expressionValues[i]);

            // Check for symbols or numbers.
        }
        else
        {
            char * exp = expressionValues[i];
            strcat(exp, " ");
            strcat(newExpression, exp);
        }
    }

    // The remaining stack is emptied.
    while(!isEmpty(s))
    {
        char * exp = pop(&s);
        strcat(exp, " ");
        strcat(newExpression, exp);
    }
    strcpy(e.expression, newExpression);
    printf("%c =%s\n", e.alphabetNumber, newExpression);
    return e;
}

Expression * calculatePostfix(Expression e, Expression * eAll, int start, Stack s)
{

    // Same extraction of values again.

    int result;
    int i, j = -1;
    int counter = 0;
    //int block = 0;
    char expressionValues[64][64];
    char * token;
    token = strtok(e.expression, " \r\n\t");
    strcpy(expressionValues[counter++], token);
    while( token != NULL )
    {
        token = strtok(NULL, " \r\n\t");
        if(token != NULL && strstr(token, ";") == NULL)
        {
            strcpy(expressionValues[counter++], token);
        }
    }
    printf("\n");

    // The evaluation begins.
    for(i = 0; i < counter; i++)
    {
        // Check if it is an integer.
        if(isInteger(expressionValues[i]))
        {
            push(&s, expressionValues[i]);
            // Check if it is an operator.
        }
        else if(strcmp(expressionValues[i], "/") == 0 ||
                strcmp(expressionValues[i], "*") == 0 ||
                strcmp(expressionValues[i], "+") == 0 ||
                strcmp(expressionValues[i], "-") == 0)
        {
            int val1 = atoi(pop(&s));
            int val2 = atoi(pop(&s));
            val1 = computeBinaryOperation(expressionValues[i][0], val1, val2);
            char var_as_string[50];
            itoa(val1, var_as_string, 10);
            push(&s, var_as_string);
        }
        // Check if it is a variable, and return its value.
        else
        {
            j = -1;
            char variable = ' ';
            int valofvar;
            char var_as_string[50];
            while(j < expCount && variable != expressionValues[i][0])
            {
                variable = eAll[++j].alphabetNumber;
            }
            valofvar = eAll[j].value;
            itoa(valofvar, var_as_string, 10);
            push(&s, var_as_string);
        }
    }


    // The remaining stack is emptied. This is required on the end of expressions.
    while(!isEmpty(s))
    {
        j = 0;
        result = atoi(pop(&s));
        while(j < expCount)
        {
            if(eAll[j].alphabetNumber == e.alphabetNumber)
            {
                eAll[j].value = result;
            }
            j++;
        }
    }
    e.value = result;

    printf("This expression is evaluated as %d.\n", e.value);
    return eAll; // This returns eAll instead of e, because whenever an unknown value is changed, it applies for all.
}

int computeBinaryOperation(char _operator, int a, int b)
{
    int result;
    switch(_operator)
    {
    case '+':
        result = b + a;
        return result;
        break;
    case '-':
        result = b - a;
        return result;
        break;
    case '*':
        result = b * a;
        return result;
        break;
    case '/':
        result = b / a;
        return result;
        break;
    }
    return 0;
}

// Do the two-stage calculation.
Expression * calculateExpression(Expression e, Expression * eAll, int start, Stack s)
{
    char oldExpression[512];
    strcpy(oldExpression, e.expression);
    // Stage 1
    e = infixToPostfix(e, s);

    // Stage 2
    eAll = calculatePostfix(e, eAll, start, s);
    strcpy(e.expression, oldExpression);
    return eAll;
}

// Menu.
int main()
{
    int i = 0;
    Stack s = initializeStack();
    Expression * e;
    char input;
    char path[512];
    while(1)
    {
        printf("Welcome to the expression evaluator.\n");
        printf("1. Load a file.\n");
        printf("2. Show expressions.\n");
        printf("3. Calculate expressions.\n");
        printf("4. Exit.\n");
        printf("Choose something: ");
        scanf("%d", &input);
        switch(input)
        {
        case 1:
            printf("Please input the filename: ");
            scanf("%s", path);
            system("cls");
            e = loadFromFile(path);
            break;
        case 2:
            system("cls");
            for(i = 0; i < expCount; i++)
            {
                showExpression(e[i]);
            }
            break;
        case 3:
            system("cls");
            printf("Postfix expressions:\n--------------\n");
            for(i = 0; i < expCount; i++)
            {
                e = calculateExpression(e[i], e, i, s);
            }
            break;
        case 4:
            printf("Bye!");
            exit(0);
            break;
        default:
            system("cls");
            printf("Wrong input.\n");
            break;
        }
    }
}
