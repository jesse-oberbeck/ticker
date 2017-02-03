#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Stock
{
    char *symbol;
    double value;
    char *name;
    struct Stock *left;
    struct Stock *right;
};
typedef struct Stock stock;


FILE *
openFile(
    char *fileName)
{

    char file[255];
    char *path = getenv("HOME");

    sprintf(file, "%s%s", path, fileName);
    return (fopen(fileName, "r"));
}

stock *insertStock(stock *market, stock *s)
{
    if(!market)
    {
        return(s);
    }
    else
    {
        if(s->value > market->value);
    }
}

stock * buildMarket(FILE *file)
{
    char buffer[128];
    fgets(buffer, 128, file);
    //printf("buffer: %s\n", buffer);
    char *token = strtok(buffer, " ");
    char *symbol = token;

    double value = strtod(strtok(NULL, " "), NULL);
    printf("SYMBOL: %s \nVALUE: %f\n", symbol, value);

    char *name = NULL;
    name = strtok(NULL, "");

    stock *company = malloc(sizeof(stock));
    company->symbol = symbol;
    company->value = value;
    company->name = name;
    printf("NAME: %s\n", company->name);
}



int main(int argc, char **argv)
{
    stock *market = NULL;
    //Check for sufficient arguments/files.
    if (argc < 2)
    {
        puts("Not enough arguments.");
        return (1);
    }
    stock *root = NULL;
    FILE *file;
    file = openFile(argv[1]);
    market = buildMarket(file);
}
