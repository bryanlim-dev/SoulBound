#pragma once

// C++/CLI header - bridges native C++ and managed C#
#using <System.dll>

#include "./Scripting/IScriptInterface.h"
#include "./Scripting/ScriptTypes.h"

namespace ScriptAPI
{
    public ref class EngineI
    {
    public:
        static void HelloWorld();
    };






    // -----------------------------------------------------------------------------

    // Managed struct visible to C#
    public value struct Vector2 {
        float X, Y;

        Vector2(float x, float y) : X(x), Y(y) {}

        // Convert to native
        NightSnack::Scripting::vec2 ToNative() {
            return NightSnack::Scripting::vec2(X, Y);
        }

        // Convert from native
        static Vector2 FromNative(const NightSnack::Scripting::vec2& v) {
            return Vector2(v.x, v.y);
        }
    };

    public value struct Vector3 {
        float X, Y, Z;

        Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

        // Convert to native
        NightSnack::Scripting::vec3 ToNative() {
            return NightSnack::Scripting::vec3(X, Y, Z);
        }

        // Convert from native
        static Vector3 FromNative(const NightSnack::Scripting::vec3& v) {
            return Vector3(v.x, v.y, v.z);
        }
    };

    public value struct Transform {
        Vector3 Position;
        float   Rotation;
        Vector2 Scale;

        NightSnack::Scripting::Trans ToNative() {
            NightSnack::Scripting::Trans t;
            t.pos = Position.ToNative();
            t.rot = Rotation;
            t.scale = Scale.ToNative();
            return t;
        }

        static Transform FromNative(const NightSnack::Scripting::Trans& t) {
            Transform result;
            result.Position = Vector3::FromNative(t.pos);
            result.Rotation = t.rot;
            result.Scale = Vector2::FromNative(t.scale);
            return result;
        }
    };

    // ref classes are classes in C#, value classes are structs in C#
    public ref class EngineAPI {
    private:
        NightSnack::Scripting::ScriptInterface* m_nativeInterface;

    public:
        EngineAPI(NightSnack::Scripting::ScriptInterface* nativeInterface);
        ~EngineAPI() = default;

        // Exposed to C#
        Transform GetTransform(unsigned int objID);
        void SetTransform(unsigned int objID, Transform transform);

        float GetDeltaTime();

        void Log(System::String^ message);
    };

    public ref class Engine {
    private:
        static EngineAPI^ s_instance = nullptr;

    public:
        static void Initialize(NightSnack::Scripting::ScriptInterface* nativeInterface) {
            if (s_instance == nullptr) {
                s_instance = gcnew EngineAPI(nativeInterface);
            }
        }
        static property EngineAPI^ API {
            EngineAPI^ get() { return s_instance; }
        }
    };

}   // ScriptAPI