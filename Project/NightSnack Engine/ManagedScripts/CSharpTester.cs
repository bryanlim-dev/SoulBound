using System;

namespace ManagedScripts
{
    public class CSharpTester
    {
        public float PublicFloatVar = 5.0f;

        public void PrintFloatVar()
        {
            Console.WriteLine("PublicFloatVar = {0:F}", PublicFloatVar);
        }
        private void IncrementFloatVar(float value)
        {
            PublicFloatVar += value;
        }
    }
}
