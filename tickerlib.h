struct Stock
{
    char *symbol;
    int value; //Money not stored as floating point.
    char *name; //Long optional name.
    struct Stock *left;
    struct Stock *right;
};
typedef struct Stock stock;

void
destroy(
    stock * root);
    
void
printTree(
    stock * root);

FILE *
openFile(
    char *fileName);

stock *Find(stock * root, stock *s);

int compareSymbol(stock *root, stock *s);

int compareValue(stock* root, stock *s);

stock *insertStock(stock *root, stock *s, int (*cmp)(stock *root, stock *s));

stock * buildNode(char *file);

stock *valueTree(stock* root, stock *newroot);
