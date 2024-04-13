static CURLcode parse_proxy_auth(struct SessionHandle *data,
                                 struct connectdata *conn)
{
  char proxyuser[MAX_CURL_USER_LENGTH]="";
  char proxypasswd[MAX_CURL_PASSWORD_LENGTH]="";

  if(data->set.str[STRING_PROXYUSERNAME] != NULL) {
    strncpy(proxyuser, data->set.str[STRING_PROXYUSERNAME],
            MAX_CURL_USER_LENGTH);
    proxyuser[MAX_CURL_USER_LENGTH-1] = '\0';   /*To be on safe side*/
  }
  if(data->set.str[STRING_PROXYPASSWORD] != NULL) {
    strncpy(proxypasswd, data->set.str[STRING_PROXYPASSWORD],
            MAX_CURL_PASSWORD_LENGTH);
    proxypasswd[MAX_CURL_PASSWORD_LENGTH-1] = '\0'; /*To be on safe side*/
  }

  conn->proxyuser = curl_easy_unescape(data, proxyuser, 0, NULL);
  if(!conn->proxyuser)
    return CURLE_OUT_OF_MEMORY;

  conn->proxypasswd = curl_easy_unescape(data, proxypasswd, 0, NULL);
  if(!conn->proxypasswd)
    return CURLE_OUT_OF_MEMORY;

  return CURLE_OK;
}