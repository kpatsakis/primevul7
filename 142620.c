count_varbinds_of_type(netsnmp_variable_list * var_ptr, u_char type)
{
    int             count = 0;

    for (; var_ptr != NULL; var_ptr = var_ptr->next_variable)
        if (var_ptr->type == type)
            count++;

    return count;
}