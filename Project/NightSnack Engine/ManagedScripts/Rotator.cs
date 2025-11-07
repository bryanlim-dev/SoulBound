// ============================================================
// RotatorScript.cs - Rotates object continuously
// ============================================================
namespace ManagedScripts
{
    public class RotatorScript : Script
    {
        private const float rotationSpeed = 50.0f; // degrees per second

        public override void OnStart()
        {
            //Log("RotatorScript started!");
            //Log($"Initial rotation: {Rotation}");
            //Log($"Rotation speed: {rotationSpeed} degrees/sec");
        }

        public override void OnUpdate()
        {
            // Get current rotation
            float rotation = Rotation;

            // Rotate around Y axis
            rotation += rotationSpeed * DeltaTime;

            // Wrap around 360 degrees
            if (rotation >= 360.0f)
            {
                rotation -= 360.0f;
            }

            // Set back to ECS
            Rotation = rotation;

            //// Log every 45 degrees
            //if ((int)rotation % 45 == 0 && rotation > 0)
            //{
            //    Log($"Current rotation: {rotation:F1} degrees");
            //}
        }

        public override void OnDestroy()
        {
            //Log($"RotatorScript destroyed at rotation: {Rotation} degrees");
        }
    }
}
