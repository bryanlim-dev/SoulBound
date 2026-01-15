/******************************************************************************
 * File:        HoverScript.cs
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan - (Primary, 95%) - Created base file and logic
                Clement Ang Yang Ying - (Secondary, 5%) - Added debug and testing logic
 * Description:
 * This file declares class HoverScripts which moves an object left/right continuously
 * Has some commented out debug fuctions for refernce and testing
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

// ============================================================
// RotatorScript.cs - Moves object left/right continuously
// ============================================================
namespace ManagedScripts
{
    public class HoverScript : Script
    {

        [SerializeField]
        public float testFloat = 0;
        [SerializeField]
        public float moveSpeed = 150.0f;
        [SerializeField]
        public float time = 0.0f;

        [SerializeField]
        RotatorScript rotate = null;

        public Transform transform;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            //Log($"Player Script ID: {GetPlayerID()}");
            //Engine.CreateScript(EntityId, "RotatorScript");
            rotate = Engine.GetScript<RotatorScript>(EntityId, "RotatorScript");

            //if (rotate != null){
            //    Log($"Script found!");
            //}
            //else
            //{
            //    Log($"Script not found!");
            //}

            //Log("HoverScript started!");
            //Log($"Initial Position: {Position.X}, {Position.Y}");
            //Log($"Move speed: {moveSpeed}.f");
            //moveSpeed = 1000;
            //Engine.Log("Inital Move Speed: " + moveSpeed);

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

            //if (rotate != null)
            //{
            //    Engine.Log($"RotatorScript exists");
            //}
            //else
            //{
            //    Engine.Log($"RotatorScript not exists");
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


            //if (Engine.IsKeyDown(KeyboardKeys.W))
            //{
            //    Engine.Log("Move Speed: " + moveSpeed);
            //}
        }

        public override void OnDestroy()
        {
        }
    }
}
