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

        // ----------------------------
        // Transform Internal for Scripts
        // ----------------------------
        #region Transform
        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetPosition_Internal(uint entityId, out Vector3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetPosition_Internal(uint entityId, ref Vector3 position);

        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetRotation_Internal(uint entityId, out float rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetRotation_Internal(uint entityId, ref float rotation);

        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetScale_Internal(uint entityId, out Vector2 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetScale_Internal(uint entityId, ref Vector2 scale);

        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetLayer_Internal(uint entityId, out LayerType layer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetLayer_Internal(uint entityId, ref LayerType layer);
        #endregion

        // ----------------------------
        // Rigidbody Internal for Scripts
        // ----------------------------
        #region Rigidbody
        #region Force
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetForceDirection_Internal(uint entityId, out Vector2 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetForceDirection_Internal(uint entityId, ref Vector2 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetForceMagnitude_Internal(uint entityId, out float position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetForceMagnitude_Internal(uint entityId, ref float position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetForceLifetime_Internal(uint entityId, out float position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetForceLifetime_Internal(uint entityId, ref float position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetForceActive_Internal(uint entityId, out bool position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetForceActive_Internal(uint entityId, ref bool position);
        #endregion //Force


        //Velocity
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetVelocity_Internal(uint entityId, out Vector2 vel);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetVelocity_Internal(uint entityId, ref Vector2 vel);

        //Acceleration
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAcceleration_Internal(uint entityId, out Vector2 acc);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAcceleration_Internal(uint entityId, ref Vector2 acc);

        //InvMass
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetInvMass_Internal(uint entityId, out float invMass);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetInvMass_Internal(uint entityId, ref float invMass);

        //InvInertia
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetInvInertia_Internal(uint entityId, out float invInertia);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetInvInertia_Internal(uint entityId, ref float invInertia);

        //Restitution
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetRestitution_Internal(uint entityId, out float restitution);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetRestitution_Internal(uint entityId, ref float restitution);

        //UseGravity
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetUseGravity_Internal(uint entityId, out bool gravity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetUseGravity_Internal(uint entityId, ref bool gravity);

        //UseRotate
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetUseRotate_Internal(uint entityId, out bool rotate);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetUseRotate_Internal(uint entityId, ref bool rotate);


        #endregion //Rigibody

        // ----------------------------
        // Collider Internal for Scripts
        // ----------------------------
        #region Collider

        //Shape Type
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderShape_Internal(uint entityId, out ShapeType shape);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderShape_Internal(uint entityId, ref ShapeType shape);

        //ColliderType
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderType_Internal(uint entityId, out ColliderType colliderType);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderType_Internal(uint entityId, ref ColliderType colliderType);

        //isStatic
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderStatic_Internal(uint entityId, out bool colStatic);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderStatic_Internal(uint entityId, ref bool colStatic);

        //Height
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderHeight_Internal(uint entityId, out float height);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderHeight_Internal(uint entityId, ref float height);

        //Width
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderWidth_Internal(uint entityId, out float width);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderWidth_Internal(uint entityId, ref float width);

        //Radius
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderRadius_Internal(uint entityId, out float radius);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderRadius_Internal(uint entityId, ref float radius);

        //isGrounded
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderGrounded_Internal(uint entityId, out bool grounded);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderGrounded_Internal(uint entityId, ref bool grounded);

        //isGrounded
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetColliderColliding_Internal(uint entityId, out bool colliding);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetColliderColliding_Internal(uint entityId, ref bool colliding);
        #endregion

        #region Key/Buttons
        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyPressed_Internal(int keyCode);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyDown_Internal(int keyCode);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyReleased_Internal(int keyCode);
        #endregion

        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void PlaySound_Internal(string soundPath);

        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetDeltaTime_Internal();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint CreateEntityArchetype_Internal(string archetypeName, Vector3 pos);

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //private static extern float GetTime_Internal();



        // ============================================================
        // Public API for Scripts
        // ============================================================
        public static void Log(string message) => Log_Internal(message);

        // ----------------------------
        // Transform API for Scripts
        // ----------------------------
        #region Transform
        //Position
        public static Vector3 GetPosition(uint entityId)
        {
            GetPosition_Internal(entityId, out Vector3 pos);
            return pos;
        }
        public static void SetPosition(uint entityId, Vector3 position)
        {
            SetPosition_Internal(entityId, ref position);
        }

        //Rotation
        public static float GetRotation(uint entityId)
        {
            GetRotation_Internal(entityId, out float rot);
            return rot;
        }
        public static void SetRotation(uint entityId, float rotation)
        {
            SetRotation_Internal(entityId, ref rotation);
        }

        //Scale
        public static Vector2 GetScale(uint entityId)
        {
            GetScale_Internal(entityId, out Vector2 scale);
            return scale;
        }
        public static void SetScale(uint entityId, Vector2 scale)
        {
            SetScale_Internal(entityId, ref scale);
        }

        //Layer Typw
        public static LayerType GetLayer(uint entityId)
        {
            GetLayer_Internal(entityId, out LayerType layer);
            return layer;
        }
        public static void SetLayer(uint entityId, LayerType layer)
        {
            SetLayer_Internal(entityId, ref layer);
        }
        #endregion

        // ----------------------------
        // Rigidbody API for Scripts
        // ----------------------------
        #region Rigidbody
        #region Force
        public static Vector2 GetForceDirection(uint entityId)
        {
            GetForceDirection_Internal(entityId, out Vector2 dir);
            return dir;
        }
        public static void SetForceDirection(uint entityId, Vector2 dir)
        {
            SetForceDirection_Internal(entityId, ref dir);
        }

        public static float GetForceMagnitude(uint entityId)
        {
            GetForceMagnitude_Internal(entityId, out float mag);
            return mag;
        }
        public static void SetForceMagnitude(uint entityId, float mag)
        {
            SetForceMagnitude_Internal(entityId, ref mag);
        }

        public static float GetForceLifetime(uint entityId)
        {
            GetForceLifetime_Internal(entityId, out float life);
            return life;
        }
        public static void SetForceLifetime(uint entityId, float life)
        {
            SetForceLifetime_Internal(entityId, ref life);
        }

        public static bool GetForceActive(uint entityId)
        {
            GetForceActive_Internal(entityId, out bool active);
            return active;
        }
        public static void SetForceActive(uint entityId, bool active)
        {
            SetForceActive_Internal(entityId, ref active);
        }
        #endregion //Force

        //--Velocity--
        public static Vector2 GetVelocity(uint entityId)
        {
            GetVelocity_Internal(entityId, out Vector2 vel);
            return vel;
        }
        public static void SetVelocity(uint entityId, Vector2 vel)
        {
            SetVelocity_Internal(entityId, ref vel);
        }

        //--Acceleration--
        public static Vector2 GetAcceleration(uint entityId)
        {
            GetAcceleration_Internal(entityId, out Vector2 acc);
            return acc;
        }
        public static void SetAcceleration(uint entityId, Vector2 acc)
        {
            SetAcceleration_Internal(entityId, ref acc);
        }

        //--InvMass--
        public static float GetInvMass(uint entityId)
        {
            GetInvMass_Internal(entityId, out float mass);
            return mass;
        }
        public static void SetInvMass(uint entityId, float mass)
        {
            SetInvMass_Internal(entityId, ref mass);
        }

        //--InvInertia--
        public static float GetInvInertia(uint entityId)
        {
            GetInvInertia_Internal(entityId, out float inertia);
            return inertia;
        }
        public static void SetInvInertia(uint entityId, float inertia)
        {
            SetInvInertia_Internal(entityId, ref inertia);
        }

        //--Restitution--
        public static float GetRestitution(uint entityId)
        {
            GetRestitution_Internal(entityId, out float restitution);
            return restitution;
        }
        public static void SetRestitution(uint entityId, float restitution)
        {
            SetRestitution_Internal(entityId, ref restitution);
        }

        //--UseGravity--
        public static bool GetUseGravity(uint entityId)
        {
            GetUseGravity_Internal(entityId, out bool gravity);
            return gravity;
        }
        public static void SetUseGravity(uint entityId, bool gravity)
        {
            SetUseGravity_Internal(entityId, ref gravity);
        }

        //--UseRotate--
        public static bool GetUseRotate(uint entityId)
        {
            GetUseRotate_Internal(entityId, out bool rotate);
            return rotate;
        }
        public static void SetUseRotate(uint entityId, bool rotate)
        {
            SetUseRotate_Internal(entityId, ref rotate);
        }
        #endregion //Rigibody

        // ----------------------------
        // Collider API for Scripts
        // ----------------------------
        #region Collider

        //---Shape Type---
        public static ShapeType GetColliderShapeType(uint entityId)
        {
            GetColliderShape_Internal(entityId, out ShapeType shape);
            return shape;
        }
        public static void SeColliderShapeType(uint entityId, ShapeType shape)
        {
            SetColliderShape_Internal(entityId, ref shape);
        }

        //---Collider Type---
        public static ColliderType GetColliderType(uint entityId)
        {
            GetColliderType_Internal(entityId, out ColliderType collider);
            return collider;
        }
        public static void SetColliderType(uint entityId, ColliderType collider)
        {
            SetColliderType_Internal(entityId, ref collider);
        }

        //---isStatic---
        public static bool GetColliderStatic(uint entityId)
        {
            GetColliderStatic_Internal(entityId, out bool colStatic);
            return colStatic;
        }
        public static void SetColliderStatic(uint entityId, bool colStatic)
        {
            SetColliderStatic_Internal(entityId, ref colStatic);
        }

        //--Height--
        public static float GetColliderHeight(uint entityId)
        {
            GetColliderHeight_Internal(entityId, out float height);
            return height;
        }
        public static void SetColliderHeight(uint entityId, float height)
        {
            SetColliderHeight_Internal(entityId, ref height);
        }

        //--Width--
        public static float GetColliderWidth(uint entityId)
        {
            GetColliderWidth_Internal(entityId, out float width);
            return width;
        }
        public static void SetColliderWidth(uint entityId, float width)
        {
            SetColliderWidth_Internal(entityId, ref width);
        }

        //--Radius--
        public static float GetColliderRadius(uint entityId)
        {
            GetColliderRadius_Internal(entityId, out float radius);
            return radius;
        }
        public static void SetColliderRadius(uint entityId, float radius)
        {
            SetColliderRadius_Internal(entityId, ref radius);
        }

        //---isGrounded---
        public static bool GetColliderGrounded(uint entityId)
        {
            GetColliderGrounded_Internal(entityId, out bool grounded);
            return grounded;
        }
        public static void SetColliderGrounded(uint entityId, bool grounded)
        {
            SetColliderGrounded_Internal(entityId, ref grounded);
        }

        //---colliding---
        public static bool GetColliderColliding(uint entityId)
        {
            GetColliderColliding_Internal(entityId, out bool colliding);
            return colliding;
        }
        public static void SetColliderColliding(uint entityId, bool colliding)
        {
            SetColliderColliding_Internal(entityId, ref colliding);
        }


        #endregion

        #region Key/Buttons
        public static bool IsKeyPressed(int keyCode) => IsKeyPressed_Internal(keyCode);
        public static bool IsKeyDown(int keyCode) => IsKeyDown_Internal(keyCode);
        public static bool IsKeyReleased(int keyCode) => IsKeyReleased_Internal(keyCode);
        #endregion

        public static void PlaySound(string path) => PlaySound_Internal(path);

        public static uint CreateEntityArchetype(string archetypeName, Vector3 pos) => CreateEntityArchetype_Internal(archetypeName, pos);


        public static float DeltaTime => GetDeltaTime_Internal();
        //public static float Time => GetTime_Internal();
    }

    public struct Vector2
    {
        public float X, Y;
        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }
        public static Vector2 operator +(Vector2 a, Vector2 b) => new Vector2(a.X + b.X, a.Y + b.Y);
        public static Vector2 operator -(Vector2 a, Vector2 b) => new Vector2(a.X - b.X, a.Y - b.Y);
        public static Vector2 operator *(Vector2 a, float scalar) => new Vector2(a.X * scalar, a.Y * scalar);

        public override string ToString() => $"({X}, {Y})";
    }

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

    //When updating this update this Component.h
    public enum LayerType : sbyte
    {
        _ENGINE,
        BACKGROUND,
        GAMELAYER,
        FOREGROUND,
        GAMEUI,
        _ENGINE2
    };

    //When updating this update this Component.h
    public enum ShapeType : sbyte { AABB, Circle };
    public enum ColliderType : sbyte
    {
        Solid,   // Blocks movement
        Trigger,  // Detects overlaps only
        Ground
    };
}
