#include "EngineInterface.h"

namespace ScriptAPI
{
    void EngineI::HelloWorld()
    {
        System::Console::WriteLine("Hello Managed World!");
    }








    EngineAPI::EngineAPI(NightSnack::Scripting::ScriptInterface* nativeInterface)
        : m_nativeInterface(nativeInterface) {
    }

    Transform EngineAPI::GetTransform(unsigned int objID) {
        NightSnack::Scripting::GameObjectHandle handle(objID);
        auto nativeTransform = m_nativeInterface->GetTransform(handle);
        return Transform::FromNative(nativeTransform);
    }
}