/******************************************************************************
 * File:        FallingPlatformScript.cs
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying - (Primary, 30%) - Created base file
 *              Wei Jie Lim - (70%) - Added falling platform logic
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
        public float respawnDelay = 2f;

        [SerializeField]
        public float fallGravityMultiplier = 2000f;

        private enum State
        {
            Idle,
            Wiggle1,
            Wiggle2,
            Wiggle3,
            Falling,
            Respawning
        }

        private State currentState = State.Idle;
        private float stateTimer = 0f;
        private Vector3 initialPosition;
        private float fallVelocity = 0f;
        private bool hasFallen = false;

        private Transform transform;
        private RigidBody rigidBody;
        private Collider collider;
        private WiggleScript wiggleScript;

        public override void OnLoad()
        {
            transform = new Transform(EntityId);
            rigidBody = new RigidBody(EntityId);
            collider = new Collider(EntityId);
            wiggleScript = GetScriptComponentInChildren<WiggleScript>(EntityId, "WiggleScript");
        }

        public override void OnStart()
        {
            initialPosition = transform.Position;
            ResetPlatform();
        }

        public override void OnUpdate()
        {
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
                    stateTimer += DeltaTime;
                    if (stateTimer >= respawnDelay)
                    {
                        ResetPlatform();
                    }
                    break;
            }
        }

        private void PlayWiggle(int times)
        {
            if (wiggleScript != null)
            {
                wiggleScript.TriggerWiggle(times);
            }
        }

        private void StartFalling()
        {
            hasFallen = true;
            fallVelocity = 0f;
        }

        private void UpdateFall()
        {
            fallVelocity += fallGravityMultiplier * DeltaTime;
            Vector3 pos = transform.Position;
            pos.Y -= fallVelocity * DeltaTime;
            transform.Position = pos;
        }

        private void ResetPlatform()
        {
            transform.Position = initialPosition;

            hasFallen = false;
            fallVelocity = 0f;
            currentState = State.Idle;
            stateTimer = 0f;
        }

        private bool IsPlayerOnPlatform()
        {
            return collider != null && collider.CheckPlayerCollision();
        }
    }
}
