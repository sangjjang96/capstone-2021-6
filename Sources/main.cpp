#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include "Core/Application.h"
#include "Rendering/TestScene.h"

int main()
{
   /** Memory leak checking */
#ifdef _DEBUG
   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
   //_CrtSetBreakAlloc(x);
#endif

   Application* app = new Application("Lit", 1200, 800);
   Scene* scene = new TestScene();
   scene->Init();
   int res = app->Run(*scene);
   delete app;
   app = nullptr;

   return res;
}