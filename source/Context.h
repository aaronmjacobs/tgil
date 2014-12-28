#ifndef CONTEXT_H
#define CONTEXT_H

class AssetManager;

class Context {
protected:
   const UPtr<AssetManager> assetManager;

public:
   Context();

   virtual ~Context();

   AssetManager &getAssetManager() const;
};

#endif
