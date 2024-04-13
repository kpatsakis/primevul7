push_handlers (void)
{
	orig_error_handler = TIFFSetErrorHandler (NULL);
	orig_warning_handler = TIFFSetWarningHandler (NULL);
}