# Embedding Assets

Sometimes you want to have an asset compiled straight into your program. Spelt engine has a built in way to go about this.

## Embedding assets

Embedded assets are represented by normal C-header files. They contain a struct (often named \[filename\]_type) that has the binary data of the asset. 

I created a tool to create these header files. The tool can be found [here](https://github.com/r11b22/SpeltEngine-AssetHeaderGenerator) along with a guide on how to use it.

The Asset Header generator should just give you a header file that you can include in your source code.

## Using embedded assets

After including the source code you are free to use the binary data as you please.

With SpeltEngine some assets support loading directly from binary. As an example we will use textures. In this case we will be loading the tigre texture that is used in the test scene.

```cpp
#include "Assets/Texture_1PNG.h"

...

MainScene::MainScene() {
    addAsset(AssetLoadInfo<Texture>::FromEmbedded("tiger", Texture_1_png));
}
```

As you can see loading the asset works exactly the same as loading a normal asset that is taken from a path.
