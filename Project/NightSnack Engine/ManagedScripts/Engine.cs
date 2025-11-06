using System;
using System.Runtime.CompilerServices;

namespace ManagedScripts
{
    /// <summary>
    /// Bridge to native engine - all calls go through internal methods implemented in C++
    /// </summary>
    public static class Engine
    {
        // ============================================================
        // Internal Calls - Implemented in C++ (MonoInternalCalls.cpp)
        // ============================================================

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Log_Internal(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetPosition_Internal(uint entityId, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetPosition_Internal(uint entityId, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyPressed_Internal(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyDown_Internal(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyReleased_Internal(int keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void PlaySound_Internal(string soundPath);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetDeltaTime_Internal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetTime_Internal();

        // ============================================================
        // Public API for Scripts
        // ============================================================

        public static void Log(string message) => Log_Internal(message);

        public static Vector3 GetPosition(uint entityId)
        {
            GetPosition_Internal(entityId, out Vector3 pos);
            return pos;
        }

        public static void SetPosition(uint entityId, Vector3 position)
        {
            SetPosition_Internal(entityId, ref position);
        }

        public static bool IsKeyPressed(int keyCode) => IsKeyPressed_Internal(keyCode);
        public static bool IsKeyDown(int keyCode) => IsKeyDown_Internal(keyCode);
        public static bool IsKeyReleased(int keyCode) => IsKeyReleased_Internal(keyCode);

        public static void PlaySound(string path) => PlaySound_Internal(path);

        public static float DeltaTime => GetDeltaTime_Internal();
        public static float Time => GetTime_Internal();
    }

    /// <summary>
    /// Simple 3D vector - must match C++ Core::Scripting::Vector3 layout
    /// </summary>
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 operator +(Vector3 a, Vector3 b) => new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        public static Vector3 operator -(Vector3 a, Vector3 b) => new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        public static Vector3 operator *(Vector3 a, float scalar) => new Vector3(a.X * scalar, a.Y * scalar, a.Z * scalar);

        public override string ToString() => $"({X}, {Y}, {Z})";
    }
}
