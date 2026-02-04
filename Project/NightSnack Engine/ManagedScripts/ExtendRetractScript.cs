using System.Diagnostics;

namespace ManagedScripts
{
    public class ExtendRetractScript : Script
    {
        [SerializeField]
        public float moveSpeed = 50f;

        [SerializeField]
        //Amount of units to move
        public float maxMoveUnits = 50;

        //Units it has moved in this dir
        private float movedUnits = 0;

        [SerializeField]
        public bool isDirUpDown = true;

        [SerializeField]
        //If true the object is currently extended, currently moving down
        //If flase object is currently retracted, currently moving up
        public bool isExtended = true;

        public Transform transform;

        public override void OnLoad()
        {
            transform = new Transform(EntityId);
        }

        public override void OnStart()
        {
            movedUnits = 0;
        }

        public override void OnUpdate()
        {
            bool isMaxMoved = false;
            float unitsToMove = DeltaTime * moveSpeed;

            Vector3 currPos = transform.Position;

            //Within bounds
            if (movedUnits + unitsToMove >= maxMoveUnits)
            {
                unitsToMove = (movedUnits + unitsToMove) - maxMoveUnits;
                isMaxMoved = true;
            }

            //Always positive easier to track :D
            movedUnits += unitsToMove;


            if (isExtended) //Next state is down
            {
                unitsToMove = -unitsToMove;
            }

            if (isDirUpDown)
                currPos.Y += unitsToMove;
            else
                currPos.X += unitsToMove;

            transform.Position = currPos;
            //Engine.Log("Pos: " + transform.Position.X + ":" + transform.Position.Y);

            if (isMaxMoved)
            {
                isExtended = !isExtended;
                movedUnits = 0;
            }

        }

        public override void OnDestroy()
        {

        }
    }
}