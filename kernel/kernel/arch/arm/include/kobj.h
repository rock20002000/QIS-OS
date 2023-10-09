#pragma once

#include "object.h"
#include "semaphore.h"


void phy_object_init(struct phy_object         *object,
                    enum phy_object_class_type type,
                    const char                 *name);

int _ipc_object_init(struct phy_ipc_object *ipc);

phy_object_t phy_object_allocate(enum phy_object_class_type type, const char *name);

void phy_object_detach(phy_object_t object);

