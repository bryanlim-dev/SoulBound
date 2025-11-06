
using ScriptAPI;

namespace ManagedScripts
{
    public abstract class Script
    {
        // Reference to Entity the Script is attached to
        public uint entityID { get; internal set; }

        // Override in derived classes;
        public virtual void OnStart() { };
        public virtual void OnUpdate() { };
        public virtual void OnDestory() { };

        protected Transform transform
        {
            get => Engine.API.GetTransform(entityID);
            set => Engine.API.SetTransform(entityID, value);
        }

        protected float DeltaTime => Engine.API.GetDeltaTime();
        protected float Time => Engine.API.GetTime();

        // Helper methods
        protected bool IsKeyPressed(int keyCode) => Engine.API.IsKeyPressed(keyCode);
        protected bool IsKeyDown(int keyCode) => Engine.API.IsKeyDown(keyCode);
        protected bool IsKeyReleased(int keyCode) => Engine.API.IsKeyReleased(keyCode);

        protected void PlaySound(string path) => Engine.API.PlaySound(path);
        protected void Log(string message) => Engine.API.Log(message);
    }

}