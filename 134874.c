STATIC AV *
S_add_multi_match(pTHX_ AV* multi_char_matches, SV* multi_string, const STRLEN cp_count)
{
    /* This adds the string scalar <multi_string> to the array
     * <multi_char_matches>.  <multi_string> is known to have exactly
     * <cp_count> code points in it.  This is used when constructing a
     * bracketed character class and we find something that needs to match more
     * than a single character.
     *
     * <multi_char_matches> is actually an array of arrays.  Each top-level
     * element is an array that contains all the strings known so far that are
     * the same length.  And that length (in number of code points) is the same
     * as the index of the top-level array.  Hence, the [2] element is an
     * array, each element thereof is a string containing TWO code points;
     * while element [3] is for strings of THREE characters, and so on.  Since
     * this is for multi-char strings there can never be a [0] nor [1] element.
     *
     * When we rewrite the character class below, we will do so such that the
     * longest strings are written first, so that it prefers the longest
     * matching strings first.  This is done even if it turns out that any
     * quantifier is non-greedy, out of this programmer's (khw) laziness.  Tom
     * Christiansen has agreed that this is ok.  This makes the test for the
     * ligature 'ffi' come before the test for 'ff', for example */

    AV* this_array;
    AV** this_array_ptr;

    PERL_ARGS_ASSERT_ADD_MULTI_MATCH;

    if (! multi_char_matches) {
        multi_char_matches = newAV();
    }

    if (av_exists(multi_char_matches, cp_count)) {
        this_array_ptr = (AV**) av_fetch(multi_char_matches, cp_count, FALSE);
        this_array = *this_array_ptr;
    }
    else {
        this_array = newAV();
        av_store(multi_char_matches, cp_count,
                 (SV*) this_array);
    }
    av_push(this_array, multi_string);

    return multi_char_matches;