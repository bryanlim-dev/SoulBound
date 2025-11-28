// ============================================================
// RotatorScript.cs - Rotates object continuously
// ============================================================
namespace ManagedScripts
{
    public class HoverScript : Script
    {
        [SerializeField]
        public float moveSpeed = 150.0f;
        [SerializeField]
        public float time = 0.0f;

        public Transform transform;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            //Log($"Entity Script ID: {EntityId}");
            //Engine.CreateScript(EntityId, "RotatorScript");

            //Log("HoverScript started!");
            //Log($"Initial Position: {Position.X}, {Position.Y}");
            //Log($"Move speed: {moveSpeed}.f");
        }

        public override void OnUpdate()
        {
            // Get current position
            Vector3 pos = transform.Position;

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
            transform.Position = pos;
        }

        public override void OnDestroy()
        {
        }
    }
}
