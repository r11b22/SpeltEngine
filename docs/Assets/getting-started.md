# Getting started with custom assets in SpeltEngine

WORK IN PROGRESS

## Creating an asset loader

This section wil outline an example of how to build the asset loader for your custom asset. 
Creating an asset loader for your Custom asset is needed to be able to use them in the AssetManager attached to the app.

Take the following Mesh loader. Mesh inherits from Asset
```c++
class MeshLoader {
    private:
        Assimp::Importer* mImporter = nullptr;
        const aiScene* mScene = nullptr;
    public:
        MeshLoader();
        ~MeshLoader();
        void readFile(const std::filesystem::path& path, bool collapse = true);

        std::vector<float> getVertices(int mesh, glm::vec3 = glm::vec3{1.0f});
        std::vector<unsigned int> getIndices(int mesh);

        Mesh createMesh(std::string name, int mesh, glm::vec3 scale = glm::vec3{1.0f});
    private:
};

```
This loader can by default not be used with the Asset loading system built into SpeltEngine.
We need to tell the engine what type the asset input is and what type that loads and how.

First we define the load input class. This class is called AssetLoadInfo<T>. This class should contain all data that is needed to load the asset from disk. 
```c++
template <>
struct AssetLoadInfo<Mesh> {
    std::string name;
    std::filesystem::path path;
    int meshIdx = 0;
    glm::vec3 scale {1.0f};
};
```

Now we need to tell the engine how to load the file from disk.
We define a struct `AssetLoader<Mesh>` that has this info. This struct has a single function static T load(AssetLoadInfo<T> asset). This function is meant to load the file from disk into the specific asset that is being loaded.
It is possible to put the full file-loading logic inside of the load function but it is not recommended.

```c++
template <>
struct AssetLoader<Mesh> {
    static Mesh load(AssetLoadInfo<Mesh> asset) {
        MeshLoader loader{};
        loader.readFile(asset.path);

        Mesh mesh = loader.createMesh(asset.name, asset.meshIdx, asset.scale);

        return std::move(mesh);
    }
};
```

This struct implements the load function. This function actually loads the thing into memory. In this function the translation between your own Loader Class/Function is defined. There are 2 possible defenitions for load:
`static Mesh load(AssetLoadInfo<Mesh> asset)`
and
`static Mesh load(AssetLoadInfo<Mesh> asset, AssetManager& manager)`.

That is everything that is needed to call `getAssetManager().loadAsset<Mesh>(AssetLoadInfo<Mesh>{});`. Important to keep in mind is that the definition for loadAsset<Mesh> is now dependant on the header that defines the AssetLoader. Make sure this header is included when using asset loading.
