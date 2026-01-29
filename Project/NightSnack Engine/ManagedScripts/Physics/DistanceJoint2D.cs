/******************************************************************************
 * File:        DistanceJoint2D.cs
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying - (Primary, 100%)
 * 
 * Description:
 * This file recreates the distance joint 2d from unity
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

using System;
using System.Diagnostics;
using static ManagedScripts.Script;

namespace ManagedScripts
{
    public class DistanceJoint2D : Script
    {
        //Is script enabled
        bool isEnabled = false;


        public RigidBody bodyA;
        public RigidBody bodyB;

        [SerializeField]
        public bool autoConfigureDistance = true;
        [SerializeField]
        public float minDistance = 0f;
        [SerializeField]
        public float maxDistance = 0f;

        #region Spring
        [SerializeField]
        public bool enableSpring = false;
        public float frequencyHz = 5.0f;
        public float dampingRatio = 0.7f;
        #endregion 

        public float MaxForce = float.PositiveInfinity;

        public override void OnLoad()
        {
            bodyA = GetComponentInParent<RigidBody>(EntityId);
        }

        public override void OnStart()
        {

        }

        public override void OnUpdate()
        {
            if (isEnabled == true && bodyA != null && bodyB != null)
            {
                Vector3 posAVec3 = new Transform(bodyA.EntityId).Position;
                Vector2 posAVec2 = new Vector2(posAVec3.X, posAVec3.Y);

                Vector3 posBVec3 = new Transform(bodyB.EntityId).Position;
                Vector2 posBVec2 = new Vector2(posBVec3.X, posBVec3.Y);
                Solve(posAVec2, posBVec2, DeltaTime);
            }
        }

        public void Solve(Vector2 positionA, Vector2 positionB, float deltaTime)
        {
            // Vector from A to B
            Vector2 delta = positionB - positionA;

            float distance = delta.Length();
            if (distance < 0.00001f)
                return;

            Vector2 normal = delta * (1.0f / distance);

            // Clamp target distance
            float targetDistance = Engine.Clamp(distance, minDistance, maxDistance);
            float error = distance - targetDistance;

            // Relative velocity along constraint axis
            Vector2 relVel = bodyB.Velocity - bodyA.Velocity;
            float relVelAlongNormal = Vector2.Dot(relVel, normal);

            float forceMagnitude;

            if (enableSpring)
            {
                // Unity-style spring model
                float omega = 2.0f * (float)Math.PI * frequencyHz;

                float springForce = -omega * omega * error;
                float dampingForce = -2.0f * dampingRatio * omega * relVelAlongNormal;

                forceMagnitude = springForce + dampingForce;
            }
            else
            {
                // Hard constraint (Baumgarte-style correction)
                forceMagnitude = -(error / deltaTime);
            }

            // Clamp force
            forceMagnitude = Engine.Clamp(forceMagnitude, -MaxForce, MaxForce);

            Vector2 force = normal * forceMagnitude;

            bodyA.ApplyForce(-force);
            bodyB.ApplyForce(force);
        }

        public void SetAnchor(uint newAnchorEntity)
        {
            bodyB = GetComponentInParent<RigidBody>(newAnchorEntity);
        }

        public void DisableScript()
        {
            isEnabled = false;
        }

        public void EnableScript()
        {
            isEnabled = true;
        }
    }
}