/******************************************************************************
 * File:        HoverScript.cs
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan - (Primary, 95%) - Created base file and logic
                Clement Ang Yang Ying - (Secondary, 5%) - Added debug and testing logic
 *
 * Description:
 * This file declares class RotatorScript which rotates object continuously.
 * Has some commented out debug fuctions for refernce and testing
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

// ============================================================
// RotatorScript.cs - Rotates object continuously
// ============================================================
namespace ManagedScripts
{
    public class RotatorScript : Script
    {
        [SerializeField]
        public  float rotationSpeed = 50.0f; // degrees per second
        [SerializeField]
        public bool testBool = false;
        [SerializeField]
        public string testStr = "EasyPeasy";
        [SerializeField]
        public Vector2 testVec2 = new Vector2(1,5);
        [SerializeField]
        public Vector3 testVec3 = new Vector3(2, 4, 6);

        public Transform transform;
        public RigidBody rigidBody;
        public Collider collider;
        public Color color;
        public Renderer renderer;

        public override void OnLoad()
        {
            //Class tests
            transform = new Transform(EntityId);
            rigidBody = new RigidBody(EntityId);
            collider = new Collider(EntityId);
            color = new Color(EntityId);
            renderer = new Renderer(EntityId);
        }

        public override void OnStart()
        {


            //rigidBody.UseGravity = true;
            collider.ColType = ColliderType.Solid;
            collider.ColStatic = false;
            color.RGBA = new Colors(0.5f, 0, 0, 1);
            //renderer.IsWireframe = true;


            //System.Console.WriteLine("Rotator: OnStart"); 

            //Log("AHHH");
            //Log($"Entity Script ID: {EntityId}");
            //Log($"Has Script: {Engine.HasScript(EntityId, "RotatorScript")}");


            Vector3 pos;
            pos.X = 100;
            pos.Y = 100;
            pos.Z = 0;
            uint id1 = Engine.CreateEntityArchetype("BlueBallScripting", pos);
            //pos.X = 200;
            //uint id2 = Engine.CreateEntityArchetype("BlueBallScripting2", pos);
            //pos.X = 300;
            //pos.Y = -100;
            //uint id3 = Engine.CreateEntityArchetype("BlueBallScripting3", pos);

            //Audio Test
            //Engine.SetAudioPlayOnStart(id1, true);
            //Engine.SetAudioIsPlaying(id1, true);
            //Engine.SetAudioLoop(id1, true);
            //Engine.SetAudioKey(id1, "playerHit");


            ////Transform Test

            ////Engine.SetLayer(id1, LayerType.GAMEUI);
            ////Engine.SetLayer(id2, LayerType.BACKGROUND);
            //Engine.SetUIBasePos(id1, new Vector3(1, 1, 0));
            //Engine.SetUIBaseScale(id1, new Vector2(1, 1));
            //Log($"Ball 1 UI Pos: {Engine.GetUIBasePos(id1)}");
            //Log($"Ball 1 UI Scale: {Engine.GetUIBaseScale(id1)}");

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
            //Engine.SetMaterialName(id2, "slime_2_2.png");
            //Log($"Ball 2 Material Name: {Engine.GetMaterialName(id2)}");

            //Engine.SetIsWireframe(id1, true);

            ////Animator Test
            ////Engine.SetAnimatorCurrentClip(id2, Engine.GetMaterialName(id2)); //Not needed
            //Engine.SetAnimatorIsPlaying(id2, true);

            ////Font test
            //Engine.SetFontType(id3, "liberation-mono");
            //Log($"Ball 3 Font Name: {Engine.GetFontType(id3)}");
            //Engine.SetFontSize(id3, 3);
            //Engine.SetFontColor(id3, new Vector3(0, 1, 0));

            //PlaySound("Sound/SFX/button_click.mp3");

            //Log("RotatorScript started!");
            //Log($"Initial rotation: {Rotation}");
            //Log($"Rotation speed: {rotationSpeed} degrees/sec");
        }

        public override void OnUpdate()
        {


            // Get current rotation
            float rotation = transform.Rotation;

            // Rotate around Y axis
            rotation += rotationSpeed * DeltaTime;

            // Wrap around 360 degrees
            if (rotation >= 360.0f)
            {
                rotation -= 360.0f;
            }

            // Set back to ECS
            transform.Rotation = rotation;

            //// Log every 45 degrees
            //if ((int)rotation % 45 == 0 && rotation > 0)
            //{
            //    Log($"Current rotation: {rotation:F1} degrees");
            //}

            //if (Engine.IsKeyDown(KeyboardKeys.W))
            //{
            //    Engine.Log("Trying to restart level");
            //    Engine.RestartLevel(); 
            //}

            //if(Engine.IsKeyDown(KeyboardKeys.D))
            //{
            //    Engine.Log("Trying to load main menu");
            //    Engine.LoadLevel("MainMenu");
            //}
        }

        public override void OnDestroy()
        {
            //Log($"RotatorScript destroyed at rotation: {Rotation} degrees");
        }
    }
}
