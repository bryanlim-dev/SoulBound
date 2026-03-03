/******************************************************************************
 * File:        HoverScript.cs
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying - (Primary, 100%)
 *
 * Description:
 * This file declares class Spike and its logic
 * This script checks if the player is colliding with this objects and kills the player if so
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

// ============================================================
// Spike.cs - Kills player on contact
// ============================================================
namespace ManagedScripts
{
    public class Spike : Script
    {
        Collider col;

        public override void OnLoad()
        {
            col = new Collider(EntityId);
        }

        public override void OnUpdate()
        {
            if (col.CheckPlayerCollision())
            {
                KillPlayer();
            }
        }
    }
}