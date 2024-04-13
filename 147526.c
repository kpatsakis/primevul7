PHPAPI php_url *php_url_parse_ex(char const *str, size_t length)
{
	zend_bool has_port;
	return php_url_parse_ex2(str, length, &has_port);
}