find_varbind_of_type(netsnmp_variable_list * var_ptr, u_char type)
{
    for (; var_ptr != NULL && var_ptr->type != type;
         var_ptr = var_ptr->next_variable);

    return var_ptr;
}