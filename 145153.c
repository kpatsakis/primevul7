static size_t php_output_wrapper(const char *str, size_t str_length)
{
	return php_output_write(str, str_length);
}