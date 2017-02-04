#include <stdio.h>
#include <stdlib.h>
#include "tickerlib.h"

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
    return(0);
}
