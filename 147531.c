PHPAPI void php_url_free(php_url *theurl)
{
	if (theurl->scheme)
		zend_string_release_ex(theurl->scheme, 0);
	if (theurl->user)
		zend_string_release_ex(theurl->user, 0);
	if (theurl->pass)
		zend_string_release_ex(theurl->pass, 0);
	if (theurl->host)
		zend_string_release_ex(theurl->host, 0);
	if (theurl->path)
		zend_string_release_ex(theurl->path, 0);
	if (theurl->query)
		zend_string_release_ex(theurl->query, 0);
	if (theurl->fragment)
		zend_string_release_ex(theurl->fragment, 0);
	efree(theurl);
}