using System.Collections;
using System.Diagnostics;

namespace ManagedScripts
{
    public class FrogTongue : Script
    {
        [SerializeField]
        private Camera mainCamera;
        private LineRenderer lineRenderer;
        private RigidBody playerRb;
        private DistanceJoint2D joint; // Stores the joint reference

        // NOTE: These should be World Space positions, not local
        public Vector2 tongueStartingPos;
        public Vector2 tongueEndingPos;

        // Configurable speeds and length
        public float extensionDuration = 0.1f; // Use duration instead of speed for Lerp
        public float retractionDuration = 0.1f;
        public float maxTongueLength = 5f;
        public LayerMask stickableLayer; //ground,walls for now

        // The current state of the line
        private Coroutine movementCoroutine; // To manage the current active movement
        private bool isLineActive = false;
        private bool hasStuckTarget = false;
        private bool canReleaseTarget = false;
        private bool isRetracting = true;

        private bool swingControlsEnabled = false;
        public float swingForce = 35f;      // Force applied per FixedUpdate
        public float maxSwingSpeed = 10f;   // Limit the total magnitude of velocity

        public GameObject targetCircle;
        private SpriteRenderer targetSprite;
        public Sprite targetNormalCircle;
        public Sprite targetHitCircle;
        public GameObject helpUI;

        public bool crossHair = true;

        //[Header("Audio Clips")]
        //[SerializeField] private SFXAnimEvents sfxAudio;
        //public AudioClip tongueSFX;

        [SerializeField]
        public Audio sfxAudio;
        [SerializeField]
        public string toungeSFX;

        public override void OnLoad()
        {
            mainCamera = new Camera(Camera.GetMainCamera());
            sfxAudio = new Audio(EntityId);
            playerRb = GetComponentInParent<RigidBody>(EntityId);
            if (playerRb != null)
                joint = GetScriptComponentInParent<DistanceJoint2D>(playerRb.EntityId, "DistanceJoint2D");
            else
                Engine.Log("No rigidbody founnd");

            lineRenderer = GetComponent<LineRenderer>();
            //playerRb = GetComponentInParent<Rigidbody2D>();
            targetSprite = targetCircle.GetComponent<SpriteRenderer>();
        }

        public override void OnStart()
        {
            //make sure its only 2 points, start and end
            lineRenderer.positionCount = 2;
        }
        public void SwingControls()
        {
            if (!swingControlsEnabled)
                return;

            if (joint == null || !joint.enabled || !Input.GetKeyDown(KeyCode.Space))
            {
                return;
            }
            float dir = playerRb.GetComponent<PlayerMovement>().isFacingRight ? -1 : 1;

            Vector2 ropeVector = joint.connectedAnchor - (Vector2)transform.position;

            // Perpendicular vector (tangent) for swing direction
            Vector2 tangentVector = Vector3.Cross(Vector3.forward, ropeVector).normalized;

            playerRb.AddForce(tangentVector * dir * swingForce, ForceMode2D.Force);
            if (playerRb.linearVelocity.magnitude > maxSwingSpeed)
            {
                // Clamp the velocity vector to the maximum allowed speed.
                playerRb.linearVelocity = playerRb.linearVelocity.normalized * maxSwingSpeed;
            }
        }

        // This must be World Space because it's the frog's current position
        public void UpdateStartingPos(PlayerMovement playerMovement)
        {
            tongueStartingPos = playerMovement.transform.position;
        }

        public void ExtendTongue()
        {
            isRetracting = false;
            if (isLineActive) return; // Ignore if already moving

            sfxAudio.PlaySFX(tongueSFX);
            if (movementCoroutine != null) StopCoroutine(movementCoroutine);

            movementCoroutine = StartCoroutine(DoTongueMovement(tongueStartingPos, tongueEndingPos, extensionDuration, true));
        }

        public void RetractTongue()
        {
            isRetracting = true;
            // Only retract if the line is already out or moving
            if (!isLineActive)
            {
                // If already fully retracted, just update the position in case the frog moved
                SetLinePositions(tongueStartingPos, tongueStartingPos);
                return;
            }
            ReleaseFromWall();

            if (movementCoroutine != null) StopCoroutine(movementCoroutine);

            // Retract from the current position back to the mouth
            movementCoroutine = StartCoroutine(DoTongueMovement(tongueEndingPos, tongueStartingPos, retractionDuration, false));
        }

