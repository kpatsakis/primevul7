SV *
Perl_parse_uniprop_string(pTHX_

    /* Parse the interior of a \p{}, \P{}.  Returns its definition if knowable
     * now.  If so, the return is an inversion list.
     *
     * If the property is user-defined, it is a subroutine, which in turn
     * may call other subroutines.  This function will call the whole nest of
     * them to get the definition they return; if some aren't known at the time
     * of the call to this function, the fully qualified name of the highest
     * level sub is returned.  It is an error to call this function at runtime
     * without every sub defined.
     *
     * If an error was found, NULL is returned, and 'msg' gets a suitable
     * message appended to it.  (Appending allows the back trace of how we got
     * to the faulty definition to be displayed through nested calls of
     * user-defined subs.)
     *
     * The caller should NOT try to free any returned inversion list.
     *
     * Other parameters will be set on return as described below */

    const char * const name,    /* The first non-blank in the \p{}, \P{} */
    const Size_t name_len,      /* Its length in bytes, not including any
                                   trailing space */
    const bool is_utf8,         /* ? Is 'name' encoded in UTF-8 */
    const bool to_fold,         /* ? Is this under /i */
    const bool runtime,         /* TRUE if this is being called at run time */
    const bool deferrable,      /* TRUE if it's ok for the definition to not be
                                   known at this call */
    bool *user_defined_ptr,     /* Upon return from this function it will be
                                   set to TRUE if any component is a
                                   user-defined property */
    SV * msg,                   /* Any error or warning msg(s) are appended to
                                   this */
   const STRLEN level)          /* Recursion level of this call */
{
    dVAR;
    char* lookup_name;          /* normalized name for lookup in our tables */
    unsigned lookup_len;        /* Its length */
    bool stricter = FALSE;      /* Some properties have stricter name
                                   normalization rules, which we decide upon
                                   based on parsing */

    /* nv= or numeric_value=, or possibly one of the cjk numeric properties
     * (though it requires extra effort to download them from Unicode and
     * compile perl to know about them) */
    bool is_nv_type = FALSE;

    unsigned int i, j = 0;
    int equals_pos = -1;    /* Where the '=' is found, or negative if none */
    int slash_pos  = -1;    /* Where the '/' is found, or negative if none */
    int table_index = 0;    /* The entry number for this property in the table
                               of all Unicode property names */
    bool starts_with_In_or_Is = FALSE;  /* ? Does the name start with 'In' or
                                             'Is' */
    Size_t lookup_offset = 0;   /* Used to ignore the first few characters of
                                   the normalized name in certain situations */
    Size_t non_pkg_begin = 0;   /* Offset of first byte in 'name' that isn't
                                   part of a package name */
    bool could_be_user_defined = TRUE;  /* ? Could this be a user-defined
                                             property rather than a Unicode
                                             one. */
    SV * prop_definition = NULL;  /* The returned definition of 'name' or NULL
                                     if an error.  If it is an inversion list,
                                     it is the definition.  Otherwise it is a
                                     string containing the fully qualified sub
                                     name of 'name' */
    SV * fq_name = NULL;        /* For user-defined properties, the fully
                                   qualified name */
    bool invert_return = FALSE; /* ? Do we need to complement the result before
                                     returning it */

    PERL_ARGS_ASSERT_PARSE_UNIPROP_STRING;

    /* The input will be normalized into 'lookup_name' */
    Newx(lookup_name, name_len, char);
    SAVEFREEPV(lookup_name);

    /* Parse the input. */
    for (i = 0; i < name_len; i++) {
        char cur = name[i];

        /* Most of the characters in the input will be of this ilk, being parts
         * of a name */
        if (isIDCONT_A(cur)) {

            /* Case differences are ignored.  Our lookup routine assumes
             * everything is lowercase, so normalize to that */
            if (isUPPER_A(cur)) {
                lookup_name[j++] = toLOWER_A(cur);
                continue;
            }

            if (cur == '_') { /* Don't include these in the normalized name */
                continue;
            }

            lookup_name[j++] = cur;

            /* The first character in a user-defined name must be of this type.
             * */
            if (i - non_pkg_begin == 0 && ! isIDFIRST_A(cur)) {
                could_be_user_defined = FALSE;
            }

            continue;
        }

        /* Here, the character is not something typically in a name,  But these
         * two types of characters (and the '_' above) can be freely ignored in
         * most situations.  Later it may turn out we shouldn't have ignored
         * them, and we have to reparse, but we don't have enough information
         * yet to make that decision */
        if (cur == '-' || isSPACE_A(cur)) {
            could_be_user_defined = FALSE;
            continue;
        }

        /* An equals sign or single colon mark the end of the first part of
         * the property name */
        if (    cur == '='
            || (cur == ':' && (i >= name_len - 1 || name[i+1] != ':')))
        {
            lookup_name[j++] = '='; /* Treat the colon as an '=' */
            equals_pos = j; /* Note where it occurred in the input */
            could_be_user_defined = FALSE;
            break;
        }

        /* Otherwise, this character is part of the name. */
        lookup_name[j++] = cur;

        /* Here it isn't a single colon, so if it is a colon, it must be a
         * double colon */
        if (cur == ':') {

            /* A double colon should be a package qualifier.  We note its
             * position and continue.  Note that one could have
             *      pkg1::pkg2::...::foo
             * so that the position at the end of the loop will be just after
             * the final qualifier */

            i++;
            non_pkg_begin = i + 1;
            lookup_name[j++] = ':';
        }
        else { /* Only word chars (and '::') can be in a user-defined name */
            could_be_user_defined = FALSE;
        }
    } /* End of parsing through the lhs of the property name (or all of it if
         no rhs) */

#define STRLENs(s)  (sizeof("" s "") - 1)

    /* If there is a single package name 'utf8::', it is ambiguous.  It could
     * be for a user-defined property, or it could be a Unicode property, as
     * all of them are considered to be for that package.  For the purposes of
     * parsing the rest of the property, strip it off */
    if (non_pkg_begin == STRLENs("utf8::") && memBEGINPs(name, name_len, "utf8::")) {
        lookup_name +=  STRLENs("utf8::");
        j -=  STRLENs("utf8::");
        equals_pos -=  STRLENs("utf8::");
    }

    /* Here, we are either done with the whole property name, if it was simple;
     * or are positioned just after the '=' if it is compound. */

    if (equals_pos >= 0) {
        assert(! stricter); /* We shouldn't have set this yet */

        /* Space immediately after the '=' is ignored */
        i++;
        for (; i < name_len; i++) {
            if (! isSPACE_A(name[i])) {
                break;
            }
        }

        /* Most punctuation after the equals indicates a subpattern, like
         * \p{foo=/bar/} */
        if (   isPUNCT_A(name[i])
            && name[i] != '-'
            && name[i] != '+'
            && name[i] != '_'
            && name[i] != '{')
        {
            /* Find the property.  The table includes the equals sign, so we
             * use 'j' as-is */
            table_index = match_uniprop((U8 *) lookup_name, j);
            if (table_index) {
                const char * const * prop_values
                                            = UNI_prop_value_ptrs[table_index];
                SV * subpattern;
                Size_t subpattern_len;
                REGEXP * subpattern_re;
                char open = name[i++];
                char close;
                const char * pos_in_brackets;
                bool escaped = 0;

                /* A backslash means the real delimitter is the next character.
                 * */
                if (open == '\\') {
                    open = name[i++];
                    escaped = 1;
                }

                /* This data structure is constructed so that the matching
                 * closing bracket is 3 past its matching opening.  The second
                 * set of closing is so that if the opening is something like
                 * ']', the closing will be that as well.  Something similar is
                 * done in toke.c */
                pos_in_brackets = strchr("([<)]>)]>", open);
                close = (pos_in_brackets) ? pos_in_brackets[3] : open;

                if (    i >= name_len
                    ||  name[name_len-1] != close
                    || (escaped && name[name_len-2] != '\\'))
                {
                    sv_catpvs(msg, "Unicode property wildcard not terminated");
                    goto append_name_to_msg;
                }

                Perl_ck_warner_d(aTHX_
                    packWARN(WARN_EXPERIMENTAL__UNIPROP_WILDCARDS),
                    "The Unicode property wildcards feature is experimental");

                /* Now create and compile the wildcard subpattern.  Use /iaa
                 * because nothing outside of ASCII will match, and it the
                 * property values should all match /i.  Note that when the
                 * pattern fails to compile, our added text to the user's
                 * pattern will be displayed to the user, which is not so
                 * desirable. */
                subpattern_len = name_len - i - 1 - escaped;
                subpattern = Perl_newSVpvf(aTHX_ "(?iaa:%.*s)",
                                              (unsigned) subpattern_len,
                                              name + i);
                subpattern = sv_2mortal(subpattern);
                subpattern_re = re_compile(subpattern, 0);
                assert(subpattern_re);  /* Should have died if didn't compile
                                         successfully */

                /* For each legal property value, see if the supplied pattern
                 * matches it. */
                while (*prop_values) {
                    const char * const entry = *prop_values;
                    const Size_t len = strlen(entry);
                    SV* entry_sv = newSVpvn_flags(entry, len, SVs_TEMP);

                    if (pregexec(subpattern_re,
                                 (char *) entry,
                                 (char *) entry + len,
                                 (char *) entry, 0,
                                 entry_sv,
                                 0))
                    { /* Here, matched.  Add to the returned list */
                        Size_t total_len = j + len;
                        SV * sub_invlist = NULL;
                        char * this_string;

                        /* We know this is a legal \p{property=value}.  Call
                         * the function to return the list of code points that
                         * match it */
                        Newxz(this_string, total_len + 1, char);
                        Copy(lookup_name, this_string, j, char);
                        my_strlcat(this_string, entry, total_len + 1);
                        SAVEFREEPV(this_string);
                        sub_invlist = parse_uniprop_string(this_string,
                                                           total_len,
                                                           is_utf8,
                                                           to_fold,
                                                           runtime,
                                                           deferrable,
                                                           user_defined_ptr,
                                                           msg,
                                                           level + 1);
                        _invlist_union(prop_definition, sub_invlist,
                                       &prop_definition);
                    }

                    prop_values++;  /* Next iteration, look at next propvalue */
                } /* End of looking through property values; (the data
                     structure is terminated by a NULL ptr) */

                SvREFCNT_dec_NN(subpattern_re);

                if (prop_definition) {
                    return prop_definition;
                }

                sv_catpvs(msg, "No Unicode property value wildcard matches:");
                goto append_name_to_msg;
            }

            /* Here's how khw thinks we should proceed to handle the properties
             * not yet done:    Bidi Mirroring Glyph
                                Bidi Paired Bracket
                                Case Folding  (both full and simple)
                                Decomposition Mapping
                                Equivalent Unified Ideograph
                                Name
                                Name Alias
                                Lowercase Mapping  (both full and simple)
                                NFKC Case Fold
                                Titlecase Mapping  (both full and simple)
                                Uppercase Mapping  (both full and simple)
             * Move the part that looks at the property values into a perl
             * script, like utf8_heavy.pl is done.  This makes things somewhat
             * easier, but most importantly, it avoids always adding all these
             * strings to the memory usage when the feature is little-used.
             *
             * The property values would all be concatenated into a single
             * string per property with each value on a separate line, and the
             * code point it's for on alternating lines.  Then we match the
             * user's input pattern m//mg, without having to worry about their
             * uses of '^' and '$'.  Only the values that aren't the default
             * would be in the strings.  Code points would be in UTF-8.  The
             * search pattern that we would construct would look like
             * (?: \n (code-point_re) \n (?aam: user-re ) \n )
             * And so $1 would contain the code point that matched the user-re.
             * For properties where the default is the code point itself, such
             * as any of the case changing mappings, the string would otherwise
             * consist of all Unicode code points in UTF-8 strung together.
             * This would be impractical.  So instead, examine their compiled
             * pattern, looking at the ssc.  If none, reject the pattern as an
             * error.  Otherwise run the pattern against every code point in
             * the ssc.  The ssc is kind of like tr18's 3.9 Possible Match Sets
             * And it might be good to create an API to return the ssc.
             *
             * For the name properties, a new function could be created in
             * charnames which essentially does the same thing as above,
             * sharing Name.pl with the other charname functions.  Don't know
             * about loose name matching, or algorithmically determined names.
             * Decomposition.pl similarly.
             *
             * It might be that a new pattern modifier would have to be
             * created, like /t for resTricTed, which changed the behavior of
             * some constructs in their subpattern, like \A. */
        } /* End of is a wildcard subppattern */


        /* Certain properties whose values are numeric need special handling.
         * They may optionally be prefixed by 'is'.  Ignore that prefix for the
         * purposes of checking if this is one of those properties */
        if (memBEGINPs(lookup_name, j, "is")) {
            lookup_offset = 2;
        }

        /* Then check if it is one of these specially-handled properties.  The
         * possibilities are hard-coded because easier this way, and the list
         * is unlikely to change.
         *
         * All numeric value type properties are of this ilk, and are also
         * special in a different way later on.  So find those first.  There
         * are several numeric value type properties in the Unihan DB (which is
         * unlikely to be compiled with perl, but we handle it here in case it
         * does get compiled).  They all end with 'numeric'.  The interiors
         * aren't checked for the precise property.  This would stop working if
         * a cjk property were to be created that ended with 'numeric' and
         * wasn't a numeric type */
        is_nv_type = memEQs(lookup_name + lookup_offset,
                       j - 1 - lookup_offset, "numericvalue")
                  || memEQs(lookup_name + lookup_offset,
                      j - 1 - lookup_offset, "nv")
                  || (   memENDPs(lookup_name + lookup_offset,
                            j - 1 - lookup_offset, "numeric")
                      && (   memBEGINPs(lookup_name + lookup_offset,
                                      j - 1 - lookup_offset, "cjk")
                          || memBEGINPs(lookup_name + lookup_offset,
                                      j - 1 - lookup_offset, "k")));
        if (   is_nv_type
            || memEQs(lookup_name + lookup_offset,
                      j - 1 - lookup_offset, "canonicalcombiningclass")
            || memEQs(lookup_name + lookup_offset,
                      j - 1 - lookup_offset, "ccc")
            || memEQs(lookup_name + lookup_offset,
                      j - 1 - lookup_offset, "age")
            || memEQs(lookup_name + lookup_offset,
                      j - 1 - lookup_offset, "in")
            || memEQs(lookup_name + lookup_offset,
                      j - 1 - lookup_offset, "presentin"))
        {
            unsigned int k;

            /* Since the stuff after the '=' is a number, we can't throw away
             * '-' willy-nilly, as those could be a minus sign.  Other stricter
             * rules also apply.  However, these properties all can have the
             * rhs not be a number, in which case they contain at least one
             * alphabetic.  In those cases, the stricter rules don't apply.
             * But the numeric type properties can have the alphas [Ee] to
             * signify an exponent, and it is still a number with stricter
             * rules.  So look for an alpha that signifies not-strict */
            stricter = TRUE;
            for (k = i; k < name_len; k++) {
                if (   isALPHA_A(name[k])
                    && (! is_nv_type || ! isALPHA_FOLD_EQ(name[k], 'E')))
                {
                    stricter = FALSE;
                    break;
                }
            }
        }

        if (stricter) {

            /* A number may have a leading '+' or '-'.  The latter is retained
             * */
            if (name[i] == '+') {
                i++;
            }
            else if (name[i] == '-') {
                lookup_name[j++] = '-';
                i++;
            }

            /* Skip leading zeros including single underscores separating the
             * zeros, or between the final leading zero and the first other
             * digit */
            for (; i < name_len - 1; i++) {
                if (    name[i] != '0'
                    && (name[i] != '_' || ! isDIGIT_A(name[i+1])))
                {
                    break;
                }
            }
        }
    }
    else {  /* No '=' */

       /* Only a few properties without an '=' should be parsed with stricter
        * rules.  The list is unlikely to change. */
        if (   memBEGINPs(lookup_name, j, "perl")
            && memNEs(lookup_name + 4, j - 4, "space")
            && memNEs(lookup_name + 4, j - 4, "word"))
        {
            stricter = TRUE;

            /* We set the inputs back to 0 and the code below will reparse,
             * using strict */
            i = j = 0;
        }
    }

    /* Here, we have either finished the property, or are positioned to parse
     * the remainder, and we know if stricter rules apply.  Finish out, if not
     * already done */
    for (; i < name_len; i++) {
        char cur = name[i];

        /* In all instances, case differences are ignored, and we normalize to
         * lowercase */
        if (isUPPER_A(cur)) {
            lookup_name[j++] = toLOWER(cur);
            continue;
        }

        /* An underscore is skipped, but not under strict rules unless it
         * separates two digits */
        if (cur == '_') {
            if (    stricter
                && (     i == 0 || (int) i == equals_pos || i == name_len- 1
                    || ! isDIGIT_A(name[i-1]) || ! isDIGIT_A(name[i+1])))
            {
                lookup_name[j++] = '_';
            }
            continue;
        }

        /* Hyphens are skipped except under strict */
        if (cur == '-' && ! stricter) {
            continue;
        }

        /* XXX Bug in documentation.  It says white space skipped adjacent to
         * non-word char.  Maybe we should, but shouldn't skip it next to a dot
         * in a number */
        if (isSPACE_A(cur) && ! stricter) {
            continue;
        }

        lookup_name[j++] = cur;

        /* Unless this is a non-trailing slash, we are done with it */
        if (i >= name_len - 1 || cur != '/') {
            continue;
        }

        slash_pos = j;

        /* A slash in the 'numeric value' property indicates that what follows
         * is a denominator.  It can have a leading '+' and '0's that should be
         * skipped.  But we have never allowed a negative denominator, so treat
         * a minus like every other character.  (No need to rule out a second
         * '/', as that won't match anything anyway */
        if (is_nv_type) {
            i++;
            if (i < name_len && name[i] == '+') {
                i++;
            }

            /* Skip leading zeros including underscores separating digits */
            for (; i < name_len - 1; i++) {
                if (   name[i] != '0'
                    && (name[i] != '_' || ! isDIGIT_A(name[i+1])))
                {
                    break;
                }
            }

            /* Store the first real character in the denominator */
            lookup_name[j++] = name[i];
        }
    }

    /* Here are completely done parsing the input 'name', and 'lookup_name'
     * contains a copy, normalized.
     *
     * This special case is grandfathered in: 'L_' and 'GC=L_' are accepted and
     * different from without the underscores.  */
    if (  (   UNLIKELY(memEQs(lookup_name, j, "l"))
           || UNLIKELY(memEQs(lookup_name, j, "gc=l")))
        && UNLIKELY(name[name_len-1] == '_'))
    {
        lookup_name[j++] = '&';
    }

    /* If the original input began with 'In' or 'Is', it could be a subroutine
     * call to a user-defined property instead of a Unicode property name. */
    if (    non_pkg_begin + name_len > 2
        &&  name[non_pkg_begin+0] == 'I'
        && (name[non_pkg_begin+1] == 'n' || name[non_pkg_begin+1] == 's'))
    {
        starts_with_In_or_Is = TRUE;
    }
    else {
        could_be_user_defined = FALSE;
    }

    if (could_be_user_defined) {
        CV* user_sub;

        /* If the user defined property returns the empty string, it could
         * easily be because the pattern is being compiled before the data it
         * actually needs to compile is available.  This could be argued to be
         * a bug in the perl code, but this is a change of behavior for Perl,
         * so we handle it.  This means that intentionally returning nothing
         * will not be resolved until runtime */
        bool empty_return = FALSE;

        /* Here, the name could be for a user defined property, which are
         * implemented as subs. */
        user_sub = get_cvn_flags(name, name_len, 0);
        if (user_sub) {
            const char insecure[] = "Insecure user-defined property";

            /* Here, there is a sub by the correct name.  Normally we call it
             * to get the property definition */
            dSP;
            SV * user_sub_sv = MUTABLE_SV(user_sub);
            SV * error;     /* Any error returned by calling 'user_sub' */
            SV * key;       /* The key into the hash of user defined sub names
                             */
            SV * placeholder;
            SV ** saved_user_prop_ptr;      /* Hash entry for this property */

            /* How many times to retry when another thread is in the middle of
             * expanding the same definition we want */
            PERL_INT_FAST8_T retry_countdown = 10;

            DECLARATION_FOR_GLOBAL_CONTEXT;

            /* If we get here, we know this property is user-defined */
            *user_defined_ptr = TRUE;

            /* We refuse to call a potentially tainted subroutine; returning an
             * error instead */
            if (TAINT_get) {
                if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                sv_catpvn(msg, insecure, sizeof(insecure) - 1);
                goto append_name_to_msg;
            }

            /* In principal, we only call each subroutine property definition
             * once during the life of the program.  This guarantees that the
             * property definition never changes.  The results of the single
             * sub call are stored in a hash, which is used instead for future
             * references to this property.  The property definition is thus
             * immutable.  But, to allow the user to have a /i-dependent
             * definition, we call the sub once for non-/i, and once for /i,
             * should the need arise, passing the /i status as a parameter.
             *
             * We start by constructing the hash key name, consisting of the
             * fully qualified subroutine name, preceded by the /i status, so
             * that there is a key for /i and a different key for non-/i */
            key = newSVpvn(((to_fold) ? "1" : "0"), 1);
            fq_name = S_get_fq_name(aTHX_ name, name_len, is_utf8,
                                          non_pkg_begin != 0);
            sv_catsv(key, fq_name);
            sv_2mortal(key);

            /* We only call the sub once throughout the life of the program
             * (with the /i, non-/i exception noted above).  That means the
             * hash must be global and accessible to all threads.  It is
             * created at program start-up, before any threads are created, so
             * is accessible to all children.  But this creates some
             * complications.
             *
             * 1) The keys can't be shared, or else problems arise; sharing is
             *    turned off at hash creation time
             * 2) All SVs in it are there for the remainder of the life of the
             *    program, and must be created in the same interpreter context
             *    as the hash, or else they will be freed from the wrong pool
             *    at global destruction time.  This is handled by switching to
             *    the hash's context to create each SV going into it, and then
             *    immediately switching back
             * 3) All accesses to the hash must be controlled by a mutex, to
             *    prevent two threads from getting an unstable state should
             *    they simultaneously be accessing it.  The code below is
             *    crafted so that the mutex is locked whenever there is an
             *    access and unlocked only when the next stable state is
             *    achieved.
             *
             * The hash stores either the definition of the property if it was
             * valid, or, if invalid, the error message that was raised.  We
             * use the type of SV to distinguish.
             *
             * There's also the need to guard against the definition expansion
             * from infinitely recursing.  This is handled by storing the aTHX
             * of the expanding thread during the expansion.  Again the SV type
             * is used to distinguish this from the other two cases.  If we
             * come to here and the hash entry for this property is our aTHX,
             * it means we have recursed, and the code assumes that we would
             * infinitely recurse, so instead stops and raises an error.
             * (Any recursion has always been treated as infinite recursion in
             * this feature.)
             *
             * If instead, the entry is for a different aTHX, it means that
             * that thread has gotten here first, and hasn't finished expanding
             * the definition yet.  We just have to wait until it is done.  We
             * sleep and retry a few times, returning an error if the other
             * thread doesn't complete. */

          re_fetch:
            USER_PROP_MUTEX_LOCK;

            /* If we have an entry for this key, the subroutine has already
             * been called once with this /i status. */
            saved_user_prop_ptr = hv_fetch(PL_user_def_props,
                                                   SvPVX(key), SvCUR(key), 0);
            if (saved_user_prop_ptr) {

                /* If the saved result is an inversion list, it is the valid
                 * definition of this property */
                if (is_invlist(*saved_user_prop_ptr)) {
                    prop_definition = *saved_user_prop_ptr;

                    /* The SV in the hash won't be removed until global
                     * destruction, so it is stable and we can unlock */
                    USER_PROP_MUTEX_UNLOCK;

                    /* The caller shouldn't try to free this SV */
                    return prop_definition;
                }

                /* Otherwise, if it is a string, it is the error message
                 * that was returned when we first tried to evaluate this
                 * property.  Fail, and append the message */
                if (SvPOK(*saved_user_prop_ptr)) {
                    if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                    sv_catsv(msg, *saved_user_prop_ptr);

                    /* The SV in the hash won't be removed until global
                     * destruction, so it is stable and we can unlock */
                    USER_PROP_MUTEX_UNLOCK;

                    return NULL;
                }

                assert(SvIOK(*saved_user_prop_ptr));

                /* Here, we have an unstable entry in the hash.  Either another
                 * thread is in the middle of expanding the property's
                 * definition, or we are ourselves recursing.  We use the aTHX
                 * in it to distinguish */
                if (SvIV(*saved_user_prop_ptr) != PTR2IV(CUR_CONTEXT)) {

                    /* Here, it's another thread doing the expanding.  We've
                     * looked as much as we are going to at the contents of the
                     * hash entry.  It's safe to unlock. */
                    USER_PROP_MUTEX_UNLOCK;

                    /* Retry a few times */
                    if (retry_countdown-- > 0) {
                        PerlProc_sleep(1);
                        goto re_fetch;
                    }

                    if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                    sv_catpvs(msg, "Timeout waiting for another thread to "
                                   "define");
                    goto append_name_to_msg;
                }

                /* Here, we are recursing; don't dig any deeper */
                USER_PROP_MUTEX_UNLOCK;

                if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                sv_catpvs(msg,
                          "Infinite recursion in user-defined property");
                goto append_name_to_msg;
            }

            /* Here, this thread has exclusive control, and there is no entry
             * for this property in the hash.  So we have the go ahead to
             * expand the definition ourselves. */

            PUSHSTACKi(PERLSI_MAGIC);
            ENTER;

            /* Create a temporary placeholder in the hash to detect recursion
             * */
            SWITCH_TO_GLOBAL_CONTEXT;
            placeholder= newSVuv(PTR2IV(ORIGINAL_CONTEXT));
            (void) hv_store_ent(PL_user_def_props, key, placeholder, 0);
            RESTORE_CONTEXT;

            /* Now that we have a placeholder, we can let other threads
             * continue */
            USER_PROP_MUTEX_UNLOCK;

            /* Make sure the placeholder always gets destroyed */
            SAVEDESTRUCTOR_X(S_delete_recursion_entry, SvPVX(key));

            PUSHMARK(SP);
            SAVETMPS;

            /* Call the user's function, with the /i status as a parameter.
             * Note that we have gone to a lot of trouble to keep this call
             * from being within the locked mutex region. */
            XPUSHs(boolSV(to_fold));
            PUTBACK;

            /* The following block was taken from swash_init().  Presumably
             * they apply to here as well, though we no longer use a swash --
             * khw */
            SAVEHINTS();
            save_re_context();
            /* We might get here via a subroutine signature which uses a utf8
             * parameter name, at which point PL_subname will have been set
             * but not yet used. */
            save_item(PL_subname);

            (void) call_sv(user_sub_sv, G_EVAL|G_SCALAR);

            SPAGAIN;

            error = ERRSV;
            if (TAINT_get || SvTRUE(error)) {
                if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                if (SvTRUE(error)) {
                    sv_catpvs(msg, "Error \"");
                    sv_catsv(msg, error);
                    sv_catpvs(msg, "\"");
                }
                if (TAINT_get) {
                    if (SvTRUE(error)) sv_catpvs(msg, "; ");
                    sv_catpvn(msg, insecure, sizeof(insecure) - 1);
                }

                if (name_len > 0) {
                    sv_catpvs(msg, " in expansion of ");
                    Perl_sv_catpvf(aTHX_ msg, "%" UTF8f, UTF8fARG(is_utf8,
                                                                  name_len,
                                                                  name));
                }

                (void) POPs;
                prop_definition = NULL;
            }
            else {  /* G_SCALAR guarantees a single return value */
                SV * contents = POPs;

                /* The contents is supposed to be the expansion of the property
                 * definition.  If the definition is deferrable, and we got an
                 * empty string back, set a flag to later defer it (after clean
                 * up below). */
                if (      deferrable
                    && (! SvPOK(contents) || SvCUR(contents) == 0))
                {
                        empty_return = TRUE;
                }
                else { /* Otherwise, call a function to check for valid syntax,
                          and handle it */

                    prop_definition = handle_user_defined_property(
                                                    name, name_len,
                                                    is_utf8, to_fold, runtime,
                                                    deferrable,
                                                    contents, user_defined_ptr,
                                                    msg,
                                                    level);
                }
            }

            /* Here, we have the results of the expansion.  Delete the
             * placeholder, and if the definition is now known, replace it with
             * that definition.  We need exclusive access to the hash, and we
             * can't let anyone else in, between when we delete the placeholder
             * and add the permanent entry */
            USER_PROP_MUTEX_LOCK;

            S_delete_recursion_entry(aTHX_ SvPVX(key));

            if (    ! empty_return
                && (! prop_definition || is_invlist(prop_definition)))
            {
                /* If we got success we use the inversion list defining the
                 * property; otherwise use the error message */
                SWITCH_TO_GLOBAL_CONTEXT;
                (void) hv_store_ent(PL_user_def_props,
                                    key,
                                    ((prop_definition)
                                     ? newSVsv(prop_definition)
                                     : newSVsv(msg)),
                                    0);
                RESTORE_CONTEXT;
            }

            /* All done, and the hash now has a permanent entry for this
             * property.  Give up exclusive control */
            USER_PROP_MUTEX_UNLOCK;

            FREETMPS;
            LEAVE;
            POPSTACK;

            if (empty_return) {
                goto definition_deferred;
            }

            if (prop_definition) {

                /* If the definition is for something not known at this time,
                 * we toss it, and go return the main property name, as that's
                 * the one the user will be aware of */
                if (! is_invlist(prop_definition)) {
                    SvREFCNT_dec_NN(prop_definition);
                    goto definition_deferred;
                }

                sv_2mortal(prop_definition);
            }

            /* And return */
            return prop_definition;

        }   /* End of calling the subroutine for the user-defined property */
    }       /* End of it could be a user-defined property */

    /* Here it wasn't a user-defined property that is known at this time.  See
     * if it is a Unicode property */

    lookup_len = j;     /* This is a more mnemonic name than 'j' */

    /* Get the index into our pointer table of the inversion list corresponding
     * to the property */
    table_index = match_uniprop((U8 *) lookup_name, lookup_len);

    /* If it didn't find the property ... */
    if (table_index == 0) {

        /* Try again stripping off any initial 'In' or 'Is' */
        if (starts_with_In_or_Is) {
            lookup_name += 2;
            lookup_len -= 2;
            equals_pos -= 2;
            slash_pos -= 2;

            table_index = match_uniprop((U8 *) lookup_name, lookup_len);
        }

        if (table_index == 0) {
            char * canonical;

            /* Here, we didn't find it.  If not a numeric type property, and
             * can't be a user-defined one, it isn't a legal property */
            if (! is_nv_type) {
                if (! could_be_user_defined) {
                    goto failed;
                }

                /* Here, the property name is legal as a user-defined one.   At
                 * compile time, it might just be that the subroutine for that
                 * property hasn't been encountered yet, but at runtime, it's
                 * an error to try to use an undefined one */
                if (! deferrable) {
                    if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                    sv_catpvs(msg, "Unknown user-defined property name");
                    goto append_name_to_msg;
                }

                goto definition_deferred;
            } /* End of isn't a numeric type property */

            /* The numeric type properties need more work to decide.  What we
             * do is make sure we have the number in canonical form and look
             * that up. */

            if (slash_pos < 0) {    /* No slash */

                /* When it isn't a rational, take the input, convert it to a
                 * NV, then create a canonical string representation of that
                 * NV. */

                NV value;
                SSize_t value_len = lookup_len - equals_pos;

                /* Get the value */
                if (   value_len <= 0
                    || my_atof3(lookup_name + equals_pos, &value,
                                value_len)
                          != lookup_name + lookup_len)
                {
                    goto failed;
                }

                /* If the value is an integer, the canonical value is integral
                 * */
                if (Perl_ceil(value) == value) {
                    canonical = Perl_form(aTHX_ "%.*s%.0" NVff,
                                            equals_pos, lookup_name, value);
                }
                else {  /* Otherwise, it is %e with a known precision */
                    char * exp_ptr;

                    canonical = Perl_form(aTHX_ "%.*s%.*" NVef,
                                                equals_pos, lookup_name,
                                                PL_E_FORMAT_PRECISION, value);

                    /* The exponent generated is expecting two digits, whereas
                     * %e on some systems will generate three.  Remove leading
                     * zeros in excess of 2 from the exponent.  We start
                     * looking for them after the '=' */
                    exp_ptr = strchr(canonical + equals_pos, 'e');
                    if (exp_ptr) {
                        char * cur_ptr = exp_ptr + 2; /* past the 'e[+-]' */
                        SSize_t excess_exponent_len = strlen(cur_ptr) - 2;

                        assert(*(cur_ptr - 1) == '-' || *(cur_ptr - 1) == '+');

                        if (excess_exponent_len > 0) {
                            SSize_t leading_zeros = strspn(cur_ptr, "0");
                            SSize_t excess_leading_zeros
                                    = MIN(leading_zeros, excess_exponent_len);
                            if (excess_leading_zeros > 0) {
                                Move(cur_ptr + excess_leading_zeros,
                                     cur_ptr,
                                     strlen(cur_ptr) - excess_leading_zeros
                                       + 1,  /* Copy the NUL as well */
                                     char);
                            }
                        }
                    }
                }
            }
            else {  /* Has a slash.  Create a rational in canonical form  */
                UV numerator, denominator, gcd, trial;
                const char * end_ptr;
                const char * sign = "";

                /* We can't just find the numerator, denominator, and do the
                 * division, then use the method above, because that is
                 * inexact.  And the input could be a rational that is within
                 * epsilon (given our precision) of a valid rational, and would
                 * then incorrectly compare valid.
                 *
                 * We're only interested in the part after the '=' */
                const char * this_lookup_name = lookup_name + equals_pos;
                lookup_len -= equals_pos;
                slash_pos -= equals_pos;

                /* Handle any leading minus */
                if (this_lookup_name[0] == '-') {
                    sign = "-";
                    this_lookup_name++;
                    lookup_len--;
                    slash_pos--;
                }

                /* Convert the numerator to numeric */
                end_ptr = this_lookup_name + slash_pos;
                if (! grok_atoUV(this_lookup_name, &numerator, &end_ptr)) {
                    goto failed;
                }

                /* It better have included all characters before the slash */
                if (*end_ptr != '/') {
                    goto failed;
                }

                /* Set to look at just the denominator */
                this_lookup_name += slash_pos;
                lookup_len -= slash_pos;
                end_ptr = this_lookup_name + lookup_len;

                /* Convert the denominator to numeric */
                if (! grok_atoUV(this_lookup_name, &denominator, &end_ptr)) {
                    goto failed;
                }

                /* It better be the rest of the characters, and don't divide by
                 * 0 */
                if (   end_ptr != this_lookup_name + lookup_len
                    || denominator == 0)
                {
                    goto failed;
                }

                /* Get the greatest common denominator using
                   http://en.wikipedia.org/wiki/Euclidean_algorithm */
                gcd = numerator;
                trial = denominator;
                while (trial != 0) {
                    UV temp = trial;
                    trial = gcd % trial;
                    gcd = temp;
                }

                /* If already in lowest possible terms, we have already tried
                 * looking this up */
                if (gcd == 1) {
                    goto failed;
                }

                /* Reduce the rational, which should put it in canonical form
                 * */
                numerator /= gcd;
                denominator /= gcd;

                canonical = Perl_form(aTHX_ "%.*s%s%" UVuf "/%" UVuf,
                        equals_pos, lookup_name, sign, numerator, denominator);
            }

            /* Here, we have the number in canonical form.  Try that */
            table_index = match_uniprop((U8 *) canonical, strlen(canonical));
            if (table_index == 0) {
                goto failed;
            }
        }   /* End of still didn't find the property in our table */
    }       /* End of       didn't find the property in our table */

    /* Here, we have a non-zero return, which is an index into a table of ptrs.
     * A negative return signifies that the real index is the absolute value,
     * but the result needs to be inverted */
    if (table_index < 0) {
        invert_return = TRUE;
        table_index = -table_index;
    }

    /* Out-of band indices indicate a deprecated property.  The proper index is
     * modulo it with the table size.  And dividing by the table size yields
     * an offset into a table constructed by regen/mk_invlists.pl to contain
     * the corresponding warning message */
    if (table_index > MAX_UNI_KEYWORD_INDEX) {
        Size_t warning_offset = table_index / MAX_UNI_KEYWORD_INDEX;
        table_index %= MAX_UNI_KEYWORD_INDEX;
        Perl_ck_warner_d(aTHX_ packWARN(WARN_DEPRECATED),
                "Use of '%.*s' in \\p{} or \\P{} is deprecated because: %s",
                (int) name_len, name, deprecated_property_msgs[warning_offset]);
    }

    /* In a few properties, a different property is used under /i.  These are
     * unlikely to change, so are hard-coded here. */
    if (to_fold) {
        if (   table_index == UNI_XPOSIXUPPER
            || table_index == UNI_XPOSIXLOWER
            || table_index == UNI_TITLE)
        {
            table_index = UNI_CASED;
        }
        else if (   table_index == UNI_UPPERCASELETTER
                 || table_index == UNI_LOWERCASELETTER
#  ifdef UNI_TITLECASELETTER   /* Missing from early Unicodes */
                 || table_index == UNI_TITLECASELETTER
#  endif
        ) {
            table_index = UNI_CASEDLETTER;
        }
        else if (  table_index == UNI_POSIXUPPER
                || table_index == UNI_POSIXLOWER)
        {
            table_index = UNI_POSIXALPHA;
        }
    }

    /* Create and return the inversion list */
    prop_definition =_new_invlist_C_array(uni_prop_ptrs[table_index]);
    sv_2mortal(prop_definition);


    /* See if there is a private use override to add to this definition */
    {
        COPHH * hinthash = (IN_PERL_COMPILETIME)
                           ? CopHINTHASH_get(&PL_compiling)
                           : CopHINTHASH_get(PL_curcop);
	SV * pu_overrides = cophh_fetch_pv(hinthash, "private_use", 0, 0);

        if (UNLIKELY(pu_overrides && SvPOK(pu_overrides))) {

            /* See if there is an element in the hints hash for this table */
            SV * pu_lookup = Perl_newSVpvf(aTHX_ "%d=", table_index);
            const char * pos = strstr(SvPVX(pu_overrides), SvPVX(pu_lookup));

            if (pos) {
                bool dummy;
                SV * pu_definition;
                SV * pu_invlist;
                SV * expanded_prop_definition =
                            sv_2mortal(invlist_clone(prop_definition, NULL));

                /* If so, it's definition is the string from here to the next
                 * \a character.  And its format is the same as a user-defined
                 * property */
                pos += SvCUR(pu_lookup);
                pu_definition = newSVpvn(pos, strchr(pos, '\a') - pos);
                pu_invlist = handle_user_defined_property(lookup_name,
                                                          lookup_len,
                                                          0, /* Not UTF-8 */
                                                          0, /* Not folded */
                                                          runtime,
                                                          deferrable,
                                                          pu_definition,
                                                          &dummy,
                                                          msg,
                                                          level);
                if (TAINT_get) {
                    if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
                    sv_catpvs(msg, "Insecure private-use override");
                    goto append_name_to_msg;
                }

                /* For now, as a safety measure, make sure that it doesn't
                 * override non-private use code points */
                _invlist_intersection(pu_invlist, PL_Private_Use, &pu_invlist);

                /* Add it to the list to be returned */
                _invlist_union(prop_definition, pu_invlist,
                               &expanded_prop_definition);
                prop_definition = expanded_prop_definition;
                Perl_ck_warner_d(aTHX_ packWARN(WARN_EXPERIMENTAL__PRIVATE_USE), "The private_use feature is experimental");
            }
        }
    }

    if (invert_return) {
        _invlist_invert(prop_definition);
    }
    return prop_definition;


  failed:
    if (non_pkg_begin != 0) {
        if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
        sv_catpvs(msg, "Illegal user-defined property name");
    }
    else {
        if (SvCUR(msg) > 0) sv_catpvs(msg, "; ");
        sv_catpvs(msg, "Can't find Unicode property definition");
    }
    /* FALLTHROUGH */

  append_name_to_msg:
    {
        const char * prefix = (runtime && level == 0) ?  " \\p{" : " \"";
        const char * suffix = (runtime && level == 0) ?  "}" : "\"";

        sv_catpv(msg, prefix);
        Perl_sv_catpvf(aTHX_ msg, "%" UTF8f, UTF8fARG(is_utf8, name_len, name));
        sv_catpv(msg, suffix);
    }

    return NULL;

  definition_deferred:

    /* Here it could yet to be defined, so defer evaluation of this
     * until its needed at runtime.  We need the fully qualified property name
     * to avoid ambiguity, and a trailing newline */
    if (! fq_name) {
        fq_name = S_get_fq_name(aTHX_ name, name_len, is_utf8,
                                      non_pkg_begin != 0 /* If has "::" */
                               );
    }
    sv_catpvs(fq_name, "\n");

    *user_defined_ptr = TRUE;
    return fq_name;