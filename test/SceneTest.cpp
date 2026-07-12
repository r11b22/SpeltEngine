//
// Created by joost on 6/1/26.
//

#include <Scene/Scene.hpp>
#include <algorithm>

#include "Object/Object.h"
#include "catch2/catch_test_macros.hpp"

using namespace Spelt;

// ---------------------------------------------------------------------------
// Operations
// ---------------------------------------------------------------------------

TEST_CASE("Scene can retrieve objects by name", "[scene][operations]") {
    Scene scene;
    ObjectReference<Object> root = scene.createObject<Object>("root");

    auto result = scene.getObjectByName<Object>("root");
    REQUIRE(root == result);
}

TEST_CASE("Scene returns nullptr for unknown name", "[scene][operations]") {
    Scene scene;
    REQUIRE(scene.getObjectByName<Object>("nonexistent").isNoReference());
}

// ---------------------------------------------------------------------------
// Hierarchy — root level
// ---------------------------------------------------------------------------

TEST_CASE("Scene manages a single root object", "[scene][hierarchy]") {
    Scene scene;
    ObjectReference<Object> root = scene.createObject<Object>("root");

    std::vector<ObjectReference<Object>> rootObjects = scene.getRootChildrenOfType<Object>();
    REQUIRE(rootObjects.size() == 1);
    REQUIRE(rootObjects[0] == root);
    REQUIRE(rootObjects[0]->getName() == "root");
}

TEST_CASE("Scene manages multiple root objects and their siblings", "[scene][hierarchy]") {
    Scene scene;
    ObjectReference<Object> root1 = scene.createObject<Object>("root1");
    ObjectReference<Object> root2 = scene.createObject<Object>("root2");
    ObjectReference<Object> root3 = scene.createObject<Object>("root3");

    std::vector<ObjectReference<Object>> rootObjects = scene.getRootChildrenOfType<Object>();
    REQUIRE(rootObjects.size() == 3);
    REQUIRE(rootObjects[0] == root1);
    REQUIRE(rootObjects[1] == root2);
    REQUIRE(rootObjects[2] == root3);

    std::vector<ObjectReference<Object>> root2Siblings = scene.getSiblingsOfType<Object>(root2);
    REQUIRE(root2Siblings.size() == 2);
    REQUIRE(std::find(root2Siblings.begin(), root2Siblings.end(), root1) != root2Siblings.end());
    REQUIRE(std::find(root2Siblings.begin(), root2Siblings.end(), root3) != root2Siblings.end());
}

// ---------------------------------------------------------------------------
// Hierarchy — parent / child relationships
// ---------------------------------------------------------------------------

TEST_CASE("Scene tracks single child relationships", "[scene][hierarchy]") {
    Scene scene;
    ObjectReference<Object> root1  = scene.createObject<Object>("root1");
    ObjectReference<Object> child1 = scene.createObject<Object>("child1");

    scene.setParent(child1, root1);

    // child1 should now appear under root1
    std::vector<ObjectReference<Object>> children = scene.getChildrenOfType<Object>(root1);
    REQUIRE(children.size() == 1);
    REQUIRE(children[0] == child1);

    // child1 is no longer a root-level object
    std::vector<ObjectReference<Object>> rootObjects = scene.getRootChildrenOfType<Object>();
    REQUIRE(rootObjects.size() == 1);
    REQUIRE(rootObjects[0] == root1);

    // getParent round-trips correctly
    REQUIRE(scene.getParent<Object>(child1) == root1);
    REQUIRE(scene.getParent<Object>(root1).isNoReference());
}

