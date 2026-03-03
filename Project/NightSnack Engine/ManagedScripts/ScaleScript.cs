// ============================================================
// ScaleScript.cs - Causes object to wiggle intervally
// ============================================================
using System;
using System.Security.AccessControl;

namespace ManagedScripts
{
    public class ScaleScript : Script
    {
        [SerializeField]
        public float scaleSpeed = 2f;
        [SerializeField]
        public float scaleAmount = 0.25f;
        [SerializeField]
        public float time = 0.0f;

        public Transform transform;

        private Vector2 originalScale;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            originalScale = transform.Scale;
        }

        public override void OnUpdate()
        {

            time += DeltaTime;

            // Get current position
            Vector3 pos = transform.Position;

            float scale = 1f + (float)Math.Sin(time * scaleSpeed) * scaleAmount;
            transform.Scale = originalScale * scale;
        }

        public override void OnDestroy()
        {
        }
    }
}