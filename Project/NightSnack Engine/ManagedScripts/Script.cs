/******************************************************************************
 * File:        Scripts.cs
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan - (Primary, 30%) - Created base file and Transform/Input/Sound functions
                Clement Ang Yang Ying - (Secondary, 70%) - Added other component and entity functions and all Convenience classes
 *
 * Description:
 * This file declares class Scripts.cs with is the base class for all game scripts.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

using System;
using System.Diagnostics;
using System.Security.Cryptography.X509Certificates;
using static ManagedScripts.Script;

namespace ManagedScripts
{
    // Base class for all game scripts
    public abstract class Script
    {
        // Set by engine when script is created
        public uint EntityId;

        // Override these in your script classes

        //This runs when the script is made 
        //Use it to link components (i.e Transfrom, rotate, audio, etc)
        public virtual void OnLoad() { }
        
        //This runs right before the script's first update
        public virtual void OnStart() { }

        //Rhis runs every update, when not paused
        public virtual void OnUpdate() { }

        //Runs when script is destroyed
        public virtual void OnDestroy() { }

        //public static T GetComponentInChildren<T>(uint entityParentID) where T : class
        //{
        //    Engine.Log("No overloaded component found, create the overloaded function for it! (GetComponentInChildren<T>())!");
        //    return null;
        //}


        public class Component
        {
            public uint EntityId;
            public Component(uint id)
            {
                EntityId = id;
            }

            //Gets name of component
            //To overload in child classes add new, e.g.
            //public new static string GetName() { return "Transform"; }
            public static string GetName() { return "ComponentBase"; }
        }



        // Convenience properties
        #region Transform
        public class Transform : Component
        {

            public Transform(uint id) : base (id)
            {
            }

            public void SetID(uint id)
                { EntityId = id; }

            public Vector3 Position
            {
                get => Engine.GetPosition(EntityId);
                set => Engine.SetPosition(EntityId, value);
            }
            public float Rotation
            {
                get => Engine.GetRotation(EntityId);
                set => Engine.SetRotation(EntityId, value);
            }
            public Vector2 Scale
            {
                get => Engine.GetScale(EntityId);
                set => Engine.SetScale(EntityId, value);
            }
            public Vector3 UIBasePos
            {
                get => Engine.GetUIBasePos(EntityId);
                set => Engine.SetUIBasePos(EntityId, value);
            }

            public Vector2 UIBaseScale
            {
                get => Engine.GetUIBaseScale(EntityId);
                set => Engine.SetUIBaseScale(EntityId, value);
            }

            public LayerType Layer
            {
                get => Engine.GetLayer(EntityId);
                set => Engine.SetLayer(EntityId, value);
            }
            public new static string GetName() { return "Transform"; }

        }

        #endregion

        #region RigidBody
        public class RigidBody : Component
        {
            public RigidBody(uint id) : base(id)
            {
            }

            #region Force
            public Vector2 ForceDirection
            {
                get => Engine.GetForceDirection(EntityId);
                set => Engine.SetForceDirection(EntityId, value);
            }
            public float ForceMagnitude
            {
                get => Engine.GetForceMagnitude(EntityId);
                set => Engine.SetForceMagnitude(EntityId, value);
            }
            public float ForceLifetime
            {
                get => Engine.GetForceLifetime(EntityId);
                set => Engine.SetForceLifetime(EntityId, value);
            }
            public bool ForceActive
            {
                get => Engine.GetForceActive(EntityId);
                set => Engine.SetForceActive(EntityId, value);
            }
            #endregion //Force

            public Vector2 Velocity
            {
                get => Engine.GetVelocity(EntityId);
                set => Engine.SetVelocity(EntityId, value);
            }
            public Vector2 Accleration
            {
                get => Engine.GetAcceleration(EntityId);
                set => Engine.SetAcceleration(EntityId, value);
            }
            public float InvMass
            {
                get => Engine.GetInvMass(EntityId);
                set => Engine.SetInvMass(EntityId, value);
            }
            public float InvInertia
            {
                get => Engine.GetInvInertia(EntityId);
                set => Engine.SetInvInertia(EntityId, value);
            }
            public float Restitution
            {
                get => Engine.GetRestitution(EntityId);
                set => Engine.SetRestitution(EntityId, value);
            }
            public bool UseGravity
            {
                get => Engine.GetUseGravity(EntityId);
                set => Engine.SetUseGravity(EntityId, value);
            }
            public bool UseRotate
            {
                get => Engine.GetUseRotate(EntityId);
                set => Engine.SetUseRotate(EntityId, value);
            }

            public new static string GetName() { return "RigidBody"; }

            public void ApplyForce(Vector2 force, Vector2 r)
            {
                if (this.InvMass == 0f)
                    return;
                ForceDirection = force.Normalize();
                ForceMagnitude = force.Length();
                ForceActive = true;
            }

        }
        #endregion //Rigidbody

        #region Collider
        public class Collider : Component
        {
            public Collider(uint id) : base(id)
            {
            }

            public ColliderType ColType
            {
                get => Engine.GetColliderType(EntityId);
                set => Engine.SetColliderType(EntityId, value);
            }
            public ShapeType ColShape
            {
                get => Engine.GetColliderShapeType(EntityId);
                set => Engine.SeColliderShapeType(EntityId, value);
            }

            public bool ColStatic
            {
                get => Engine.GetColliderStatic(EntityId);
                set => Engine.SetColliderStatic(EntityId, value);
            }

            public float ColHeight
            {
                get => Engine.GetColliderHeight(EntityId);
                set => Engine.SetColliderHeight(EntityId, value);
            }

            public float ColWidth
            {
                get => Engine.GetColliderWidth(EntityId);
                set => Engine.SetColliderWidth(EntityId, value);
            }
            public float ColRadius
            {
                get => Engine.GetColliderRadius(EntityId);
                set => Engine.SetColliderRadius(EntityId, value);
            }

            public bool ColGrounded
            {
                get => Engine.GetColliderGrounded(EntityId);
                set => Engine.SetColliderGrounded(EntityId, value);
            }

            public bool ColColliding
            {
                get => Engine.GetColliderColliding(EntityId);
                set => Engine.SetColliderColliding(EntityId, value);
            }
            public new static string GetName() { return "Collider"; }

        }

        #endregion

        #region Color

        public class Color : Component
        {
            public Color(uint id) : base(id)
            {
            }

            public Colors RGBA
            {
                get => Engine.GetRGBA(EntityId);
                set => Engine.SetRGBA(EntityId, value);
            }

            public new static string GetName() { return "Color"; }

        }
        #endregion

        #region Renderable
        public class Renderer : Component
        {
            public Renderer(uint id) : base(id)
            {
            }

            public string MeshName
            {
                get => Engine.GetMeshName(EntityId);
                set => Engine.SetMeshName(EntityId, value);
            }
            public string MaterialName
            {
                get => Engine.GetMaterialName(EntityId);
                set => Engine.SetMaterialName(EntityId, value);
            }
            public bool IsWireframe
            {
                get => Engine.GetIsWireframe(EntityId);
                set => Engine.SetIsWireframe(EntityId, value);
            }

            public new static string GetName() { return "Renderable"; }

        }
        #endregion

        #region Animator
        public class Animator : Component
        {
            public Animator(uint id) : base(id)
            {
            }
            //public string AnimatorCurrentClip
            //{
            //    get => Engine.GetAnimatorCurrentClip(EntityId);
            //    set => Engine.SetAnimatorCurrentClip(EntityId, value);
            //}

            //public string AnimatorSpriteSheet
            //{
            //    get => Engine.GetAnimatorSpriteSheet(EntityId);
            //    set => Engine.SetAnimatorSpriteSheet(EntityId, value);
            //}

            public int AnimatorCurrentFrameIndex
            {
                get => Engine.GetAnimatorCurrentFrameIndex(EntityId);
                set => Engine.SetAnimatorCurrentFrameIndex(EntityId, value);
            }

            public float AnimatorCurrentTime
            {
                get => Engine.GetAnimatorCurrentTime(EntityId);
                set => Engine.SetAnimatorCurrentTime(EntityId, value);
            }

            public float AnimatorCurrentPlaybackSpeed
            {
                get => Engine.GetAnimatorCurrentPlaybackSpeed(EntityId);
                set => Engine.SetAnimatorCurrentPlaybackSpeed(EntityId, value);
            }

            public bool AnimatorIsPlaying
            {
                get => Engine.GetAnimatorIsPlaying(EntityId);
                set => Engine.SetAnimatorIsPlaying(EntityId, value);
            }

            public bool AnimatorIsLooping
            {
                get => Engine.GetAnimatorIsLooping(EntityId);
                set => Engine.SetAnimatorIsLooping(EntityId, value);
            }

            public bool AnimatorLastFacingRight
            {
                get => Engine.GetAnimatorLastFacingRight(EntityId);
                set => Engine.SetAnimatorLastFacingRight(EntityId, value);
            }

            //public Vector2 AnimatorUVTopLeft
            //{
            //    get => Engine.GetAnimatorUVTopLeft(EntityId);
            //    set => Engine.SetAnimatorUVTopLeft(EntityId, value);
            //}

            //public Vector2 AnimatorUVBottomRight
            //{
            //    get => Engine.GetAnimatorUVBottomRight(EntityId);
            //    set => Engine.SetAnimatorUVTBottomRight(EntityId, value);
            //}

            public new static string GetName() { return "Animator"; }

        }


        #endregion

        #region Font
        public class Font : Component
        {
            public Font(uint id) : base(id)
            {
            }

            public string FontType
            {
                get => Engine.GetFontType(EntityId);
                set => Engine.SetFontType(EntityId, value);
            }
            public string FontText
            {
                get => Engine.GetFontText(EntityId);
                set => Engine.SetFontText(EntityId, value);
            }
            public float FontSize
            {
                get => Engine.GetFontSize(EntityId);
                set => Engine.SetFontSize(EntityId, value);
            }
            public Vector3 FontColor
            {
                get => Engine.GetFontColor(EntityId);
                set => Engine.SetFontColor(EntityId, value);
            }

            public Vector3 FontOffsetPosition
            {
                get => Engine.GetFontOffsetPosition(EntityId);
                set => Engine.SetFontOffsetPosition(EntityId, value);
            }

            public new static string GetName() { return "Font"; }

        }



        #endregion

        #region Audio

        public class Audio : Component
        {
            public Audio(uint id) : base(id)
            {
            }

            public string AudioKey
            {
                get => Engine.GetAudioKey(EntityId);
                set => Engine.SetAudioKey(EntityId, value);
            }

            public float AudioVolume
            {
                get => Engine.GetAudioVolume(EntityId);
                set => Engine.SetAudioVolume(EntityId, value);
            }

            public bool AudioLoop
            {
                get => Engine.GetAudioLoop(EntityId);
                set => Engine.SetAudioLoop(EntityId, value);
            }

            public bool AudioPlayOnStart
            {
                get => Engine.GetAudioPlayOnStart(EntityId);
                set => Engine.SetAudioPlayOnStart(EntityId, value);
            }

            public bool AudioIsPlaying
            {
                get => Engine.GetAudioIsPlaying(EntityId);
                set => Engine.SetAudioIsPlaying(EntityId, value);
            }
            public new static string GetName() { return "Audio"; }

        }

        #endregion

        public class Camera : Component
        {
            public Camera(uint id) : base(id)
            {
            }

            public static uint GetMainCamera()
            {
                return Engine.GetMainCamera();
            }

            public bool CameraIsMain
            {
                get => Engine.GetCameraIsMain(EntityId);
                set => Engine.SetCameraIsMain(EntityId, value);
            }
            public float CameraZoom
            {
                get => Engine.GetCameraZoom(EntityId);
                set => Engine.SetCameraZoom(EntityId, value);
            }
            public float CameraWidth
            {
                get => Engine.GetCameraWidth(EntityId);
                set => Engine.SetCameraWidth(EntityId, value);
            }
            public float CameraHeight
            {
                get => Engine.GetCameraHeight(EntityId);
                set => Engine.SetCameraHeight(EntityId, value);
            }
            public Vector2 CameraOffset
            {
                get => Engine.GetCameraOffset(EntityId);
                set => Engine.SetCameraOffset(EntityId, value);
            }

            public new static string GetName() { return "Camera"; }

        }

        #region Scripting
        //public class Scripting
        //{
        //    private uint EntityId;

        //    public Scripting(uint id)
        //    {
        //        EntityId = id;
        //    }

        //}

        //public T GetScript<T>(uint entityId, string name) where T : Script
        //{
        //    get => Engine.GetScript(entityId, name) as T;
        //}

        #endregion

        #region GetComponentIn
        public T GetComponentInChildren<T>(uint entityParentID) where T : Component
        {
            //Add more compoents as needed (Case Senstitive)
            if (typeof(T) == typeof(Transform))
            {
                Transform component = new Transform(Engine.GetComponentInChildren(EntityId, Transform.GetName()));
                return (T)(object)component;
                //return GetTransformInChildren(parentID) as T;
            }
            else if (typeof(T) == typeof(RigidBody))
            {
                RigidBody component = new RigidBody(Engine.GetComponentInChildren(EntityId, RigidBody.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Collider))
            {
                Collider component = new Collider(Engine.GetComponentInChildren(EntityId, Collider.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Color))
            {
                Color component = new Color(Engine.GetComponentInChildren(EntityId, Color.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Renderer))
            {
                Renderer component = new Renderer(Engine.GetComponentInChildren(EntityId, Renderer.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Animator))
            {
                Animator component = new Animator(Engine.GetComponentInChildren(EntityId, Animator.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Font))
            {
                Font component = new Font(Engine.GetComponentInChildren(EntityId, Font.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Audio))
            {
                Audio component = new Audio(Engine.GetComponentInChildren(EntityId, Audio.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Camera))
            {
                Camera component = new Camera(Engine.GetComponentInChildren(EntityId, Camera.GetName()));
                return (T)(object)component;
            }

            Engine.Log($"GetComponentInChildren<{typeof(T).Name}> not supported, add it in Script.cs & ScriptInterface.h!");
            return null;
        }

        public T GetScriptComponentInChildren<T>(uint entityParentID, string scriptName) where T : Script
        {
            return Engine.GetScriptComponentInChildren<T>(entityParentID, scriptName) as T;
        }


        public T GetComponentInParent<T>(uint entityParentID) where T : Component
        {
            //Add more compoents as needed (Case Senstitive)
            if (typeof(T) == typeof(Transform))
            {
                Transform component = new Transform(Engine.GetComponentInParent(EntityId, Transform.GetName()));
                return (T)(object)component;
                //return GetTransformInChildren(parentID) as T;
            }
            else if (typeof(T) == typeof(RigidBody))
            {
                RigidBody component = new RigidBody(Engine.GetComponentInParent(EntityId, RigidBody.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Collider))
            {
                Collider component = new Collider(Engine.GetComponentInParent(EntityId, Collider.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Color))
            {
                Color component = new Color(Engine.GetComponentInParent(EntityId, Color.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Renderer))
            {
                Renderer component = new Renderer(Engine.GetComponentInParent(EntityId, Renderer.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Animator))
            {
                Animator component = new Animator(Engine.GetComponentInParent(EntityId, Animator.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Font))
            {
                Font component = new Font(Engine.GetComponentInParent(EntityId, Font.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Audio))
            {
                Audio component = new Audio(Engine.GetComponentInParent(EntityId, Audio.GetName()));
                return (T)(object)component;
            }
            else if (typeof(T) == typeof(Camera))
            {
                Camera component = new Camera(Engine.GetComponentInParent(EntityId, Camera.GetName()));
                return (T)(object)component;
            }

            Engine.Log($"GetComponentInParent<{typeof(T).Name}> not supported, add it in Script.cs & ScriptInterface.h!");
            return null;
        }

        public T GetScriptComponentInParent<T>(uint entityParentID, string scriptName) where T : Script
        {
            return Engine.GetScriptComponentInParent<T>(entityParentID, scriptName) as T;
        }
        #endregion

        public float DeltaTime => Engine.DeltaTime;
        //public float Time => Engine.Time;

        // Helper methods
        public void Log(string message) => Engine.Log(message);
        public bool IsKeyPressed(KeyboardKeys key) => Engine.IsKeyPressed(key);
        public bool IsKeyDown(KeyboardKeys key) => Engine.IsKeyDown(key);
        public bool IsKeyReleased(KeyboardKeys key) => Engine.IsKeyReleased(key);
        public void PlaySound(string path) => Engine.PlaySound(path);
        public uint CreateEntityArchetype(string archetypeName, Vector3 pos) => Engine.CreateEntityArchetype(archetypeName, pos);
        public uint GetPlayerID() => Engine.GetPlayerID();

    }
}   // ManagedScripts
