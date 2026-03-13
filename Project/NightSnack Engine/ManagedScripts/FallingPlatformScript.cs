/******************************************************************************
 * File:        FallingPlatformScript.cs
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim - (Primary, 100%) - Added falling platform logic
 *
 * Description:
 * This script makes a platform fall after the player stands on it for a short duration.
 * 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
using System;

namespace ManagedScripts
{
    public class FallingPlatformScript : Script
    {
        [SerializeField]
        public float respawnDelay = 3f;

        [SerializeField]
        public float fallGravityMultiplier = 2000f;

        public enum State
        {
            Idle,
            Wiggle1,
            Wiggle2,
            Wiggle3,
            Falling,
            Respawning
        }

        // Platform state variables
        public State currentState = State.Idle;
        private float stateTimer = 0f;
        private Vector3 initialPosition;
        private float fallVelocity = 0f;

        // Component references
        private Transform transform;
        private RigidBody rigidBody;
        private Collider collider;
        private WiggleScript wiggleScript;

        public override void OnLoad()
        {
            // Component initialization
            transform = new Transform(EntityId);
            rigidBody = new RigidBody(EntityId);
            collider = new Collider(EntityId);
            wiggleScript = GetScriptComponentInChildren<WiggleScript>(EntityId, "WiggleScript");
        }

        public override void OnStart()
        {
            currentState = State.Idle;
            initialPosition = transform.Position;
            ResetPlatform();
        }

        public override void OnUpdate()
        {
            // Current state of the platform
            switch (currentState)
            {
                case State.Idle:
                    if (IsPlayerOnPlatform())
                    {
                        currentState = State.Wiggle1;
                        stateTimer = 0f;
                        PlayWiggle(1);
                    }
                    break;

                case State.Wiggle1:
                    stateTimer += DeltaTime;
                    if (stateTimer >= 1f)
                    {
                        currentState = State.Wiggle2;
                        stateTimer = 0f;
                        PlayWiggle(2);
                    }
                    break;

                case State.Wiggle2:
                    stateTimer += DeltaTime;
                    if (stateTimer >= 1f)
                    {
                        currentState = State.Wiggle3;
                        stateTimer = 0f;
                        PlayWiggle(3);
                    }
                    break;

                case State.Wiggle3:
                    stateTimer += DeltaTime;
                    if (stateTimer >= 1f)
                    {
                        currentState = State.Falling;
                        stateTimer = 0f;
                        StartFalling();
                    }
                    break;

                case State.Falling:
                    stateTimer += DeltaTime;
                    UpdateFall();
                    if (stateTimer >= respawnDelay)
                    {
                        currentState = State.Respawning;
                        stateTimer = 0f;
                    }
                    break;

                case State.Respawning:
                    ResetPlatform();
                    break;
            }
        }

        private void PlayWiggle(int times)
        {
            // Trigger wiggle effect in the WiggleScript
            if (wiggleScript != null)
            {
                wiggleScript.TriggerWiggle(times);
            }
        }

        private void StartFalling()
        {
            // Disable player collision and start falling
            fallVelocity = 0f;
            if (collider != null)
            {
                collider.ColType = ColliderType.Trigger;
            }
        }

        private void UpdateFall()
        {
            // Apply gravity to the platform's fall velocity and update its position
            fallVelocity += fallGravityMultiplier * DeltaTime;
            Vector3 pos = transform.Position;
            pos.Y -= fallVelocity * DeltaTime;
            transform.Position = pos;
        }

        private void ResetPlatform()
        {
            // Reset platform to initial state and position
            transform.Position = initialPosition;

            if (collider != null)
            {
                collider.ColType = ColliderType.Solid;
            }

            fallVelocity = 0f;
            currentState = State.Idle;
            stateTimer = 0f;
        }

        private bool IsPlayerOnPlatform()
        {
            // Check if the player is currently colliding with the platform
            return collider != null && collider.CheckPlayerCollision();
        }
    }
}
