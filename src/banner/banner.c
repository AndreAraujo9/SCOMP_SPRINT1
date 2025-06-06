#include <stdio.h>
#include <string.h>
#include "banner.h"

#define VERSION_STRING APP_VERSION
#define APP_STRING APP_NAME

static const char *BANNER =
        "  ______   __                   __                               ______                                      _                      \n"
        ".' ____ \\ [  |                 |  ]                             |_   _ `.                                   / \\                     \n"
        "| (___ \\_| | |--.   .--.   .--.| |  .--.  _ .--. _ .--.  .---.    | | `. \\_ .--.  .--.  _ .--.  .---.      / _ \\   _ .--.  _ .--.   \n"
        " _.____`.  | .-. |/ .'`\\ Y /'`\\' |/ .'`\\ [ `/'`\\| `.-. |/ /__\\\\   | |  | [ `/'`\\] .'`\\ [ `.-. |/ /__\\\\    / ___ \\ [ '/'`\\ [ '/'`\\ \\ \n"
        "| \\____) | | | | || \\__. | \\__/  || \\__. || |    | | | || \\__.,  _| |_.' /| |   | \\__. || | | || \\__.,  _/ /   \\ \\_| \\__/ || \\__/ | \n"
        " \\______.'[___]|__]'.__.' '.__.;__]'.__.'[___]  [___||__]'.__.' |______.'[___]   '.__.'[___||__]'.__.' |____| |____| ;.__/ | ;.__/  \n"
        "                                                                                                                  [__|    [__|     \n"
        "$APP_NAME $VERSION\n"
        "Powered by C23 on Linux\n";

char * bannerFormatter(){
    char * buffer = calloc(8192, sizeof(char));
    const char *src = BANNER;
    char *dst = buffer;

    while (*src) {
        if (strncmp(src, "$APP_NAME", 9) == 0) {
            dst += sprintf(dst, "%s", APP_STRING);
            src += 9;
        } else if (strncmp(src, "$VERSION", 8) == 0) {
            dst += sprintf(dst, "%s", VERSION_STRING);
            src += 8;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
    return buffer;
}

void print_banner() {
   char * buffer = bannerFormatter();
    printf("%s\n", buffer);
    free(buffer);
}

char * bannerStr(){
    return bannerFormatter();
}