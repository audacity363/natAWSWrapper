#include <stdio.h>

#include "utils.h"

int main()
{
    char teststr1[] = "Hello World";
    char teststr2[] = "Hello World    ";

    RemoveSpaces(teststr1);
    printf("RemoveSpaces:\n");
    printf("[%s]\n", teststr1);

    printf("StripTrailingSpaces:\n");
    printf("[%s]\n", StripTrailingSpaces(teststr2));
}
