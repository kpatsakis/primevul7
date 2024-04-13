usertype2s(int mask)
{
    const char *result = "unknown";
    if (mask & (1 << BOOLEAN)) {
	result = "boolean";
    } else if (mask & (1 << NUMBER)) {
	result = "number";
    } else if (mask & (1 << STRING)) {
	result = "string";
    }
    return result;
}