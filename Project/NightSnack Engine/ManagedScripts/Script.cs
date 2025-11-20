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
        protected Vector3 Position
        {
            get => Engine.GetPosition(EntityId);
            set => Engine.SetPosition(EntityId, value);
        }
        protected float Rotation
        {
            get => Engine.GetRotation(EntityId);
            set => Engine.SetRotation(EntityId, value);
        }
        protected Vector2 Scale
        {
            get => Engine.GetScale(EntityId);
            set => Engine.SetScale(EntityId, value);
        }

        protected LayerType Layer
        {
            get => Engine.GetLayer(EntityId);
            set => Engine.SetLayer(EntityId, value);
        }
        #endregion

        #region RigidBody
        #region Force
        protected Vector2 ForceDirection
        {
            get => Engine.GetForceDirection(EntityId);
            set => Engine.SetForceDirection(EntityId, value);
        }
        protected float ForceMagnitude
        {
            get => Engine.GetForceMagnitude(EntityId);
            set => Engine.SetForceMagnitude(EntityId, value);
        }
        protected float ForceLifetime
        {
            get => Engine.GetForceLifetime(EntityId);
            set => Engine.SetForceLifetime(EntityId, value);
        }
        protected bool ForceActive 
        {
            get => Engine.GetForceActive(EntityId);
            set => Engine.SetForceActive(EntityId, value);
        }
        #endregion //Force

        protected Vector2 Velocity
        {
            get => Engine.GetVelocity(EntityId);
            set => Engine.SetVelocity(EntityId, value);
        }
        protected Vector2 Accleration
        {
            get => Engine.GetAcceleration(EntityId);
            set => Engine.SetAcceleration(EntityId, value);
        }
        protected float InvMass
        {
            get => Engine.GetInvMass(EntityId);
            set => Engine.SetInvMass(EntityId, value);
        }
        protected float InvInertia
        {
            get => Engine.GetInvInertia(EntityId);
            set => Engine.SetInvInertia(EntityId, value);
        }
        protected float Restitution
        {
            get => Engine.GetRestitution(EntityId);
            set => Engine.SetRestitution(EntityId, value);
        }
        protected bool UseGravity
        {
            get => Engine.GetUseGravity(EntityId);
            set => Engine.SetUseGravity(EntityId, value);
        }
        protected bool UseRotate
        {
            get => Engine.GetUseRotate(EntityId);
            set => Engine.SetUseRotate(EntityId, value);
        }
        #endregion //Rigidbody

        #region Collider

        protected ColliderType ColType
        {
            get => Engine.GetColliderType(EntityId);
            set => Engine.SetColliderType(EntityId, value);
        }
        protected ShapeType ColShape
        {
            get => Engine.GetColliderShapeType(EntityId);
            set => Engine.SeColliderShapeType(EntityId, value);
        }

        protected bool ColStatic
        {
            get => Engine.GetColliderStatic(EntityId);
            set => Engine.SetColliderStatic(EntityId, value);
        }

        protected float ColHeight
        {
            get => Engine.GetColliderHeight(EntityId);
            set => Engine.SetColliderHeight(EntityId, value);
        }

        protected float ColWidth
        {
            get => Engine.GetColliderWidth(EntityId);
            set => Engine.SetColliderWidth(EntityId, value);
        }
        protected float ColRadius
        {
            get => Engine.GetColliderRadius(EntityId);
            set => Engine.SetColliderRadius(EntityId, value);
        }

        protected bool ColGrounded
        {
            get => Engine.GetColliderGrounded(EntityId);
            set => Engine.SetColliderGrounded(EntityId, value);
        }

        protected bool ColColliding
        {
            get => Engine.GetColliderColliding(EntityId);
            set => Engine.SetColliderColliding(EntityId, value);
        }

        #endregion

        #region Color
        protected Colors Color
        {
            get => Engine.GetRGBA(EntityId);
            set => Engine.SetRGBA(EntityId, value);
        }
        #endregion

        #region Renderable
        protected string MeshName
        {
            get => Engine.GetMeshName(EntityId);
            set => Engine.SetMeshName(EntityId, value);
        }
        protected string MaterialName
        {
            get => Engine.GetMaterialName(EntityId);
            set => Engine.SetMaterialName(EntityId, value);
        }
        protected bool IsWireframe
        {
            get => Engine.GetIsWireframe(EntityId);
            set => Engine.SetIsWireframe(EntityId, value);
        }
        #endregion

        #region Renderable
        protected string AnimatorCurrentClip
        {
            get => Engine.GetAnimatorCurrentClip(EntityId);
            set => Engine.SetAnimatorCurrentClip(EntityId, value);
        }

        protected string AnimatorSpriteSheet
        {
            get => Engine.GetAnimatorSpriteSheet(EntityId);
            set => Engine.SetAnimatorSpriteSheet(EntityId, value);
        }

        protected int AnimatorCurrentFrameIndex
        {
            get => Engine.GetAnimatorCurrentFrameIndex(EntityId);
            set => Engine.SetAnimatorCurrentFrameIndex(EntityId, value);
        }

        protected float AnimatorCurrentTime
        {
            get => Engine.GetAnimatorCurrentTime(EntityId);
            set => Engine.SetAnimatorCurrentTime(EntityId, value);
        }

        protected float AnimatorCurrentPlaybackSpeed
        {
            get => Engine.GetAnimatorCurrentPlaybackSpeed(EntityId);
            set => Engine.SetAnimatorCurrentPlaybackSpeed(EntityId, value);
        }

        protected bool AnimatorIsPlaying
        {
            get => Engine.GetAnimatorIsPlaying(EntityId);
            set => Engine.SetAnimatorIsPlaying(EntityId, value);
        }

        protected bool AnimatorIsLooping
        {
            get => Engine.GetAnimatorIsLooping(EntityId);
            set => Engine.SetAnimatorIsLooping(EntityId, value);
        }

        protected bool AnimatorLastFacingRight
        {
            get => Engine.GetAnimatorLastFacingRight(EntityId);
            set => Engine.SetAnimatorLastFacingRight(EntityId, value);
        }

        protected Vector2 AnimatorUVTopLeft
        {
            get => Engine.GetAnimatorUVTopLeft(EntityId);
            set => Engine.SetAnimatorUVTopLeft(EntityId, value);
        }

        protected Vector2 AnimatorUVBottomRight
        {
            get => Engine.GetAnimatorUVBottomRight(EntityId);
            set => Engine.SetAnimatorUVTBottomRight(EntityId, value);
        }




        #endregion

        protected float DeltaTime => Engine.DeltaTime;
        //protected float Time => Engine.Time;

        // Helper methods
        protected void Log(string message) => Engine.Log(message);
        protected bool IsKeyPressed(int key) => Engine.IsKeyPressed(key);
        protected bool IsKeyDown(int key) => Engine.IsKeyDown(key);
        protected bool IsKeyReleased(int key) => Engine.IsKeyReleased(key);
        protected void PlaySound(string path) => Engine.PlaySound(path);
        protected uint CreateEntityArchetype(string archetypeName, Vector3 pos) => Engine.CreateEntityArchetype(archetypeName, pos);
    }
}   // ManagedScripts
