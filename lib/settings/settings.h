#ifndef INTERNET_OF_BUTTON_V0_4_0_SETTINGS_H
#define INTERNET_OF_BUTTON_V0_4_0_SETTINGS_H

#define SETTINGS_PREFIX (char *)"/"
#define SETTINGS_POSTFIX ".txt"

bool settings_init();

void settings_get(char **target, const char *name);

void settings_get_int(unsigned int *target, const char *name);

#endif //INTERNET_OF_BUTTON_V0_4_0_SETTINGS_H
