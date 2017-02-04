#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

struct Stock
{
    char *symbol;
    int value;
    char *name; //Long optional name.
    struct Stock *left;
    struct Stock *right;
};
typedef struct Stock stock;

void
destroy(
    stock * root)
{
    //Function taken and modified from BST exercise done in class.
    if (root == NULL)           // or if (!root)
        return;

    destroy(root->left);
    destroy(root->right);
    free(root->symbol);
    root->symbol = NULL;
    free(root->name);
    root->name = NULL;
    free(root);
}

void
printTree(
    stock * root)
{
    if (root == NULL)
    {
        return;
    }

    printTree(root->left);

    printf("%s\n", root->symbol);
    printf("%.2f\n", (double)root->value/100);
    if(root->name)
    {
        printf("%s\n\n", root->name);
    }
    else
    {
        puts(" ");
    }

    printTree(root->right);
}

FILE *
openFile(
    char *fileName)
{

    char file[255];
    char *path = getenv("HOME");

    sprintf(file, "%s%s", path, fileName);
    return (fopen(fileName, "r"));
}

stock *Find(stock * root, stock *s)
{
    //Function taken and modified from BST exercise done in class.
    if (root == NULL)
    {
        return NULL;
    }
    if (strcasecmp(s->symbol, root->symbol) < 0)
    {
        return Find(root->left, s);
    }
    else if (strcasecmp(s->symbol, root->symbol) > 0)
    {
        return Find(root->right, s);
    }
    else                        //match found.
    {
        root->value += s->value;
        return root;
    }
}

int compareSymbol(stock *root, stock *s)
{
    return(strcmp(s->symbol, root->symbol));
}

int compareValue(stock* root, stock *s)
{
    printf("%i - %i\n", s->value, root->value);
    return(s->value - root->value);
}

stock *insertStock(stock *root, stock *s, int (*cmp)(stock *root, stock *s))
{
    if(s->symbol == NULL)
    {
        puts("NO SYMBOL");
        return(root);
    }
    if(root == NULL)
    {
        root = calloc(sizeof(*root), 1);

        if(!root)
        {
            puts("ERROR.");
            return(NULL);
        }
        else
        {
            //Make the node, fill with data.
            root->value = s->value;
            root->symbol = calloc(strlen(s->symbol) + 1, 1);
            strncpy(root->symbol, s->symbol, strlen(s->symbol) + 1);//+1s might not be needed.
            printf("INSERTING: %s\n", root->symbol);
            if(s->name != NULL)
            {
                root->name = calloc(strlen(s->name) + 1, 1);
                strncpy(root->name, s->name, strlen(s->name) + 1);
            }
            root->left = NULL;
            root->right = NULL;
        }
    }
    else
    {
        printf("ROOT SYM: %s\n", root->symbol);
        //Move to appropriate free node.
        if(cmp(root, s) < 0)
        {
            puts("left");
            root->left = insertStock(root->left, s, cmp);
        }
        else
        {
            puts("right");
            root->right = insertStock(root->right, s, cmp);
        }
    }
    return(root);
}


stock * buildNode(char *file)
{
    stock *company = calloc(sizeof(stock), 1);
    
    if(!file)
    {
        return(NULL);
    }

    if((file[0] != '\0') && (file[0] != '\n'))
    {
        char *token = strtok(file, " ");
        char *symbol = calloc(strlen(token) + 1, 1);
        strncpy(symbol,token, strlen(token));
        for(size_t i = 0; i <= strlen(token); ++i)
        {
            //Remove newlines, and convert to uppercase.
            if(symbol[i] == '\n')
            {
                symbol[i] = '\0';
            }
            symbol[i] = toupper(symbol[i]);
        }
        token = strtok(NULL, " ");
        if(!token)
        {
            puts("NOTOK");
            return(NULL);
        }
        int value = strtod(token, NULL) * 100;
        company->symbol = symbol;
        company->value = value;

        //Handle optional name.
        token = strtok(NULL, "#");
        //char *name = NULL;
        if((token))
        {
            company->name = calloc(strlen(token) + 1, 1);
            strncpy(company->name, token, strlen(token));
        }
        else
        {
            company->name = NULL;
        }
        company->left = NULL;
        company->right = NULL;
    }
    return(company);
}

/*///////////////////////////////
stock *valueTree(stock *root, stock *newroot)
{
    puts("VALTREE");
    if (root == NULL)
    {
        return(newroot);
    }
    valueTree(root->left, newroot);
    valueTree(root->right, newroot);

    char buffer[128] = {'\0'};
    sprintf(buffer, "%s %d %s", root->symbol, root->value, root->name);
    stock *newStock = buildNode(buffer);
    printf("NEWSTOCK S: %s\n", newStock->symbol);
    newroot = insertStock(newroot, newStock, compareValue);
}
*////////////////////////////////
stock *valueTree(stock* root, stock *newroot)
{
    if(root == NULL)
    {
        return(NULL);
    }
    
    printf("!~!~%s\n", root->symbol);
    newroot = insertStock(newroot, root, compareValue);
    valueTree(root->left, newroot);
    valueTree(root->right, newroot);
    return(newroot);
}


int main(int argc, char **argv)
{
    stock *company = NULL;
    stock *root = NULL;
    //Check for sufficient arguments/files.
    if (argc < 2)
    {
        puts("Not enough arguments.");
        return (1);
    }
    //stock *root = NULL;
    FILE *file;
    file = openFile(argv[1]);
    if(!file)
    {
        puts("Not a valid file.");
        exit(1);
    }
    char buffer[128] = {'\0'};
    while(!feof(file))
    {
        char * check = fgets(buffer, 128, file);
        /*
        if(check == NULL)
        {
            fgets(buffer, sizeof(buffer), stdin);
        }
        */
        company = buildNode(buffer);
        if(company == NULL)
        {
            puts("INVALID ENTRY DETECTED!");
            exit(1);
        }
        if(company->symbol)
        {
            stock * check = Find(root, company);
            if(check == NULL)
            {
                root = insertStock(root, company, compareSymbol);
            }
        }
        destroy(company);
        fflush(stdin);
        buffer[0] = '\0';
    }
    while(fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        company = buildNode(buffer);
        if(company == NULL)
        {
            puts("INVALID ENTRY DETECTED!");
            exit(1);
        }

        if(company->symbol)
        {
            stock * check = Find(root, company);
            if(check == NULL)
            {
                root = insertStock(root, company, compareSymbol);
            }
        }
        destroy(company);
        fflush(stdin);
    }
    stock *newroot = NULL;
    newroot = valueTree(root, newroot);
    printTree(newroot);
    destroy(newroot);
    destroy(root);
    fclose(file);
}
