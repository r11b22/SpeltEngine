//
// Created by joost on 5/31/26.
//

#ifndef CGVCPROJECT_SCENEHIERARCHYCOMPONENT_H
#define CGVCPROJECT_SCENEHIERARCHYCOMPONENT_H
#include "ObjectID.h"

struct SceneHierarchyComponent {
    ObjectID parent = 0;
    ObjectID firstChild = 0;
    ObjectID prevSibling = 0;
    ObjectID nextSibling = 0;
};

#endif //CGVCPROJECT_SCENEHIERARCHYCOMPONENT_H