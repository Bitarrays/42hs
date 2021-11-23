#ifndef BUILTINS_H
#define BUILTINS_H

enum flags_echo
{
    NO_OPTION = 0,
    E_OPTION,
    N_OPTION,
    N_E_OPTIONS,
};

//Pour l'instant echo ne retourne aucune erreurs car tout les cas
//semble se gérer seul pour le moment. A voir ce brice m'envoie.
/**
 * @brief echo builtin with -e and -n options.
 * 
 * @param toPrint : message that will be printed.
 * @param flags : option echo was called with.
 */
void echo(char *toPrint, enum flags_echo flags);

#endif
