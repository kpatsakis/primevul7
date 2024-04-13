name_to_backref_error(VALUE name)
{
    rb_raise(rb_eIndexError, "undefined group name reference: % "PRIsVALUE,
	     name);
}