TEST_CASE("Scene tracks multiple children and their sibling connections", "[scene][hierarchy]") {
    Scene scene;
    ObjectReference<Object> root1  = scene.createObject<Object>("root1");
    ObjectReference<Object> child1 = scene.createObject<Object>("child1");
    ObjectReference<Object> child2 = scene.createObject<Object>("child2");
    ObjectReference<Object> child3 = scene.createObject<Object>("child3");

    scene.setParent(child1, root1);
    scene.setParent(child2, root1);
    scene.setParent(child3, root1);

    // Only root1 remains at the top level
    REQUIRE(scene.getRootChildrenOfType<Object>().size() == 1);

    std::vector<ObjectReference<Object>> children = scene.getChildrenOfType<Object>(root1);
    REQUIRE(children.size() == 3);
    REQUIRE(children[0] == child1);
    REQUIRE(children[1] == child2);
    REQUIRE(children[2] == child3);

    std::vector<ObjectReference<Object>> child2Siblings = scene.getSiblingsOfType<Object>(child2);
    REQUIRE(child2Siblings.size() == 2);
    REQUIRE(std::find(child2Siblings.begin(), child2Siblings.end(), child1) != child2Siblings.end());
    REQUIRE(std::find(child2Siblings.begin(), child2Siblings.end(), child3) != child2Siblings.end());
}

// ---------------------------------------------------------------------------
// Hierarchy — reparenting
// ---------------------------------------------------------------------------

TEST_CASE("Scene dynamically handles reparenting subtrees", "[scene][hierarchy]") {
    Scene scene;
    ObjectReference<Object> root1  = scene.createObject<Object>("root1");
    ObjectReference<Object> child1 = scene.createObject<Object>("child1");
    ObjectReference<Object> child2 = scene.createObject<Object>("child2");
    ObjectReference<Object> child3 = scene.createObject<Object>("child3");

    ObjectReference<Object> child2child1 = scene.createObject<Object>("child2child1");
    ObjectReference<Object> child2child2 = scene.createObject<Object>("child2child2");

    scene.setParent(child1, root1);
    scene.setParent(child2, root1);
    scene.setParent(child3, root1);
    scene.setParent(child2child1, child2);
    scene.setParent(child2child2, child2);

    // Verify initial setup of child2's subtree
    std::vector<ObjectReference<Object>> child2children = scene.getChildrenOfType<Object>(child2);
    REQUIRE(child2children.size() == 2);
    REQUIRE(std::find(child2children.begin(), child2children.end(), child2child1) != child2children.end());
    REQUIRE(std::find(child2children.begin(), child2children.end(), child2child2) != child2children.end());

    // Move child2 under child1
    scene.setParent(child2, child1);

    // child1 now has child2 as its only child
    std::vector<ObjectReference<Object>> child1children = scene.getChildrenOfType<Object>(child1);
    REQUIRE(child1children.size() == 1);
    REQUIRE(child1children[0] == child2);

    // root1's direct children are now only child1 and child3
    std::vector<ObjectReference<Object>> root1children = scene.getChildrenOfType<Object>(root1);
    REQUIRE(root1children.size() == 2);

    // child1's only sibling is child3
    std::vector<ObjectReference<Object>> child1siblings = scene.getSiblingsOfType<Object>(child1);
    REQUIRE(child1siblings.size() == 1);
    REQUIRE(child1siblings[0] == child3);

    // child2's subtree remains completely intact after the move
    std::vector<ObjectReference<Object>> child2childrenAfter = scene.getChildrenOfType<Object>(child2);
    REQUIRE(child2childrenAfter.size() == 2);
    REQUIRE(std::find(child2childrenAfter.begin(), child2childrenAfter.end(), child2child1) != child2childrenAfter.end());
    REQUIRE(std::find(child2childrenAfter.begin(), child2childrenAfter.end(), child2child2) != child2childrenAfter.end());
}

// ---------------------------------------------------------------------------
// Destroy
// ---------------------------------------------------------------------------

