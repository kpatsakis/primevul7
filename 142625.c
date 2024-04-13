_copy_varlist(netsnmp_variable_list * var,      /* source varList */
              int errindex,     /* index of variable to drop (if any) */
              int copy_count)
{                               /* !=0 number variables to copy */
    netsnmp_variable_list *newhead, *newvar, *oldvar;
    int             ii = 0;

    newhead = NULL;
    oldvar = NULL;

    while (var && (copy_count-- > 0)) {
        /*
         * Drop the specified variable (if applicable) 
         * xxx hmm, is it intentional that dropping the errindex
         *     counts towards copy_count?
         */
        if (++ii == errindex) {
            var = var->next_variable;
            continue;
        }

        /*
         * clone the next variable. Cleanup if alloc fails 
         */
        newvar = (netsnmp_variable_list *)
            malloc(sizeof(netsnmp_variable_list));
        if (snmp_clone_var(var, newvar)) {
            if (newvar)
                free((char *) newvar);
            snmp_free_varbind(newhead);
            return NULL;
        }

        /*
         * add cloned variable to new list  
         */
        if (NULL == newhead)
            newhead = newvar;
        if (oldvar)
            oldvar->next_variable = newvar;
        oldvar = newvar;

        var = var->next_variable;
    }
    return newhead;
}