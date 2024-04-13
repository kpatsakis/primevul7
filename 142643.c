snmp_clone_var(netsnmp_variable_list * var, netsnmp_variable_list * newvar)
{
    if (!newvar || !var)
        return 1;

    memmove(newvar, var, sizeof(netsnmp_variable_list));
    newvar->next_variable = NULL;
    newvar->name = NULL;
    newvar->val.string = NULL;
    newvar->data = NULL;
    newvar->dataFreeHook = NULL;
    newvar->index = 0;

    /*
     * Clone the object identifier and the value.
     * Allocate memory iff original will not fit into local storage.
     */
    if (snmp_set_var_objid(newvar, var->name, var->name_length))
        return 1;

    /*
     * need a pointer to copy a string value. 
     */
    if (var->val.string) {
        if (var->val.string != &var->buf[0]) {
            if (var->val_len <= sizeof(var->buf))
                newvar->val.string = newvar->buf;
            else {
                newvar->val.string = (u_char *) malloc(var->val_len);
                if (!newvar->val.string)
                    return 1;
            }
            memmove(newvar->val.string, var->val.string, var->val_len);
        } else {                /* fix the pointer to new local store */
            newvar->val.string = newvar->buf;
            /*
             * no need for a memmove, since we copied the whole var
             * struct (and thus var->buf) at the beginning of this function.
             */
        }
    } else {
        newvar->val.string = NULL;
        newvar->val_len = 0;
    }

    return 0;
}