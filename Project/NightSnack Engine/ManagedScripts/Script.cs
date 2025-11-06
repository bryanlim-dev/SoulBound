namespace ManagedScripts
{
    // Base class for all game scripts
    public abstract class Script
    {
        // Set by engine when script is created
        public uint EntityId { get; internal set; }

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

        protected float DeltaTime => Engine.DeltaTime;
        //protected float Time => Engine.Time;

        // Helper methods
        protected void Log(string message) => Engine.Log(message);
        protected bool IsKeyPressed(int keyCode) => Engine.IsKeyPressed(keyCode);
        protected bool IsKeyDown(int keyCode) => Engine.IsKeyDown(keyCode);
        protected bool IsKeyReleased(int keyCode) => Engine.IsKeyReleased(keyCode);
        protected void PlaySound(string path) => Engine.PlaySound(path);
    }
}   // ManagedScripts
