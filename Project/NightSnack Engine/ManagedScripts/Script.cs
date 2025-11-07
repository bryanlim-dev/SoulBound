namespace ManagedScripts
{
    // Base class for all game scripts
    public abstract class Script
    {
        // Set by engine when script is created
        public uint EntityId;

        // Override these in your script classes
        public virtual void OnStart() { }
        public virtual void OnUpdate() { }
        public virtual void OnDestroy() { }

        // Convenience properties
        protected Vector3 Position
        {
            get => Engine.GetPosition(EntityId);
            set => Engine.SetPosition(EntityId, value);
        }
        protected float Rotation
        {
            get => Engine.GetRotation(EntityId);
            set => Engine.SetRotation(EntityId, value);
        }
        protected Vector2 Scale
        {
            get => Engine.GetScale(EntityId);
            set => Engine.SetScale(EntityId, value);
        }

        protected float DeltaTime => Engine.DeltaTime;
        //protected float Time => Engine.Time;

        // Helper methods
        protected void Log(string message) => Engine.Log(message);
        protected bool IsKeyPressed(int key) => Engine.IsKeyPressed(key);
        protected bool IsKeyDown(int key) => Engine.IsKeyDown(key);
        protected bool IsKeyReleased(int key) => Engine.IsKeyReleased(key);
        protected void PlaySound(string path) => Engine.PlaySound(path);
    }
}   // ManagedScripts
