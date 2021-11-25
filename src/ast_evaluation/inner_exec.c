#include <string.h>

#include "ast_evaluation_tools.h"

int is_in(char **condition)
{
    while ((*condition)[0] != '\0')
    {
        if (!strcmp(*condition, "in"))
            return 1;
        condition++;
    }
    return 0;
}
