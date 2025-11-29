// ============================================================
// BobberScript.cs - Causes object to bob up and down
// ============================================================
using System;

namespace ManagedScripts
{
    public class BobberScript : Script
    {
        [SerializeField]
        public float bobSpeed = 15.0f;
        [SerializeField]
        public float time = 0.0f;

        public Transform transform;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
        }

        public override void OnUpdate()
        {
            // Get current position
            Vector3 pos = transform.Position;

            // Bob along Y-axis
            pos.Y += (float)Math.Sin(time) * bobSpeed * DeltaTime;
            time += DeltaTime;

            // Update ECS transform
            transform.Position = pos;
        }

        public override void OnDestroy()
        {
        }
    }
}