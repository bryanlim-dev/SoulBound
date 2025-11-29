/******************************************************************************
 * File:        SerializeFieldAttribute.cs
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying - (Primary, 100%) - Created class
 *
 * Description:
 * Class to expose fields to the c++ inspector
 * To use attach [SerializeField] above any PUBLIC variables to show in inspector
 * Only works for simple types (int, bool, float, string) and vec2/3
 * Does NOT work for static/const use readonly if you want something similar
 * e.g.
 * [SerializeField]
 * public const float rotationSpeed = 50.0f;
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

using System;
namespace ManagedScripts
{
    //To use attach [SerializeField] above any PUBLIC variables to show in inspector
    //Only works for simple types (int, bool, float, string) and vec2/3
    //Does NOT work for static/const use readonly if you want something similar
    //e.g.
    //[SerializeField]
    //public const float rotationSpeed = 50.0f;
    public sealed class SerializeFieldAttribute : Attribute
    {
        //Empty by design
        //Using _____Atribute allows ____ to be attached to variables
        //In this case _____ is [SerializeField]
    }
}