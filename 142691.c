_get_vb_num(netsnmp_variable_list *vars, int index)
{
    for (; vars && index > 0; --index)
        vars = vars->next_variable;

    if (!vars || index > 0)
        return NULL;
    
    return vars;
}