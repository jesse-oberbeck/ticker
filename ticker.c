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
    //Delete every node in a tree and its data, then the tree itself.
    if (root == NULL)
        return;
    //Travel to every node, destroying its parts (post order).
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
    //An in order print of values in tree.
    if (root == NULL)
    {
        return;
    }

    printTree(root->left);

    printf("%s ", root->symbol);
    printf("%.2f ", (double)root->value/100);
    if(root->name)
    {
        printf("%s\n", root->name);
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
    //Creates and returns a file handle based on arg.
    //Assumes file is in same directory as executable.

    char file[255];
    char *path = getenv("HOME");

    sprintf(file, "%s%s", path, fileName);
    return (fopen(fileName, "r"));
}

stock *Find(stock * root, stock *s)
{
    //Finds stock with matching symbol if it exists.
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
    else //When a match is found, adjust the value rather than adding node.
    {
        root->value += s->value;
        return root;
    }
}

int compareSymbol(stock *root, stock *s)
{
    //Comparator function for symbol order.
    return(strcmp(s->symbol, root->symbol));
}

int compareValue(stock* root, stock *s)
{
    //Comparator function for value order.
    return(s->value - root->value);
}

stock *insertStock(stock *root, stock *s, int (*cmp)(stock *root, stock *s))
{
    //Builds binary tree in an order dependent on the passed function (cmp).
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
            strncpy(root->symbol, s->symbol, strlen(s->symbol) + 1);
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
        //Move to appropriate free node.
        if(cmp(root, s) < 0)
        {
            root->left = insertStock(root->left, s, cmp);
        }
        else
        {
            root->right = insertStock(root->right, s, cmp);
        }
    }
    return(root);
}


stock * buildNode(char *file)
{
    //Creates each new node, and fills in values.
    stock *company = calloc(sizeof(stock), 1);
    
    if(!file)
    {
        return(NULL);
    }
    //Skip empty lines.
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
            return(NULL);
        }
        int value = strtod(token, NULL) * 100;
        company->symbol = symbol;
        company->value = value;

        //Handle optional name, and leave out comments.
        token = strtok(NULL, "#");
        if((token))
        {
            //Remove newlines from name if present. Didn't turn newline
            //removal into a function because this one doesnt go toupper.
            for(size_t i = 0; i <= strlen(token); ++i)
            {
                if(token[i] == '\n')
                {
                    token[i] = '\0';
                }
            }
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

stock *valueTree(stock* root, stock *newroot)
{
    //Creates a clone of the first tree, this time
    //sorted by value, ascending.
    if(root == NULL)
    {
        return(NULL);
    }
    
    //printf("!~!~%s\n", root->symbol);
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
    //Opens file and checks if successful.
    FILE *file;
    file = openFile(argv[1]);
    if(!file)
    {
        puts("Not a valid file.");
        exit(1);
    }
    char buffer[128] = {'\0'};
    //Collect and handle stocks from file.
    while(!feof(file))
    {
        fgets(buffer, 128, file);
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
    //Collect and handle user input until CTRL + D.
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
    //Newline to visually separate user input from final printout.
    puts(" ");
    printTree(newroot);
    //Cleanup.
    destroy(newroot);
    destroy(root);
    fclose(file);
}