TEST_CASE("Destroying a leaf object removes it from its parent's children", "[scene][destroy]") {
    Scene scene;
    ObjectReference<Object> root   = scene.createObject<Object>("root");
    ObjectReference<Object> child1 = scene.createObject<Object>("child1");
    ObjectReference<Object> child2 = scene.createObject<Object>("child2");

    scene.setParent(child1, root);
    scene.setParent(child2, root);

    scene.destroyObject(child1);

    // child1 is gone from the scene
    REQUIRE(scene.getObjectByName<Object>("child1").isNoReference());

    // root still has exactly child2
    std::vector<ObjectReference<Object>> children = scene.getChildrenOfType<Object>(root);
    REQUIRE(children.size() == 1);
    REQUIRE(children[0] == child2);
}

TEST_CASE("Destroying an object also destroys all of its descendants", "[scene][destroy]") {
    Scene scene;
    ObjectReference<Object> root       = scene.createObject<Object>("root");
    ObjectReference<Object> child      = scene.createObject<Object>("child");
    ObjectReference<Object> grandchild = scene.createObject<Object>("grandchild");

    scene.setParent(child, root);
    scene.setParent(grandchild, child);

    scene.destroyObject(child);

    // Both child and its descendant are gone
    REQUIRE(scene.getObjectByName<Object>("child").isNoReference());
    REQUIRE(scene.getObjectByName<Object>("grandchild").isNoReference());

    // root has no children left
    REQUIRE(scene.getChildrenOfType<Object>(root).empty());

    // root itself is still alive
    REQUIRE(scene.getObjectByName<Object>("root") == root);
}

TEST_CASE("Destroying an object with deep subtree removes the entire subtree", "[scene][destroy]") {
    Scene scene;
    ObjectReference<Object> root   = scene.createObject<Object>("root");
    ObjectReference<Object> child1 = scene.createObject<Object>("child1");
    ObjectReference<Object> child2 = scene.createObject<Object>("child2");
    ObjectReference<Object> gc1    = scene.createObject<Object>("gc1");   // child of child1
    ObjectReference<Object> gc2    = scene.createObject<Object>("gc2");   // child of child1
    ObjectReference<Object> gc3    = scene.createObject<Object>("gc3");   // child of child2

    scene.setParent(child1, root);
    scene.setParent(child2, root);
    scene.setParent(gc1, child1);
    scene.setParent(gc2, child1);
    scene.setParent(gc3, child2);

    // Destroy child1 — takes gc1 and gc2 with it
    scene.destroyObject(child1);

    REQUIRE(scene.getObjectByName<Object>("child1").isNoReference());
    REQUIRE(scene.getObjectByName<Object>("gc1").isNoReference());
    REQUIRE(scene.getObjectByName<Object>("gc2").isNoReference());

    // child2 and its subtree are untouched
    REQUIRE(scene.getObjectByName<Object>("child2") == child2);
    REQUIRE(scene.getObjectByName<Object>("gc3")    == gc3);

    std::vector<ObjectReference<Object>> rootChildren = scene.getChildrenOfType<Object>(root);
    REQUIRE(rootChildren.size() == 1);
    REQUIRE(rootChildren[0] == child2);
}

TEST_CASE("Destroying a root object with children removes all from the scene", "[scene][destroy]") {
    Scene scene;
    ObjectReference<Object> root1  = scene.createObject<Object>("root1");
    ObjectReference<Object> root2  = scene.createObject<Object>("root2");
    ObjectReference<Object> child1 = scene.createObject<Object>("child1");
    ObjectReference<Object> child2 = scene.createObject<Object>("child2");

    scene.setParent(child1, root1);
    scene.setParent(child2, root1);

    scene.destroyObject(root1);

    REQUIRE(scene.getObjectByName<Object>("root1").isNoReference());
    REQUIRE(scene.getObjectByName<Object>("child1").isNoReference());
    REQUIRE(scene.getObjectByName<Object>("child2").isNoReference());

    // root2 is the only remaining root-level object
    std::vector<ObjectReference<Object>> rootObjects = scene.getRootChildrenOfType<Object>();
    REQUIRE(rootObjects.size() == 1);
    REQUIRE(rootObjects[0] == root2);
}
