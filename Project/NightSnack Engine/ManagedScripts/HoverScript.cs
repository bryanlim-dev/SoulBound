// ============================================================
// RotatorScript.cs - Rotates object continuously
// ============================================================
namespace ManagedScripts
{
    public class HoverScript : Script
    {
        [Expose]
        public float moveSpeed = 150.0f;
        [Expose]
        public float time = 0.0f;

        public override void OnStart()
        {
            Log("HoverScript started!");
            Log($"Initial Position: {Position.X}, {Position.Y}");
            Log($"Move speed: {moveSpeed}.f");
        }

        public override void OnUpdate()
        {
            // Get current position
            Vector3 pos = Position;

            // Move along X-axis
            pos.X += moveSpeed * DeltaTime;
            time += DeltaTime;

            // Wrap around 360 degrees
            if (time >= 2.0f)
            {
                moveSpeed = -moveSpeed;
                time = 0.0f;
            }

            // Set back to ECS
            Position = pos;
        }

        public override void OnDestroy()
        {
        }
    }
}
