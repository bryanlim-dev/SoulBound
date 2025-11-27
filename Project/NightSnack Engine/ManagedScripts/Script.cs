namespace ManagedScripts
{
    // Base class for all game scripts
    public abstract class Script
    {
        // Set by engine when script is created
        public uint EntityId;

        // Override these in your script classes
        public virtual void OnStart() { }
        public virtual void OnUpdate() { }
        public virtual void OnDestroy() { }

        // Convenience properties
        #region Transform
        public class Transform
        {
            private uint EntityId;

            public Transform(uint id)
            {
                EntityId = id;
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
        }
        #endregion

        #region RigidBody
        public class RigidBody
        {
            private uint EntityId;

            public RigidBody(uint id)
            {
                EntityId = id;
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
        }
        #endregion //Rigidbody

        #region Collider
        public class Collider
        {
            private uint EntityId;

            public Collider(uint id)
            {
                EntityId = id;
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
        }

        #endregion

        #region Color
        public Colors Color
        {
            get => Engine.GetRGBA(EntityId);
            set => Engine.SetRGBA(EntityId, value);
        }
        #endregion

        #region Renderable
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
        #endregion

        #region Animator
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




        #endregion

        #region Font
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



        #endregion

        public float DeltaTime => Engine.DeltaTime;
        //public float Time => Engine.Time;

        // Helper methods
        public void Log(string message) => Engine.Log(message);
        public bool IsKeyPressed(int key) => Engine.IsKeyPressed(key);
        public bool IsKeyDown(int key) => Engine.IsKeyDown(key);
        public bool IsKeyReleased(int key) => Engine.IsKeyReleased(key);
        public void PlaySound(string path) => Engine.PlaySound(path);
        public uint CreateEntityArchetype(string archetypeName, Vector3 pos) => Engine.CreateEntityArchetype(archetypeName, pos);
    }
}   // ManagedScripts
