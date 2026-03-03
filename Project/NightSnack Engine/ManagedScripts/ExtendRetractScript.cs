/******************************************************************************
 * File:        ExtendRetractScript.cs
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying - (Primary, 100%)
 *
 * Description:
 * This file declares class ExtendRetractScript which extends and retracts an object
 * It can be set to move up and down or left and right
 * The speed can be modified as well as the time it stays in each position
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
using System.Diagnostics;

namespace ManagedScripts
{
    public class ExtendRetractScript : Script
    {
        [SerializeField]
        public float moveSpeed = 50f;

        [SerializeField]
        //Amount of units to move
        public float maxMoveUnits = 45;


        //Units it has moved in this dir
        private float movedUnits = 0;

        [SerializeField]
        public bool isDirUpDown = true;

        [SerializeField]
        //If true the object is currently extended, currently moving down
        //If flase object is currently retracted, currently moving up
        public bool isExtended = true;


        //How long before it starts extending
        [SerializeField]
        public float maxStartExtendWaitTime = 5.0f;

        //How long before it starts retracting
        [SerializeField]
        public float maxStartRetractWaitTime = 5.0f;

        //Tracks how long the 
        [SerializeField]
        public float waitTimer = 0f;

        public Transform transform;

        public override void OnLoad()
        {
            transform = new Transform(EntityId);
        }

        public override void OnStart()
        {
            movedUnits = 0;
        }

        public override void OnUpdate()
        {
            bool allowedMovement = false;

            //Checks to see if the entity has waiting beforew moving again
            if (isExtended) //Currently extended
            {
                if (waitTimer >= maxStartExtendWaitTime)
                    allowedMovement = true;
            }
            else //Currently retracted
            {
                if (waitTimer >= maxStartRetractWaitTime)
                    allowedMovement = true;
            }

            if (allowedMovement)
            {
                MoveEntity();
                //Engine.Log("Can Move: " + waitTimer);
            }
            else
            {
                waitTimer += DeltaTime;
                //Engine.Log("Waiting Time: " + waitTimer);
            }
        }

        public override void OnDestroy()
        {

        }

        public void MoveEntity()
        {
            bool isMaxMoved = false;
            float unitsToMove = DeltaTime * moveSpeed;

            Vector3 currPos = transform.Position;

            //Within bounds
            if (movedUnits + unitsToMove >= maxMoveUnits)
            {
                unitsToMove = (movedUnits + unitsToMove) - maxMoveUnits;
                isMaxMoved = true;
            }

            //Always positive easier to track :D
            movedUnits += unitsToMove;


            if (isExtended) //Next state is down
            {
                unitsToMove = -unitsToMove;
            }

            if (isDirUpDown)
                currPos.Y += unitsToMove;
            else
                currPos.X += unitsToMove;

            transform.Position = currPos;
            //Engine.Log("Pos: " + transform.Position.X + ":" + transform.Position.Y);

            if (isMaxMoved)
            {
                isExtended = !isExtended;
                movedUnits = 0;
                waitTimer = 0;
            }

        }
    }
}