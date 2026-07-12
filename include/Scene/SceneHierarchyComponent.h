//
// Created by joost on 5/31/26.
//

#pragma once

#include "Object/ObjectID.h"
namespace Spelt {
    struct SceneHierarchyComponent {
        ObjectID parent = 0;
        ObjectID firstChild = 0;
        ObjectID prevSibling = 0;
        ObjectID nextSibling = 0;
    };
}
