using System;

//To use attach [Expose] above any variables to show in inspector
//Only works for simple types (int, bool, float, string) and vec2/3
//e.g.
//[Expose]
//private const float rotationSpeed = 50.0f;
public sealed class ExposeAttribute : Attribute
{
    //Empty by design
    //Using _____Atribute allows ____ to be attached to variables
    //In this case _____ is [Exposed]
}