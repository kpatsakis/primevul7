_XkbFindNamedIndicatorMap(XkbSrvLedInfoPtr sli, Atom indicator, int *led_return)
{
    XkbIndicatorMapPtr map;

    /* search for the right indicator */
    map = NULL;
    if (sli->names && sli->maps) {
        int led;

        for (led = 0; (led < XkbNumIndicators) && (map == NULL); led++) {
            if (sli->names[led] == indicator) {
                map = &sli->maps[led];
                *led_return = led;
                break;
            }
        }
    }

    return map;
}