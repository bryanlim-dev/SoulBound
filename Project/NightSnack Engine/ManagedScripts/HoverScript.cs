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

        RotatorScript rotate = null;

        public Transform transform;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            //Log($"Player Script ID: {GetPlayerID()}");
            //Engine.CreateScript(EntityId, "RotatorScript");

            //Log("HoverScript started!");
            //Log($"Initial Position: {Position.X}, {Position.Y}");
            //Log($"Move speed: {moveSpeed}.f");

        }

        public override void OnUpdate()
        {
            //Engine.DeleteScript(EntityId, "RotatorScript");
            //if (rotate == null)
            //{
            //    rotate = Engine.GetScript<RotatorScript>(EntityId, "RotatorScript");
            //    Log($"Got RotatorScript: {rotate != null}, for ID: {EntityId}");
            //    if (rotate != null)
            //        rotate.rotationSpeed = 200.0f;
            //}

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
