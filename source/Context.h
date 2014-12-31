#ifndef CONTEXT_H
#define CONTEXT_H

class AssetManager;
class Scene;

class Context {
protected:
   const UPtr<AssetManager> assetManager;
   const UPtr<Scene> scene;

public:
   Context();

   virtual ~Context();

   AssetManager& getAssetManager() const;
   Scene& getScene() const;
};

#endif
