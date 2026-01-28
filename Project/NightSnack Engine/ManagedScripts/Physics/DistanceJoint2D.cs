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

using System.Diagnostics;
using static ManagedScripts.Script;

namespace ManagedScripts
{
    public class DistanceJoint2D : Script
    {
        //Is script enabled
        bool isEnabled = false;


        //The Rigidbody2D object to which the other end of the joint is attached (ie, the object without the joint component).
        RigidBody connectedBody = null;
        RigidBody rb;

        [SerializeField]
        Vector2 anchorPoint;
        [SerializeField]
        public Vector2 connectedAnchor;

        [SerializeField]
        public bool autoConfigureDistance = true;
        [SerializeField]
        public float minDistance = 0f;
        [SerializeField]
        public float maxDistance = 0f;

        #region Spring
        [SerializeField]
        public bool enableSpring = false;
        public float frequency = 5f;
        public float dampingRatio = 0.7f;
        #endregion 

        public override void OnLoad()
        {
            rb = GetComponentInParent<RigidBody>(EntityId);
        }

        public override void OnStart()
        {

        }

        public override void OnUpdate()
        {

        }

        public void SetAnchor(Vector2 newAnchor)
        {
            anchorPoint = newAnchor;
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