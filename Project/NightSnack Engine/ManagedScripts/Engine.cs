using System;
using System.Drawing;
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
        private static extern void GetUIBasePos_Internal(uint entityId, out Vector3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetUIBasePos_Internal(uint entityId, ref Vector3 position);

        // ----------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetUIBaseScale_Internal(uint entityId, out Vector2 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetUIBaseScale_Internal(uint entityId, ref Vector2 scale);

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
        
        //GravityMult
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetGravityMult_Internal(uint entityId, out float gravMult);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetGravityMult_Internal(uint entityId, ref float gravMult);

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

        // ----------------------------
        // Color Internal for Scripts
        // ----------------------------
        #region Color
        //Required function, r,g,b,a, rgb (vec3), rgba (vec4)
        //Wait no i just need rgba and then split from there

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetRGBA_Internal(uint entityId, out Vector4 color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetRGBA_Internal(uint entityId, ref Vector4 color);

        #endregion

        // ----------------------------
        // Renderable Internal for Scripts
        // ----------------------------
        #region Renderable
        //meshName
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetMeshName_Internal(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMeshName_Internal(uint entityId, string mesh);

        //materialName
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetMaterialName_Internal(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetMaterialName_Internal(uint entityId, string mat);

        //isWireframe
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetIsWireframe_Internal(uint entityId, out bool wire);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetIsWireframe_Internal(uint entityId, ref bool wire);

        //wasModified is not here, it is just a flag

        #endregion

        // ----------------------------
        // Font Internal for Scripts
        // ----------------------------
        #region Font
        
        //fontType
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetFontType_Internal(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetFontType_Internal(uint entityId, string fontType);

        //text
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetFontText_Internal(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetFontText_Internal(uint entityId, string fontType);

        //fontSize
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetFontSize_Internal(uint entityId, out float size);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetFontSize_Internal(uint entityId, ref float size);

        //textColor
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetFontColor_Internal(uint entityId, out Vector3 col);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetFontColor_Internal(uint entityId, ref Vector3 col);

        //offsetPosition
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetFontOffsetPosition_Internal(uint entityId, out Vector3 pos);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetFontOffsetPosition_Internal(uint entityId, ref Vector3 pos);
        #endregion

        // ----------------------------
        // Animator Internal for Scripts
        // ----------------------------
        #region Animator
        //---- currentAnimatorClip
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetAnimatorCurrentClip_Internal(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorCurrentClip_Internal(uint entityId, string clipName);

        //---- spriteSheetName
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetAnimatorSpriteSheet_Internal(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorSpriteSheet_Internal(uint entityId, string sheet);

        //---- currentFrameIndex
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorCurrentFrameIndex_Internal(uint entityId, out int frame);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorCurrentFrameIndex_Internal(uint entityId, ref int frame);

        //---- currentTime
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorCurrentTime_Internal(uint entityId, out float time);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorCurrentTime_Internal(uint entityId, ref float time);
        
        //---- playbackSpeed
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorCurrentPlaybackSpeed_Internal(uint entityId, out float speed);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorCurrentPlaybackSpeed_Internal(uint entityId, ref float speed);

        //---- isPlaying
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorIsPlaying_Internal(uint entityId, out bool playing);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorIsPlaying_Internal(uint entityId, ref bool playing);

        //---- isLooping
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorIsLooping_Internal(uint entityId, out bool looping);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorIsLooping_Internal(uint entityId, ref bool looping);

        //---- lastFacingRight
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorLastFacingRight_Internal(uint entityId, out bool facingRight);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorLastFacingRight_Internal(uint entityId, ref bool facingRight);

        //---- uvTopLeft
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorUVTopLeft_Internal(uint entityId, out Vector2 uv);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorUVTopLeft_Internal(uint entityId, ref Vector2 uv);

        //---- uvBottomRight
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetAnimatorUVBottomRight_Internal(uint entityId, out Vector2 uv);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetAnimatorUVBottomRight_Internal(uint entityId, ref Vector2 uv);
        #endregion

        // ----------------------------
        // Camera Internal for Scripts
        // ----------------------------
        #region Camera
        //isMainCam
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetCameraIsMain_Internal(uint entityId, out bool main);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCameraIsMain_Internal(uint entityId, ref bool main);


        //zoom
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetCameraZoom_Internal(uint entityId, out float zoom);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCameraZoom_Internal(uint entityId, ref float zoom);

        //width
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetCameraWidth_Internal(uint entityId, out float width);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCameraWidth_Internal(uint entityId, ref float width);

        //height
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetCameraHeight_Internal(uint entityId, out float height);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetCameraHeight_Internal(uint entityId, ref float height);

        #endregion

        // ----------------------------
        // Scripting Internal for Scripts
        // ----------------------------
        #region Scripting
        //---- isLooping
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void HasScript_Internal(uint entityId, string scriptName, out bool exists);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void CreateScript_Internal(uint entityId, string scriptName);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void DeleteScript_Internal(uint entityId, string scriptName);
        #endregion

        // ----------------------------
        // Key/Buttons Internal for Scripts
        // ----------------------------
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint GetPlayerID_Internal();


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

        //UIbasePos
        public static Vector3 GetUIBasePos(uint entityId)
        {
            GetUIBasePos_Internal(entityId, out Vector3 pos);
            return pos;
        }
        public static void SetUIBasePos(uint entityId, Vector3 position)
        {
            SetUIBasePos_Internal(entityId, ref position);
        }

        //UIbaseScale
        public static Vector2 GetUIBaseScale(uint entityId)
        {
            GetUIBaseScale_Internal(entityId, out Vector2 scale);
            return scale;
        }
        public static void SetUIBaseScale(uint entityId, Vector2 scale)
        {
            SetUIBaseScale_Internal(entityId, ref scale);
        }

        //Layer Type
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

        //--GravityMult--
        public static float GetGravityMult(uint entityId)
        {
            GetGravityMult_Internal(entityId, out float gravMult);
            return gravMult;
        }
        public static void SetGravityMult(uint entityId, float gravMult)
        {
            SetGravityMult_Internal(entityId, ref gravMult);
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

        // ----------------------------
        // Color API for Scripts
        // ----------------------------
        #region Color
        public static Colors GetRGBA(uint entityId)
        {
            GetRGBA_Internal(entityId, out Vector4 vec4);
            return new Colors(vec4);
        }
        public static void SetRGBA(uint entityId, Colors col)
        {
            SetRGBA_Internal(entityId, ref col.vec4);
        }

        #endregion

        // ----------------------------
        // Renderable API for Scripts
        // ----------------------------
        #region Renderable
        public static string GetMeshName(uint entityId)
        {
            return GetMeshName_Internal(entityId);
        }
        public static void SetMeshName(uint entityId, string str)
        {
            SetMeshName_Internal(entityId, str);
        }

        public static string GetMaterialName(uint entityId)
        {
            return GetMaterialName_Internal(entityId);
        }
        public static void SetMaterialName(uint entityId, string str)
        {
            SetMaterialName_Internal(entityId, str);
        }

        public static bool GetIsWireframe(uint entityId)
        {
            GetIsWireframe_Internal(entityId, out bool wire);
            return wire;
        }
        public static void SetIsWireframe(uint entityId, bool wire)
        {
            SetIsWireframe_Internal(entityId, ref wire);
        }
        #endregion

        // ----------------------------
        // Animator API for Scripts
        // ----------------------------
        #region Animator
        //Not needed auto set in AnimationSystem
        //public static string GetAnimatorCurrentClip(uint entityId)
        //{
        //    return GetAnimatorCurrentClip_Internal(entityId);
        //}
        //public static void SetAnimatorCurrentClip(uint entityId, string clipName)
        //{
        //    SetAnimatorCurrentClip_Internal(entityId, clipName);
        //}


        //Not needed auto set in AnimationSystem
        //public static string GetAnimatorSpriteSheet(uint entityId)
        //{
        //    return GetAnimatorSpriteSheet_Internal(entityId);
        //}
        //public static void SetAnimatorSpriteSheet(uint entityId, string clipName)
        //{
        //    SetAnimatorSpriteSheet_Internal(entityId, clipName);
        //}


        public static int GetAnimatorCurrentFrameIndex(uint entityId)
        {
            GetAnimatorCurrentFrameIndex_Internal(entityId, out int frame);
            return frame;
        }
        public static void SetAnimatorCurrentFrameIndex(uint entityId, int frame)
        {
            SetAnimatorCurrentFrameIndex_Internal(entityId, ref frame);
        }


        public static float GetAnimatorCurrentTime(uint entityId)
        {
            GetAnimatorCurrentTime_Internal(entityId, out float time);
            return time;
        }
        public static void SetAnimatorCurrentTime(uint entityId, float time)
        {
            SetAnimatorCurrentTime_Internal(entityId, ref time);
        }


        public static float GetAnimatorCurrentPlaybackSpeed(uint entityId)
        {
            GetAnimatorCurrentPlaybackSpeed_Internal(entityId, out float speed);
            return speed;
        }
        public static void SetAnimatorCurrentPlaybackSpeed(uint entityId, float speed)
        {
            SetAnimatorCurrentPlaybackSpeed_Internal(entityId, ref speed);
        }

        public static bool GetAnimatorIsPlaying(uint entityId)
        {
            GetAnimatorIsPlaying_Internal(entityId, out bool playing);
            return playing;
        }
        public static void SetAnimatorIsPlaying(uint entityId, bool playing)
        {
            SetAnimatorIsPlaying_Internal(entityId, ref playing);
        }


        public static bool GetAnimatorIsLooping(uint entityId)
        {
            GetAnimatorIsLooping_Internal(entityId, out bool looping);
            return looping;
        }
        public static void SetAnimatorIsLooping(uint entityId, bool looping)
        {
            SetAnimatorIsLooping_Internal(entityId, ref looping);
        }


        public static bool GetAnimatorLastFacingRight(uint entityId)
        {
            GetAnimatorLastFacingRight_Internal(entityId, out bool facingRight);
            return facingRight;
        }
        public static void SetAnimatorLastFacingRight(uint entityId, bool facingRight)
        {
            SetAnimatorLastFacingRight_Internal(entityId, ref facingRight);
        }

        //Not needed auto set in AnimationSystem
        //public static Vector2 GetAnimatorUVTopLeft(uint entityId)
        //{
        //    GetAnimatorUVTopLeft_Internal(entityId, out Vector2 uv);
        //    return uv;
        //}
        //public static void SetAnimatorUVTopLeft(uint entityId, Vector2 uv)
        //{
        //    SetAnimatorUVTopLeft_Internal(entityId, ref uv);
        //}


        //Not needed auto set in AnimationSystem
        //public static Vector2 GetAnimatorUVBottomRight(uint entityId)
        //{
        //    GetAnimatorUVBottomRight_Internal(entityId, out Vector2 uv);
        //    return uv;
        //}
        //public static void SetAnimatorUVTBottomRight(uint entityId, Vector2 uv)
        //{
        //    SetAnimatorUVBottomRight_Internal(entityId, ref uv);
        //}
        #endregion

        // ----------------------------
        // Font API for Scripts
        // ----------------------------
        #region Font
        //fontType
        public static string GetFontType(uint entityId)
        {
            return GetFontType_Internal(entityId);
        }
        public static void SetFontType(uint entityId, string str)
        {
            SetFontType_Internal(entityId, str);
        }

        //text
        public static string GetFontText(uint entityId)
        {
            return GetFontText_Internal(entityId);
        }
        public static void SetFontText(uint entityId, string str)
        {
            SetFontText_Internal(entityId, str);
        }

        //text
        public static float GetFontSize(uint entityId)
        {
            GetFontSize_Internal(entityId, out float size);
            return size;
        }
        public static void SetFontSize(uint entityId, float size)
        {
            SetFontSize_Internal(entityId, ref size);
        }

        //textColor
        public static Vector3 GetFontColor(uint entityId)
        {
            GetFontColor_Internal(entityId, out Vector3 color);
            return color;
        }
        public static void SetFontColor(uint entityId, Vector3 color)
        {
            SetFontColor_Internal(entityId, ref color);
        }

        //offsetPosition
        public static Vector3 GetFontOffsetPosition(uint entityId)
        {
            GetFontOffsetPosition_Internal(entityId, out Vector3 pos);
            return pos;
        }
        public static void SetFontOffsetPosition(uint entityId, Vector3 position)
        {
            SetFontOffsetPosition_Internal(entityId, ref position);
        }

        #endregion

        // ----------------------------
        // Camera API for Scripts
        // ----------------------------
        #region Camera
        //isMainCam
        public static bool GetCameraIsMain(uint entityId)
        {
            GetCameraIsMain_Internal(entityId, out bool main);
            return main;
        }
        public static void SetCameraIsMain(uint entityId, bool main)
        {
            SetCameraIsMain_Internal(entityId, ref main);
        }

        //zoom
        public static float GetCameraZoom(uint entityId)
        {
            GetCameraZoom_Internal(entityId, out float zoom);
            return zoom;
        }
        public static void SetCameraZoom(uint entityId, float zoom)
        {
            SetCameraZoom_Internal(entityId, ref zoom);
        }

        //width
        public static float GetCameraWidth(uint entityId)
        {
            GetCameraWidth_Internal(entityId, out float width);
            return width;
        }
        public static void SetCameraWidth(uint entityId, float width)
        {
            SetCameraWidth_Internal(entityId, ref width);
        }

        //height
        public static float GetCameraHeight(uint entityId)
        {
            GetCameraHeight_Internal(entityId, out float height);
            return height;
        }
        public static void SetCameraHeight(uint entityId, float height)
        {
            SetCameraHeight_Internal(entityId, ref height);
        }
        #endregion

        // ----------------------------
        // Scripting API for Scripts
        // ----------------------------
        #region Scripting
        public static bool HasScript(uint entityId, string str)
        {
            HasScript_Internal(entityId, str, out bool exists);
            return exists;
        }
        public static void CreateScript(uint entityId, string str)
        {
            CreateScript_Internal(entityId, str);

        }
        public static void DeleteScript(uint entityId, string str)
        {
            DeleteScript_Internal(entityId, str);

        }
        #endregion

        // ----------------------------
        // Key/Buttons API for Scripts
        // ----------------------------
        #region Key/Buttons
        public static bool IsKeyPressed(int keyCode) => IsKeyPressed_Internal(keyCode);
        public static bool IsKeyDown(int keyCode) => IsKeyDown_Internal(keyCode);
        public static bool IsKeyReleased(int keyCode) => IsKeyReleased_Internal(keyCode);
        #endregion

        public static void PlaySound(string path) => PlaySound_Internal(path);

        public static uint CreateEntityArchetype(string archetypeName, Vector3 pos) => CreateEntityArchetype_Internal(archetypeName, pos);

        public static uint GetPlayerID()
        {
            return GetPlayerID_Internal();
        }


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

    public struct Vector4
    {
        public float X, Y, Z, W;
        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }
        public static Vector4 operator +(Vector4 a, Vector4 b) => new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
        public static Vector4 operator -(Vector4 a, Vector4 b) => new Vector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W - b.W);
        public static Vector4 operator *(Vector4 a, float scalar) => new Vector4(a.X * scalar, a.Y * scalar, a.Z * scalar, a.W * scalar);
        public static Vector4 operator /(Vector4 a, float denominator)
        {
            if (denominator == 0) // Make sure no divide by 0
                return new Vector4(a.X, a.Y, a.Z, a.W);
            else
                return new Vector4(a.X / denominator, a.Y / denominator, a.Z / denominator, a.W / denominator);
        }

        public override string ToString() => $"({X}, {Y}, {Z}, {W})";
    }

    //Wrapper struct for color specifically
    public struct Colors
    {
        public Vector4 vec4;

        public Colors(Vector4 vec)
        {
            this.vec4 = vec;
        }

        public Colors(Colors rhs)
        {
            this.vec4 = rhs.vec4;
        }

        public Colors(float r, float g, float b, float a)
        {
            this.vec4 = new Vector4(r, g, b, a);
        }

        public float R
        {
            get => vec4.X;
            set => vec4.X = value;
        }
        public float G
        {
            get => vec4.Y;
            set => vec4.Y = value;
        }
        public float B
        {
            get => vec4.Z;
            set => vec4.Z = value;
        }
        public float A
        {
            get => vec4.W;
            set => vec4.W = value;
        }

        public static Colors operator +(Colors a, Colors b) => new Colors(a.vec4 + b.vec4);
        public static Colors operator -(Colors a, Colors b) => new Colors(a.vec4 - b.vec4);
        public static Colors operator *(Colors a, float scalar) => new Colors(a.vec4 * scalar);
        public static Colors operator /(Colors a, float denominator) => new Colors(a.vec4 / denominator);
        public override string ToString() => $"({vec4.X}, {vec4.Y}, {vec4.Z}, {vec4.W})";

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
