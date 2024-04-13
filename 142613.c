count_varbinds(netsnmp_variable_list * var_ptr)
{
    int             count = 0;

    for (; var_ptr != NULL; var_ptr = var_ptr->next_variable)
        count++;

    return count;
}