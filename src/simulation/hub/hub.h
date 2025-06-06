#ifndef SHODRONE_HUB_H
#define SHODRONE_HUB_H
#include "../../structs.h"
#include "../../logger/logger.h"
#include "../../errors.h"
int init(int nDrones, Hub *);
int start(Hub* hub);
void clean(Hub* hub);
#endif //SHODRONE_HUB_H
