// ============================================================
// WiggleScript.cs - Causes object to wiggle intervally
// ============================================================
using System;

namespace ManagedScripts
{
    public class WiggleScript : Script
    {
        [SerializeField]
        public float wiggleDist = 5.0f;
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

            if (time > 2.0f) {

                // Stop Wiggle after 1 Second
                if (time > 3.0f) {
                    time = 0.0f;
                }
                // Wiggle
                pos.X += (float)Math.Sin(time * 5.0f) * wiggleDist;
                pos.Y += (float)Math.Cos(time * 10.0f) * wiggleDist;
            }
            time += DeltaTime;

            // Update ECS transform
            transform.Position = pos;
        }

        public override void OnDestroy()
        {
        }
    }
}