snmp_reset_var_buffers(netsnmp_variable_list * var)
{
    while (var) {
        if (var->name != var->name_loc) {
            if(NULL != var->name)
                free(var->name);
            var->name = var->name_loc;
            var->name_length = 0;
        }
        if (var->val.string != var->buf) {
            if (NULL != var->val.string)
                free(var->val.string);
            var->val.string = var->buf;
            var->val_len = 0;
        }
        var = var->next_variable;
    }
}