        private IEnumerator DoTongueMovement(Vector3 startWorldPos, Vector3 endWorldPos, float duration, bool extending)
        {
            isLineActive = true;
            float timeElapsed = 0f;

            while (timeElapsed < duration)
            {
                float t = timeElapsed / duration;

                // 1. LERP IN WORLD SPACE
                Vector3 currentWorldTip = Vector3.Lerp(startWorldPos, endWorldPos, t);

                // 2. CONVERT TO LOCAL SPACE AND UPDATE LINE RENDERER
                // Use the helper method
                SetLinePositions(tongueStartingPos, currentWorldTip);

                timeElapsed += Time.deltaTime;
                yield return null;
            }

            // Final snap to target
            SetLinePositions(tongueStartingPos, endWorldPos);
            movementCoroutine = null;

            if (isRetracting)
                isLineActive = false;

            if (!canReleaseTarget && !isRetracting)
                RetractTongue();

        }
        public void ResetFrogTongue()
        {
            isLineActive = false;
            SetLinePositions(tongueStartingPos, tongueStartingPos);
            Debug.Log("released 1");
            StopCoroutine(SlowTime());
            Debug.Log("released 2");
            playerRb.GetComponent<PlayerMovement>().disableMovement = false;
            Debug.Log("released 3");
            playerRb.linearDamping = 0f;
            Debug.Log("released 4");
            gameObject.SetActive(false);
            Debug.Log("released 5");
        }
        // Helper method to update the Line Renderer with world positions
        private void SetLinePositions(Vector3 worldStart, Vector3 worldEnd)
        {
            lineRenderer.SetPosition(0, worldStart);
            lineRenderer.SetPosition(1, worldEnd);
        }

        // (Rest of your GetInputDirection and OnDrawGizmos methods remain the same)
        public void GetInputDirection(PlayerMovement playerMovement)
        {
            //tongue direction script
            /*        float h = Input.GetAxis("Horizontal");
                    float v = Input.GetAxis("Vertical");

                    //if no inputs
                    Vector2 left = new Vector2(-1f, 0).normalized;
                    Vector2 right = new Vector2(1f, 0).normalized;

                    Vector2 shootDirection;
                    Vector2 inputDir = new Vector2(h, v).normalized;

                    if (inputDir != Vector2.zero)
                    {
                        shootDirection = inputDir;
                    }
                    else
                    {
                        // Default to facing direction
                        if (playerMovement != null)
                        {
                            shootDirection = playerMovement.isFacingRight ? Vector2.right : Vector2.left;
                        }
                        else
                        {
                            shootDirection = Vector2.right;
                        }
                    }*/

            if (playerMovement.isGrounded() && !swingControlsEnabled)
            {
                StopCoroutine(SlowTime());
                playerRb.GetComponent<PlayerMovement>().disableMovement = false;
                playerRb.linearDamping = 0f;
            }

            Vector2 mousePos = (Vector2)mainCamera.ScreenToWorldPoint(Input.mousePosition);
            Vector2 shootDirection = (mousePos - tongueStartingPos).normalized;
            //tongue direction script


            RaycastHit2D hit = Physics2D.Raycast(
                tongueStartingPos,
                shootDirection,
                maxTongueLength,
                stickableLayer // Use a general layer for performance
            );

            //aiming when attached
            if (swingControlsEnabled)
            {
                playerMovement.horizontal = mousePos.x > transform.position.x ? 1f : -1f;

                Debug.Log(playerMovement.horizontal);
                helpUI.SetActive(true);
                if (hit.collider != null)
                {
                    // Check if the hit object implements the IStickable interface
                    IStickable stickable = hit.collider.GetComponent<IStickable>();

                    if (stickable != null)
                    {
                        targetCircle.transform.position = hit.point;
                        targetSprite.sprite = targetHitCircle;
                    }
                    else
                    {
                        // HIT but NOT STICKABLE: Stops at the obstacle but won't stick.
                        targetCircle.transform.position = hit.point;
                        targetSprite.sprite = targetNormalCircle;
                    }
                }
                else
                {
                    // MISS: The tongue extends to the full maxTongueLength.
                    targetCircle.transform.position = tongueStartingPos + shootDirection * maxTongueLength;
                    targetSprite.sprite = targetNormalCircle;
                }
            }
            //aiming when attached

            //stickable script
            if (!canReleaseTarget)
            {
                helpUI.SetActive(false);
                if (hit.collider != null)
                {
                    // Check if the hit object implements the IStickable interface
                    IStickable stickable = hit.collider.GetComponent<IStickable>();

                    if (stickable != null)
                    {
                        // HIT & STICKABLE: The tongue stops at the exact hit point.
                        tongueEndingPos = hit.point;
                        targetSprite.sprite = targetHitCircle;
                        hasStuckTarget = true;
                        stickable.OnTongueHit(this); // You can trigger the wall logic here if needed
                    }
                    else
                    {
                        // HIT but NOT STICKABLE: Stops at the obstacle but won't stick.
                        tongueEndingPos = hit.point;
                        targetSprite.sprite = targetNormalCircle;
                        hasStuckTarget = false;
                    }
                }
                else
                {
                    // MISS: The tongue extends to the full maxTongueLength.
                    tongueEndingPos = tongueStartingPos + shootDirection * maxTongueLength;
                    targetSprite.sprite = targetNormalCircle;
                    hasStuckTarget = false;
                }
            }
            //stickable script

            if (isLineActive)
            {
                SetLinePositions(tongueStartingPos, tongueEndingPos);
            }
            else
            {
                SetLinePositions(tongueStartingPos, tongueStartingPos);
            }
            DrawTargetCircle();
        }

