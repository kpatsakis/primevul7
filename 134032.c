static inline char *prepreproc(char *line)
{
    if (unlikely(tasm_compatible_mode))
        return check_tasm_directive(line);
    else
        return line;
}