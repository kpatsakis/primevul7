xsltNumberFormatAlpha(xsltNumberDataPtr data,
		      xmlBufferPtr buffer,
		      double number,
		      int is_upper)
{
    char temp_string[sizeof(double) * CHAR_BIT * sizeof(xmlChar) + 1];
    char *pointer;
    int i;
    char *alpha_list;
    double alpha_size = (double)(sizeof(alpha_upper_list) - 1);

    /*
     * XSLT 1.0 isn't clear on how to handle zero, but XSLT 2.0 says:
     *
     *     For all format tokens other than the first kind above (one that
     *     consists of decimal digits), there may be implementation-defined
     *     lower and upper bounds on the range of numbers that can be
     *     formatted using this format token; indeed, for some numbering
     *     sequences there may be intrinsic limits. [...] Numbers that fall
     *     outside this range must be formatted using the format token 1.
     *
     * The "a" token has an intrinsic lower limit of 1.
     */
    if (number < 1.0) {
        xsltNumberFormatDecimal(buffer, number, '0', 1,
                                data->digitsPerGroup,
                                data->groupingCharacter,
                                data->groupingCharacterLen);
        return;
    }

    /* Build buffer from back */
    pointer = &temp_string[sizeof(temp_string)];
    *(--pointer) = 0;
    alpha_list = (is_upper) ? alpha_upper_list : alpha_lower_list;

    for (i = 1; i < (int)sizeof(temp_string); i++) {
	number--;
	*(--pointer) = alpha_list[((int)fmod(number, alpha_size))];
	number /= alpha_size;
	if (number < 1.0)
	    break; /* for */
    }
    xmlBufferCCat(buffer, pointer);
}