        public void AttachToWall()
        {
            if (isRetracting)
                return;
            //return when nothing has been hit and its allowed to release the surface
            if (!hasStuckTarget || canReleaseTarget)
                return;
            // 1. Get the Joint Component (Assuming it's already on the player's GameObject)
            joint = playerRb.GetComponent<DistanceJoint2D>();
            if (joint == null)
            {
                // Add it if it's missing (though better to add it once in the Editor)
                joint = playerRb.gameObject.AddComponent<DistanceJoint2D>();
            }

            // 2. Configure the Joint

            // Disable auto-configuration: we set the distance manually
            joint.autoConfigureDistance = false;
            joint.enableCollision = true;
            // The point on the world the player is attaching to
            joint.connectedAnchor = tongueEndingPos;

            // Set the anchor point on the player's Rigidbody (usually center: 0, 0)
            joint.anchor = Vector2.zero;

            // Set the length of the rope/tongue
            // This is the distance between the player's position and the stick point
            joint.distance = Vector2.Distance(playerRb.position, tongueEndingPos);

            // Enable the joint
            joint.enabled = true;
            StopAllCoroutines();
            canReleaseTarget = true;

            playerRb.linearDamping = 1f;
            playerRb.GetComponent<PlayerMovement>().disableMovement = true;
            swingControlsEnabled = true;
        }

        private void ReleaseFromWall()
        {
            //return when nothing has been hit and its not allowed to release the surface
            if (!canReleaseTarget)
                return;
            // CRITICAL: Destroy or disable the joint to allow free movement
            if (joint != null)
            {
                // Disabling is usually safer than destroying
                joint.enabled = false;

                // Optionally, you could destroy it if you don't use it elsewhere:
                // Destroy(joint); 
                joint = null;
            }

            StartCoroutine(SlowTime());
            swingControlsEnabled = false;
            canReleaseTarget = false;
            hasStuckTarget = false;
        }
        private void DrawTargetCircle()
        {
            if (!crossHair)
            {
                targetCircle.SetActive(false);
            }
            else
            {
                targetCircle.SetActive(true);
            }

            if (!swingControlsEnabled)
                targetCircle.transform.position = tongueEndingPos;
        }

        private IEnumerator SlowTime()
        {
            yield return new WaitForSecondsRealtime(0.5f);
            playerRb.GetComponent<PlayerMovement>().disableMovement = false;
            playerRb.linearDamping = 0f;
        }
    }
}