// ============================================================
// EnableOnAbilityUse.cs - Causes object to be active or inactive on ability use
// ============================================================
using System;
using System.Security.AccessControl;

namespace ManagedScripts
{
    public class EnableOnAbilityUse : Script
    {
        [SerializeField]
        public bool isDefaultDisabled = true; //

        public Transform transform;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            DoAbilityTexture();
        }

        public override void OnUpdate()
        {
            DoAbilityTexture();
        }

        public void DoAbilityTexture()
        {
            if (GetPlayerUsingAbility())
                EnableEntity();
            else
                DisableEntity();

            return;
        }

        public void EnableEntity()
        {
            transform.IsActive = true;
        }

        public void DisableEntity()
        {
            transform.IsActive = false;
        }


        public override void OnDestroy()
        {
        }
    }
}