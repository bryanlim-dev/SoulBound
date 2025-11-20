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
            //Vector3 pos;
            //pos.X = 100;
            //pos.Y = 100;
            //pos.Z = 0;
            //uint id1 = Engine.CreateEntityArchetype("BlueBallScripting", pos);
            //pos.X = 200;
            //uint id2 = Engine.CreateEntityArchetype("BlueBallScripting2", pos);

            ////Transform Test

            ////Engine.SetLayer(id1, LayerType.GAMEUI);
            ////Engine.SetLayer(id2, LayerType.BACKGROUND);

            ////Rigibody Test
            //Vector2 dir;
            //dir.X = 1;
            //dir.Y = 1;
            //Engine.SetForceDirection(id2, dir);
            //Engine.SetForceMagnitude(id2, 100);
            //Engine.SetForceLifetime(id2, 0.5f);
            //Engine.SetForceActive(id2, true);

            //Vector2 vel;
            //vel.X = 150;
            //vel.Y = 0;
            //Engine.SetUseGravity(id1, true);
            //Engine.SetGravityMult(id1, 1);
            //Engine.SetVelocity(id1, vel);

            ////Collider Test
            //Engine.SetColliderWidth(id1, 100);
            //Engine.SetColliderHeight(id1, 100);

            //Engine.SetColliderType(id2, ColliderType.Ground);
            //Engine.SeColliderShapeType(id2, ShapeType.Circle);
            //Engine.SetColliderRadius(id2, 100);

            ////Color Test
            //Engine.SetRGBA(id2, new Colors(0, 1, 0, 0.25f));
            //Log($"Ball 1 color: {Engine.GetRGBA(id1)}");

            ////Renderable test
            //Engine.SetMeshName(id2, "square");
            //Log($"Ball 2 Mesh Name: {Engine.GetMeshName(id2)}");
            //Engine.SetMaterialName(id2, "../assets\\slime_2_2.png");
            //Log($"Ball 2 Material Name: {Engine.GetMaterialName(id2)}");

            //Engine.SetIsWireframe(id1, true);

            ////Animator Test
            //Engine.SetAnimatorCurrentClip(id2, Engine.GetMaterialName(id2));
            //Engine.SetAnimatorIsPlaying(id2, true);
            //Engine.SetAnimatorIsLooping(id2, true);

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
