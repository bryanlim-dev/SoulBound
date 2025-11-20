/******************************************************************************
 * File:        MonoInternalCalls.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file defines internal calls that will link:
 * C# functions defined in Engine API in ManagedScripts/Engine.cs 
 * to NightSnack's native ScriptInterface's C++ functions. 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Scripting/MonoInternalCalls.h"

#include "Scripting/ScriptInterface.h"
#include "Scripting/ScriptTypes.h"

#include "mono/metadata/object.h"
#include "mono/metadata/appdomain.h"

#include <iostream>

// Global pointer to script interface (set during initialization)
static NightSnack::Scripting::ScriptInterface* g_scriptInterface = nullptr;

namespace NightSnack::Scripting {
    MonoString* StringToMonoString(std::string str)
    {
        MonoDomain* domain = mono_domain_get();
        return mono_string_new(domain, str.c_str());
    }
}
// Internal call functions from C# must use C linkage
extern "C" {
    // --------------------------------------------------------------

#pragma region Transform
	// Position --------------
	static void NightSnack_GetPosition(uint32_t id, NightSnack::Scripting::vec3* outpos) {
		if (!g_scriptInterface || !outpos) { return; }
		*outpos = g_scriptInterface->GetPosition(id);
	}
	static void NightSnack_SetPosition(uint32_t id, NightSnack::Scripting::vec3* pos) {
		if (!g_scriptInterface || !pos) { return; }
		g_scriptInterface->SetPosition(id, *pos);
	}

    // Rotation --------------
    static void NightSnack_GetRotation(uint32_t id, float* outrot) {
        if (!g_scriptInterface || !outrot) { return; }
        *outrot = g_scriptInterface->GetRotation(id);
    }
    static void NightSnack_SetRotation(uint32_t id, float* rot) {
        if (!g_scriptInterface || !rot) { return; }
        g_scriptInterface->SetRotation(id, *rot);
    }

    // Scale -----------------
    static void NightSnack_GetScale(uint32_t id, NightSnack::Scripting::vec2* scale) {
        if (!g_scriptInterface || !scale) { return; }
        *scale = g_scriptInterface->GetScale(id);
    }
    static void NightSnack_SetScale(uint32_t id, NightSnack::Scripting::vec2* scale) {
        if (!g_scriptInterface || !scale) { return; }
        g_scriptInterface->SetScale(id, *scale);
    }

    // Layer -----------------
    static void NightSnack_GetLayer(uint32_t id, uint8_t* layer) {
        if (!g_scriptInterface || !layer) { return; }
        *layer = static_cast<uint8_t>(g_scriptInterface->GetLayer(id));
    }
    static void NightSnack_SetLayer(uint32_t id, uint8_t* layer) {
        if (!g_scriptInterface || !layer) { return; }
        g_scriptInterface->SetLayer(id, static_cast<NightSnack::LayerType>(*layer));
    }
#pragma endregion //Transform

    // --------------------------------------------------------------

#pragma region Rigidbody
#pragma region Force
    // Direction --------------
    static void NightSnack_GetForceDirection(uint32_t id, NightSnack::Scripting::vec2* outdir) {
        if (!g_scriptInterface || !outdir) { return; }
        *outdir = g_scriptInterface->GetForceDirection(id);
    }
    static void NightSnack_SetForceDirection(uint32_t id, NightSnack::Scripting::vec2* dir) {
        if (!g_scriptInterface || !dir) { return; }
        g_scriptInterface->SetForceDirection(id, *dir);
    }

    // Magnitude --------------
    static void NightSnack_GetForceMagnitude(uint32_t id, float* outmag) {
        if (!g_scriptInterface || !outmag) { return; }
        *outmag = g_scriptInterface->GetForceMagnitude(id);
    }
    static void NightSnack_SetForceMagnitude(uint32_t id, float* mag) {
        if (!g_scriptInterface || !mag) { return; }
        g_scriptInterface->SetForceMagnitude(id, *mag);
    }

    // Lifetime --------------
    static void NightSnack_GetForceLifetime(uint32_t id, float* outLife) {
        if (!g_scriptInterface || !outLife) { return; }
        *outLife = g_scriptInterface->GetForceLifetime(id);
    }
    static void NightSnack_SetForceLifetime(uint32_t id, float* life) {
        if (!g_scriptInterface || !life) { return; }
        g_scriptInterface->SetForceLifetime(id, *life);
    }

    // Active --------------
    static void NightSnack_GetForceActive(uint32_t id, bool* outActive) {
        if (!g_scriptInterface || !outActive) { return; }
        *outActive = g_scriptInterface->GetForceActive(id);
    }
    static void NightSnack_SetForceActive(uint32_t id, bool* active) {
        if (!g_scriptInterface || !active) { return; }
        g_scriptInterface->SetForceActive(id, *active);
    }


#pragma endregion //Force

    // Velocity -----------------
    static void NightSnack_GetVelocity(uint32_t id, NightSnack::Scripting::vec2* outVel) {
        if (!g_scriptInterface || !outVel) { return; }
        *outVel = g_scriptInterface->GetVelocity(id);
    }
    static void NightSnack_SetVelocity(uint32_t id, NightSnack::Scripting::vec2* vel) {
        if (!g_scriptInterface || !vel) { return; }
        g_scriptInterface->SetVelocity(id, *vel);
    }

    // Acceleration -----------------
    static void NightSnack_GetAcceleration(uint32_t id, NightSnack::Scripting::vec2* outAcc) {
        if (!g_scriptInterface || !outAcc) { return; }
        *outAcc = g_scriptInterface->GetAcceleration(id);
    }
    static void NightSnack_SetAcceleration(uint32_t id, NightSnack::Scripting::vec2* acc) {
        if (!g_scriptInterface || !acc) { return; }
        g_scriptInterface->SetAcceleration(id, *acc);
    }

    // InvMass --------------
    static void NightSnack_GetInvMass(uint32_t id, float* outInvMass) {
        if (!g_scriptInterface || !outInvMass) { return; }
        *outInvMass = g_scriptInterface->GetInvMass(id);
    }
    static void NightSnack_SetInvMass(uint32_t id, float* invMass) {
        if (!g_scriptInterface || !invMass) { return; }
        g_scriptInterface->SetInvMass(id, *invMass);
    }

    // InvInertia --------------
    static void NightSnack_GetInvInertia(uint32_t id, float* outInvInertia) {
        if (!g_scriptInterface || !outInvInertia) { return; }
        *outInvInertia = g_scriptInterface->GetInvInertia(id);
    }
    static void NightSnack_SetInvInertia(uint32_t id, float* invInertia) {
        if (!g_scriptInterface || !invInertia) { return; }
        g_scriptInterface->SetInvInertia(id, *invInertia);
    }

    // Restitution --------------
    static void NightSnack_GetRestitution(uint32_t id, float* outrestitution) {
        if (!g_scriptInterface || !outrestitution) { return; }
        *outrestitution = g_scriptInterface->GetRestitution(id);
    }
    static void NightSnack_SetRestitution(uint32_t id, float* restitution) {
        if (!g_scriptInterface || !restitution) { return; }
        g_scriptInterface->SetRestitution(id, *restitution);
    }

    // GravityMult --------------
    static void NightSnack_GetGravityMult(uint32_t id, float* outMult) {
        if (!g_scriptInterface || !outMult) { return; }
        *outMult = g_scriptInterface->GetGravityMult(id);
    }
    static void NightSnack_SetGravityMult(uint32_t id, float* mult) {
        if (!g_scriptInterface || !mult) { return; }
        g_scriptInterface->SetGravityMult(id, *mult);
    }

    // UseGravity --------------
    static void NightSnack_GetUseGravity(uint32_t id, bool* outuseGravity) {
        if (!g_scriptInterface || !outuseGravity) { return; }
        *outuseGravity = g_scriptInterface->GetUseGravity(id);
    }
    static void NightSnack_SetUseGravity(uint32_t id, bool* useGravity) {
        if (!g_scriptInterface || !useGravity) { return; }
        g_scriptInterface->SetUseGravity(id, *useGravity);
    }

    // UseRotate --------------
    static void NightSnack_GetUseRotate(uint32_t id, bool* outuseRotate) {
        if (!g_scriptInterface || !outuseRotate) { return; }
        *outuseRotate = g_scriptInterface->GetUseRotate(id);
    }
    static void NightSnack_SetUseRotate(uint32_t id, bool* useRotate) {
        if (!g_scriptInterface || !useRotate) { return; }
        g_scriptInterface->SetUseRotate(id, *useRotate);
    }
#pragma endregion //Rigidbody

    // --------------------------------------------------------------

#pragma region Collider
    // ShapeType -----------------
    static void NightSnack_GetColliderShape(uint32_t id, uint8_t* shape) {
        if (!g_scriptInterface || !shape) { return; }
        *shape = static_cast<uint8_t>(g_scriptInterface->GetColliderShape(id));
    }
    static void NightSnack_SetColliderShape(uint32_t id, uint8_t* shape) {
        if (!g_scriptInterface || !shape) { return; }
        g_scriptInterface->SetColliderShape(id, static_cast<NightSnack::ShapeType>(*shape));
    }

    // ColliderType -----------------
    static void NightSnack_GetColliderType(uint32_t id, uint8_t* type) {
        if (!g_scriptInterface || !type) { return; }
        *type = static_cast<uint8_t>(g_scriptInterface->GetColliderType(id));
    }
    static void NightSnack_SetColliderType(uint32_t id, uint8_t* type) {
        if (!g_scriptInterface || !type) { return; }
        g_scriptInterface->SetColliderType(id, static_cast<NightSnack::ColliderType>(*type));
    }

    // isStatic --------------
    static void NightSnack_GetColliderStatic(uint32_t id, bool* outcolStatic) {
        if (!g_scriptInterface || !outcolStatic) { return; }
        *outcolStatic = g_scriptInterface->GetColliderStatic(id);
    }
    static void NightSnack_SetColliderStatic(uint32_t id, bool* colStatic) {
        if (!g_scriptInterface || !colStatic) { return; }
        g_scriptInterface->SetColliderStatic(id, *colStatic);
    }

    // Height --------------
    static void NightSnack_GetColliderHeight(uint32_t id, float* outheight) {
        if (!g_scriptInterface || !outheight) { return; }
        *outheight = g_scriptInterface->GetColliderHeight(id);
    }
    static void NightSnack_SetColliderHeight(uint32_t id, float* height) {
        if (!g_scriptInterface || !height) { return; }
        g_scriptInterface->SetColliderHeight(id, *height);
    }

    // Width --------------
    static void NightSnack_GetColliderWidth(uint32_t id, float* outwidth) {
        if (!g_scriptInterface || !outwidth) { return; }
        *outwidth = g_scriptInterface->GetColliderWidth(id);
    }
    static void NightSnack_SetColliderWidth(uint32_t id, float* width) {
        if (!g_scriptInterface || !width) { return; }
        g_scriptInterface->SetColliderWidth(id, *width);
    }

    // Radius --------------
    static void NightSnack_GetColliderRadius(uint32_t id, float* outradius) {
        if (!g_scriptInterface || !outradius) { return; }
        *outradius = g_scriptInterface->GetColliderRadius(id);
    }
    static void NightSnack_SetColliderRadius(uint32_t id, float* radius) {
        if (!g_scriptInterface || !radius) { return; }
        g_scriptInterface->SetColliderRadius(id, *radius);
    }

    // isGrounded --------------
    static void NightSnack_GetColliderGrounded(uint32_t id, bool* outgrounded) {
        if (!g_scriptInterface || !outgrounded) { return; }
        *outgrounded = g_scriptInterface->GetColliderGrounded(id);
    }
    static void NightSnack_SetColliderGrounded(uint32_t id, bool* grounded) {
        if (!g_scriptInterface || !grounded) { return; }
        g_scriptInterface->SetColliderGrounded(id, *grounded);
    }

    // Colliding --------------
    static void NightSnack_GetColliderColliding(uint32_t id, bool* outcollided) {
        if (!g_scriptInterface || !outcollided) { return; }
        *outcollided = g_scriptInterface->GetColliderColliding(id);
    }
    static void NightSnack_SetColliderColliding(uint32_t id, bool* collided) {
        if (!g_scriptInterface || !collided) { return; }
        g_scriptInterface->SetColliderColliding(id, *collided);
    }

#pragma endregion

    // --------------------------------------------------------------
    
#pragma region Color
    // RGBA -----------------
    static void NightSnack_GetColorRGBA(uint32_t id, NightSnack::Scripting::vec4* outVec) {
        if (!g_scriptInterface || !outVec) { return; }
        *outVec = g_scriptInterface->GetColorRGBA(id);
    }
    static void NightSnack_SetColorRGBA(uint32_t id, NightSnack::Scripting::vec4* vec) {
        if (!g_scriptInterface || !vec) { return; }
        g_scriptInterface->SetColorRGBA(id, *vec);
    }
#pragma endregion

    // --------------------------------------------------------------
    
#pragma region Renderable
    // meshName --------------
    static MonoString* NightSnack_GetMeshName(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetMeshName(id));
    }
    static void NightSnack_SetMeshName(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }
        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->SetMeshName(id, std::string{ cStr });
    }

    // materialName --------------
    static MonoString* NightSnack_GetMaterialhName(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetMaterialName(id));
    }
    static void NightSnack_SetMaterialName(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }
        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->SetMaterialName(id, std::string{ cStr });
    }

    // isWireframe --------------
    static void NightSnack_GetIsWireframe(uint32_t id, bool* outWireframe) {
        if (!g_scriptInterface || !outWireframe) { return; }
        *outWireframe = g_scriptInterface->GetIsWireframe(id);
    }
    static void NightSnack_SetIsWireframe(uint32_t id, bool* wireframe) {
        if (!g_scriptInterface || !wireframe) { return; }
        g_scriptInterface->SetIsWireframe(id, *wireframe);
    }
#pragma endregion


    // --------------------------------------------------------------

#pragma region Key/Buttons
    // Input - Is Key Pressed (just pressed this frame)
    static bool NightSnack_IsKeyPressed(int keyCode) {
        if (!g_scriptInterface) { return false; }
        return g_scriptInterface->IsKeyPressed(keyCode);
    }

    // Input - Is Key Down (held down)
    static bool NightSnack_IsKeyDown(int keyCode) {
        if (!g_scriptInterface) { return false; }
        return g_scriptInterface->IsKeyDown(keyCode);
    }

    // Input - Is Key Released
    static bool NightSnack_IsKeyReleased(int keyCode) {
        if (!g_scriptInterface) { return false; }
        return g_scriptInterface->IsKeyReleased(keyCode);
    }
#pragma endregion //Key/Buttons

    // --------------------------------------------------------------


    // Printing
    static void NightSnack_Log(MonoString* msg) {
        if (!g_scriptInterface) { return; }

        char* str = mono_string_to_utf8(msg);
        g_scriptInterface->Log(std::string{ str });
        mono_free(str);
    }

    // Audio - Play Sound
    static void NightSnack_PlaySound(MonoString* soundPath) {
        if (!g_scriptInterface) { return; }

        char* cStr = mono_string_to_utf8(soundPath);
        g_scriptInterface->PlaySound(std::string(cStr));
        mono_free(cStr);
    }

    // Time - Get Delta Time
    static float NightSnack_GetDeltaTime() {
        if (!g_scriptInterface) { return 0.0f; }
        return g_scriptInterface->GetDeltaTime();
    }

    //Create Entity
    static uint32_t NightSnack_CreateEntityArchetype(MonoString* archeType, NightSnack::Scripting::vec3* pos) {
        if (!g_scriptInterface) { return 0; }
        char* cStr = mono_string_to_utf8(archeType);
        return g_scriptInterface->CreateEntityArchetype(std::string(cStr), *pos);
    }
    // --------------------------------------------------------------


}	// extern "C"

namespace NightSnack::Scripting {

    void RegisterMonoInternalCalls(ScriptInterface* scriptInterface) {
        std::cout << "[Mono] Registering internal calls..." << std::endl;

        // Store global reference
        g_scriptInterface = scriptInterface;

        // Register all internal calls
        // Format: "Namespace.ClassName::MethodName"
#pragma region Transform
        mono_add_internal_call("ManagedScripts.Engine::GetPosition_Internal", (void*)NightSnack_GetPosition);
        mono_add_internal_call("ManagedScripts.Engine::SetPosition_Internal", (void*)NightSnack_SetPosition);

        mono_add_internal_call("ManagedScripts.Engine::GetRotation_Internal", (void*)NightSnack_GetRotation);
        mono_add_internal_call("ManagedScripts.Engine::SetRotation_Internal", (void*)NightSnack_SetRotation);

        mono_add_internal_call("ManagedScripts.Engine::GetScale_Internal", (void*)NightSnack_GetScale);
        mono_add_internal_call("ManagedScripts.Engine::SetScale_Internal", (void*)NightSnack_SetScale);

        mono_add_internal_call("ManagedScripts.Engine::GetLayer_Internal", (void*)NightSnack_GetLayer);
        mono_add_internal_call("ManagedScripts.Engine::SetLayer_Internal", (void*)NightSnack_SetLayer);
#pragma endregion

#pragma region Rigidbody
#pragma region Force
        mono_add_internal_call("ManagedScripts.Engine::GetForceDirection_Internal", (void*)NightSnack_GetForceDirection);
        mono_add_internal_call("ManagedScripts.Engine::SetForceDirection_Internal", (void*)NightSnack_SetForceDirection);

        mono_add_internal_call("ManagedScripts.Engine::GetForceMagnitude_Internal", (void*)NightSnack_GetForceMagnitude);
        mono_add_internal_call("ManagedScripts.Engine::SetForceMagnitude_Internal", (void*)NightSnack_SetForceMagnitude);

        mono_add_internal_call("ManagedScripts.Engine::GetForceLifetime_Internal", (void*)NightSnack_GetForceLifetime);
        mono_add_internal_call("ManagedScripts.Engine::SetForceLifetime_Internal", (void*)NightSnack_SetForceLifetime);

        mono_add_internal_call("ManagedScripts.Engine::GetForceActive_Internal", (void*)NightSnack_GetForceActive);
        mono_add_internal_call("ManagedScripts.Engine::SetForceActive_Internal", (void*)NightSnack_SetForceActive);
#pragma endregion //Force
        
        //Velocity
        mono_add_internal_call("ManagedScripts.Engine::GetVelocity_Internal", (void*)NightSnack_GetVelocity);
        mono_add_internal_call("ManagedScripts.Engine::SetVelocity_Internal", (void*)NightSnack_SetVelocity);

        //Acceleration
        mono_add_internal_call("ManagedScripts.Engine::GetAcceleration_Internal", (void*)NightSnack_GetAcceleration);
        mono_add_internal_call("ManagedScripts.Engine::SetAcceleration_Internal", (void*)NightSnack_SetAcceleration);

        //InvMass
        mono_add_internal_call("ManagedScripts.Engine::GetInvMass_Internal", (void*)NightSnack_GetInvMass);
        mono_add_internal_call("ManagedScripts.Engine::SetInvMass_Internal", (void*)NightSnack_SetInvMass);

        //InvInertia
        mono_add_internal_call("ManagedScripts.Engine::GetInvInertia_Internal", (void*)NightSnack_GetInvInertia);
        mono_add_internal_call("ManagedScripts.Engine::SetInvInertia_Internal", (void*)NightSnack_SetInvInertia);

        //Restitution
        mono_add_internal_call("ManagedScripts.Engine::GetRestitution_Internal", (void*)NightSnack_GetRestitution);
        mono_add_internal_call("ManagedScripts.Engine::SetRestitution_Internal", (void*)NightSnack_SetRestitution);
        
        //GravityMult
        mono_add_internal_call("ManagedScripts.Engine::GetGravityMult_Internal", (void*)NightSnack_GetGravityMult);
        mono_add_internal_call("ManagedScripts.Engine::SetGravityMult_Internal", (void*)NightSnack_SetGravityMult);

        //UseGravity
        mono_add_internal_call("ManagedScripts.Engine::GetUseGravity_Internal", (void*)NightSnack_GetUseGravity);
        mono_add_internal_call("ManagedScripts.Engine::SetUseGravity_Internal", (void*)NightSnack_SetUseGravity);

        //UseRotate
        mono_add_internal_call("ManagedScripts.Engine::GetUseRotate_Internal", (void*)NightSnack_GetUseRotate);
        mono_add_internal_call("ManagedScripts.Engine::SetUseRotate_Internal", (void*)NightSnack_SetUseRotate);


#pragma endregion //RigidBody

#pragma region Collider
        mono_add_internal_call("ManagedScripts.Engine::GetColliderShape_Internal", (void*)NightSnack_GetColliderShape);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderShape_Internal", (void*)NightSnack_SetColliderShape);

        mono_add_internal_call("ManagedScripts.Engine::GetColliderType_Internal", (void*)NightSnack_GetColliderType);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderType_Internal", (void*)NightSnack_SetColliderType);

        mono_add_internal_call("ManagedScripts.Engine::GetColliderStatic_Internal", (void*)NightSnack_GetColliderStatic);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderStatic_Internal", (void*)NightSnack_SetColliderStatic);

        mono_add_internal_call("ManagedScripts.Engine::GetColliderHeight_Internal", (void*)NightSnack_GetColliderHeight);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderHeight_Internal", (void*)NightSnack_SetColliderHeight);

        mono_add_internal_call("ManagedScripts.Engine::GetColliderWidth_Internal", (void*)NightSnack_GetColliderWidth);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderWidth_Internal", (void*)NightSnack_SetColliderWidth);

        mono_add_internal_call("ManagedScripts.Engine::GetColliderRadius_Internal", (void*)NightSnack_GetColliderRadius);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderRadius_Internal", (void*)NightSnack_SetColliderRadius);

        mono_add_internal_call("ManagedScripts.Engine::GetColliderGrounded_Internal", (void*)NightSnack_GetColliderGrounded);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderGrounded_Internal", (void*)NightSnack_SetColliderGrounded);
        
        mono_add_internal_call("ManagedScripts.Engine::GetColliderColliding_Internal", (void*)NightSnack_GetColliderColliding);
        mono_add_internal_call("ManagedScripts.Engine::SetColliderColliding_Internal", (void*)NightSnack_SetColliderColliding);

#pragma endregion

#pragma region Color
        mono_add_internal_call("ManagedScripts.Engine::GetRGBA_Internal", (void*)NightSnack_GetColorRGBA);
        mono_add_internal_call("ManagedScripts.Engine::SetRGBA_Internal", (void*)NightSnack_SetColorRGBA);
#pragma endregion

#pragma region Renderaable
        mono_add_internal_call("ManagedScripts.Engine::GetMeshName_Internal", (void*)NightSnack_GetMeshName);
        mono_add_internal_call("ManagedScripts.Engine::SetMeshName_Internal", (void*)NightSnack_SetMeshName);

        mono_add_internal_call("ManagedScripts.Engine::GetMaterialName_Internal", (void*)NightSnack_GetMaterialhName);
        mono_add_internal_call("ManagedScripts.Engine::SetMaterialName_Internal", (void*)NightSnack_SetMaterialName);

        mono_add_internal_call("ManagedScripts.Engine::GetIsWireframe_Internal", (void*)NightSnack_GetIsWireframe);
        mono_add_internal_call("ManagedScripts.Engine::SetIsWireframe_Internal", (void*)NightSnack_SetIsWireframe);
#pragma endregion


#pragma region Key/Inputs
        mono_add_internal_call("ManagedScripts.Engine::IsKeyPressed_Internal", (void*)NightSnack_IsKeyPressed);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyDown_Internal", (void*)NightSnack_IsKeyDown);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyReleased_Internal", (void*)NightSnack_IsKeyReleased);
#pragma endregion

        mono_add_internal_call("ManagedScripts.Engine::Log_Internal", (void*)NightSnack_Log);
        mono_add_internal_call("ManagedScripts.Engine::PlaySound_Internal", (void*)NightSnack_PlaySound);
        mono_add_internal_call("ManagedScripts.Engine::GetDeltaTime_Internal", (void*)NightSnack_GetDeltaTime);

        mono_add_internal_call("ManagedScripts.Engine::CreateEntityArchetype_Internal", (void*)NightSnack_CreateEntityArchetype);


        std::cout << "[Mono] Internal calls registered successfully" << std::endl;
    }

} // NightSnack::Scripting
