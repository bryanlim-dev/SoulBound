// ============================================================
// AbilityAppear.cs - Causes object texture to change based on current player ability
// ============================================================
using System;
using System.Security.AccessControl;

namespace ManagedScripts
{
    public class AbilityTextureChange : Script
    {
        [SerializeField]
        public bool isDefaultTexture = true; //Is current texture the deafult, if not swap to the alt texture

        [SerializeField]
        public string abilityNameToTrigger = "Grapple"; //Grapple/Glide/Bounce

        [SerializeField]
        public string altTexture = "blank"; //Texture to swap to when triggered

        public string defaultTexture = "";

        public Transform transform;
        public Renderer renderer;
         

        bool isCurrDefault = true;

        public override void OnStart()
        {
            transform = new Transform(EntityId);
            renderer = new Renderer(EntityId);

            isCurrDefault = isDefaultTexture;
            defaultTexture = renderer.MaterialName;

            DoCheckTexture(true);
        }

        public override void OnUpdate()
        {
            DoCheckTexture();
        }

        public void DoCheckTexture(bool bypassCheck = false)
        {
            //Ability Name matches, trigger swap
            if (abilityNameToTrigger == GetPlayerAbility())
            {
                if (isDefaultTexture != isCurrDefault && !bypassCheck)
                    return;

                if (isDefaultTexture)
                    SwapToAlt();
                else
                    SwapToDefault();
            }
            //Ability name dosnt match go default
            else
            {
                if (isDefaultTexture == isCurrDefault && !bypassCheck)
                    return;

                if (isDefaultTexture)
                    SwapToDefault();
                else
                    SwapToAlt();
            }
        }

        public void SwapToDefault()
        {
            isCurrDefault = true;
            renderer.MaterialName = defaultTexture;
        }

        public void SwapToAlt()
        {
            isCurrDefault = false;
            renderer.MaterialName = altTexture;
        }


        public override void OnDestroy()
        {
        }
    }
}