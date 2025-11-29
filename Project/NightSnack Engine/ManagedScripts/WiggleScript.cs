// ============================================================
// WiggleScript.cs - Causes object to wiggle intervally
// ============================================================
using System;
using System.Security.AccessControl;

namespace ManagedScripts
{
    public class WiggleScript : Script
    {
        [SerializeField]
        public float wiggleAmount = 4.0f;   // Amount of wiggle
        [SerializeField]
        public float interval = 2f;         // Time interval between wiggles
        [SerializeField]
        public float duration = 0.15f;      // Time interval between wiggles
        [SerializeField]
        public float time = 0.0f;

        public Transform transform;

        private Vector3 originalPos;
        private Random rseed;
        private float min, max;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            originalPos = transform.Position;
            rseed = new Random();

            min = -wiggleAmount;
            max = wiggleAmount;
        }

        public override void OnUpdate()
        {
            // Get current position
            Vector3 pos = transform.Position;

            // Only Wiggle every 2 seconds
            if (time > interval) {
                // Stop after Wiggling for 0.3 seconds
                if (time > (interval + duration))
                {
                    pos = originalPos;
                    time = 0.0f;
                }
                else
                {
                    // Wiggle
                    float Xval = (float)(rseed.NextDouble() * (max - min) + min);
                    float Yval = (float)(rseed.NextDouble() * (max - min) + min);

                    // Apply wiggle offset
                    pos = originalPos + new Vector3(Xval, Yval, 0f);
                }
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