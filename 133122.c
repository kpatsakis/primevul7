BOOL license_read_product_info(wStream* s, PRODUCT_INFO* productInfo)
{
	if (Stream_GetRemainingLength(s) < 8)
		return FALSE;

	Stream_Read_UINT32(s, productInfo->dwVersion); /* dwVersion (4 bytes) */

	Stream_Read_UINT32(s, productInfo->cbCompanyName); /* cbCompanyName (4 bytes) */

	if (Stream_GetRemainingLength(s) < productInfo->cbCompanyName + 4)
		return FALSE;

	productInfo->pbCompanyName = (BYTE*) malloc(productInfo->cbCompanyName);
	Stream_Read(s, productInfo->pbCompanyName, productInfo->cbCompanyName);

	Stream_Read_UINT32(s, productInfo->cbProductId); /* cbProductId (4 bytes) */

	if (Stream_GetRemainingLength(s) < productInfo->cbProductId)
	{
		free(productInfo->pbCompanyName);
		productInfo->pbCompanyName = NULL;
		return FALSE;
	}

	productInfo->pbProductId = (BYTE*) malloc(productInfo->cbProductId);
	Stream_Read(s, productInfo->pbProductId, productInfo->cbProductId);

	return TRUE;
}