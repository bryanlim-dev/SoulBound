/******************************************************************************
 * File:        MonoInternalCalls.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 30%) - Created base file and Transform/Input/Sound calls & inital C# linkage functions
 *              Clement Ang Yang Ying (Primary, 70%) - Created rest of component/entity function class & linkage, orgnized functions
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

    // UIbasePos --------------
    static void NightSnack_GetUIBasePos(uint32_t id, NightSnack::Scripting::vec3* outpos) {
        if (!g_scriptInterface || !outpos) { return; }
        *outpos = g_scriptInterface->GetUIBasePos(id);
    }
    static void NightSnack_SetUIBasePos(uint32_t id, NightSnack::Scripting::vec3* pos) {
        if (!g_scriptInterface || !pos) { return; }
        g_scriptInterface->SetUIBasePos(id, *pos);
    }

    // UIbaseScale -----------------
    static void NightSnack_GetUIBaseScale(uint32_t id, NightSnack::Scripting::vec2* scale) {
        if (!g_scriptInterface || !scale) { return; }
        *scale = g_scriptInterface->GetUIBaseScale(id);
    }
    static void NightSnack_SetUIBaseScale(uint32_t id, NightSnack::Scripting::vec2* scale) {
        if (!g_scriptInterface || !scale) { return; }
        g_scriptInterface->SetUIBaseScale(id, *scale);
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

#pragma region Animator
    // currentAnimationClip --------------
    static MonoString* NightSnack_GetAnimatorCurrentAnimationClip(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetAnimatorCurrentAnimationClip(id));
    }
    static void NightSnack_SetAnimatorCurrentAnimationClip(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }
        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->SetAnimatorCurrentAnimationClip(id, std::string{ cStr });
    }

    // spriteSheetName --------------
    static MonoString* NightSnack_GetAnimatorSpriteSheetName(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetAnimatorSpriteSheetName(id));
    }
    static void NightSnack_SetAnimatorSpriteSheetName(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }
        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->SetAnimatorSpriteSheetName(id, std::string{ cStr });
    }

    // currentFrameIndex --------------
    static void NightSnack_GetAnimatorCurrentFrameIndex(uint32_t id, int* outindex) {
        if (!g_scriptInterface || !outindex) { return; }
        *outindex = g_scriptInterface->GetAnimatorCurrentFrameIndex(id);
    }
    static void NightSnack_SetAnimatorCurrentFrameIndex(uint32_t id, int* index) {
        if (!g_scriptInterface || !index) { return; }
        g_scriptInterface->SetAnimatorCurrentFrameIndex(id, *index);
    }

    // currentTime --------------
    static void NightSnack_GetAnimatorCurrentTime(uint32_t id, float* outTime) {
        if (!g_scriptInterface || !outTime) { return; }
        *outTime = g_scriptInterface->GetAnimatorCurrentTime(id);
    }
    static void NightSnack_SetAnimatorCurrentTime(uint32_t id, float* time) {
        if (!g_scriptInterface || !time) { return; }
        g_scriptInterface->SetAnimatorCurrentTime(id, *time);
    }

    // playbackSpeed --------------
    static void NightSnack_GetAnimatorPlaybackSpeed(uint32_t id, float* outSpeed) {
        if (!g_scriptInterface || !outSpeed) { return; }
        *outSpeed = g_scriptInterface->GetAnimatorPlaybackSpeed(id);
    }
    static void NightSnack_SetAnimatorPlaybackSpeed(uint32_t id, float* speed) {
        if (!g_scriptInterface || !speed) { return; }
        g_scriptInterface->SetAnimatorPlaybackSpeed(id, *speed);
    }

    // isPlaying --------------
    static void NightSnack_GetAnimatorIsPlaying(uint32_t id, bool* outPlaying) {
        if (!g_scriptInterface || !outPlaying) { return; }
        *outPlaying = g_scriptInterface->GetAnimatorIsPlaying(id);
    }
    static void NightSnack_SetAnimatorIsPlaying(uint32_t id, bool* playing) {
        if (!g_scriptInterface || !playing) { return; }
        g_scriptInterface->SetAnimatorIsPlaying(id, *playing);
    }

    // isLooping --------------
    static void NightSnack_GetAnimatorIsLooping(uint32_t id, bool* outLoop) {
        if (!g_scriptInterface || !outLoop) { return; }
        *outLoop = g_scriptInterface->GetAnimatorIsLooping(id);
    }
    static void NightSnack_SetAnimatorIsLooping(uint32_t id, bool* loop) {
        if (!g_scriptInterface || !loop) { return; }
        g_scriptInterface->SetAnimatorIsLooping(id, *loop);
    }

    // lastFacingRight --------------
    static void NightSnack_GetAnimatorLastFacingRight(uint32_t id, bool* outLastFacingRight) {
        if (!g_scriptInterface || !outLastFacingRight) { return; }
        *outLastFacingRight = g_scriptInterface->GetAnimatorLastFacingRight(id);
    }
    static void NightSnack_SetAnimatorLastFacingRight(uint32_t id, bool* lastFacingRight) {
        if (!g_scriptInterface || !lastFacingRight) { return; }
        g_scriptInterface->SetAnimatorLastFacingRight(id, *lastFacingRight);
    }

    // uvTopLeft -----------------
    static void NightSnack_GetAnimatorUVTopLeft(uint32_t id, NightSnack::Scripting::vec2* outUV) {
        if (!g_scriptInterface || !outUV) { return; }
        *outUV = g_scriptInterface->GetAnimatorUVTopLeft(id);
    }
    static void NightSnack_SetAnimatorUVTopLeft(uint32_t id, NightSnack::Scripting::vec2* uv) {
        if (!g_scriptInterface || !uv) { return; }
        g_scriptInterface->SetAnimatorUVTopLeft(id, *uv);
    }

    // uvBottomRight -----------------
    static void NightSnack_GetAnimatorUVBottomRight(uint32_t id, NightSnack::Scripting::vec2* outUV) {
        if (!g_scriptInterface || !outUV) { return; }
        *outUV = g_scriptInterface->GetAnimatorUVBottomRight(id);
    }
    static void NightSnack_SetAnimatorUVBottomRight(uint32_t id, NightSnack::Scripting::vec2* uv) {
        if (!g_scriptInterface || !uv) { return; }
        g_scriptInterface->SetAnimatorUVBottomRight(id, *uv);
    }
#pragma endregion

    // --------------------------------------------------------------
    
#pragma region Font

    // fontType --------------
    static MonoString* NightSnack_GetFontType(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetFontType(id));
    }
    static void NightSnack_SetFontType(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }
        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->SetFontType(id, std::string{ cStr });
    }

    // text --------------
    static MonoString* NightSnack_GetFontText(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetFontText(id));
    }
    static void NightSnack_SetFontText(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }
        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->SetFontText(id, std::string{ cStr });
    }

    // fontSize --------------
    static void NightSnack_GetFontSize(uint32_t id, float* outSize) {
        if (!g_scriptInterface || !outSize) { return; }
        *outSize = g_scriptInterface->GetFontSize(id);
    }
    static void NightSnack_SetFontSize(uint32_t id, float* size) {
        if (!g_scriptInterface || !size) { return; }
        g_scriptInterface->SetFontSize(id, *size);
    }	
    
    // textColor --------------
    static void NightSnack_GetFontTextColor(uint32_t id, NightSnack::Scripting::vec3* outCol) {
        if (!g_scriptInterface || !outCol) { return; }
        *outCol = g_scriptInterface->GetFontTextColor(id);
    }
    static void NightSnack_SetFontTextColor(uint32_t id, NightSnack::Scripting::vec3* col) {
        if (!g_scriptInterface || !col) { return; }
        g_scriptInterface->SetFontTextColor(id, *col);
    }

    // offsetPosition --------------
    static void NightSnack_GetFontOffsetPosition(uint32_t id, NightSnack::Scripting::vec3* outpos) {
        if (!g_scriptInterface || !outpos) { return; }
        *outpos = g_scriptInterface->GetFontOffsetPosition(id);
    }
    static void NightSnack_FontOffsetPosition(uint32_t id, NightSnack::Scripting::vec3* pos) {
        if (!g_scriptInterface || !pos) { return; }
        g_scriptInterface->SetFontOffsetPosition(id, *pos);
    }
#pragma endregion

    // --------------------------------------------------------------
    
#pragma region Camera
    // isMainCam --------------
    static void NightSnack_GetCameraIsMain(uint32_t id, bool* outMain) {
        if (!g_scriptInterface || !outMain) { return; }
        *outMain = g_scriptInterface->GetCameraIsMain(id);
    }
    static void NightSnack_SetCameraIsMain(uint32_t id, bool* main) {
        if (!g_scriptInterface || !main) { return; }
        g_scriptInterface->SetCameraIsMain(id, *main);
    }

    // zoom --------------
    static void NightSnack_GetCameraZoom(uint32_t id, float* outZoom) {
        if (!g_scriptInterface || !outZoom) { return; }
        *outZoom = g_scriptInterface->GetCameraZoom(id);
    }
    static void NightSnack_SetCameraZoom(uint32_t id, float* zoom) {
        if (!g_scriptInterface || !zoom) { return; }
        g_scriptInterface->SetCameraZoom(id, *zoom);
    }

    // width --------------
    static void NightSnack_GetCameraWidth(uint32_t id, float* outWidth) {
        if (!g_scriptInterface || !outWidth) { return; }
        *outWidth = g_scriptInterface->GetCameraWidth(id);
    }
    static void NightSnack_SetCameraWidth(uint32_t id, float* width) {
        if (!g_scriptInterface || !width) { return; }
        g_scriptInterface->SetCameraWidth(id, *width);
    }

    // height --------------
    static void NightSnack_GetCameraHeight(uint32_t id, float* outHeight) {
        if (!g_scriptInterface || !outHeight) { return; }
        *outHeight = g_scriptInterface->GetCameraHeight(id);
    }
    static void NightSnack_SetCameraHeight(uint32_t id, float* height) {
        if (!g_scriptInterface || !height) { return; }
        g_scriptInterface->SetCameraHeight(id, *height);
    }

#pragma endregion
    // --------------------------------------------------------------

#pragma region Audio
    // audiokey --------------
    static MonoString* NightSnack_GetAudioKey(uint32_t id) {
        if (!g_scriptInterface) { return NULL; }
        return NightSnack::Scripting::StringToMonoString(g_scriptInterface->GetAudioKey(id));
    }
    static void NightSnack_SetAudioKey(uint32_t id, MonoString* key) {
        if (!g_scriptInterface || !key) { return; }
        char* cStr = mono_string_to_utf8(key);
        g_scriptInterface->SetAudioKey(id, std::string{ cStr });
    }

    // volume --------------
    static void NightSnack_GetAudioVolume(uint32_t id, float* outVolume) {
        if (!g_scriptInterface || !outVolume) { return; }
        *outVolume = g_scriptInterface->GetAudioVolume(id);
    }
    static void NightSnack_SetAudioVolume(uint32_t id, float* volume) {
        if (!g_scriptInterface || !volume) { return; }
        g_scriptInterface->SetAudioVolume(id, *volume);
    }

    // loop --------------
    static void NightSnack_GetAudioLoop(uint32_t id, bool* outLooping) {
        if (!g_scriptInterface || !outLooping) { return; }
        *outLooping = g_scriptInterface->GetAudioLoop(id);
    }
    static void NightSnack_SetAudioLoop(uint32_t id, bool* looping) {
        if (!g_scriptInterface || !looping) { return; }
        g_scriptInterface->SetAudioLoop(id, *looping);
	}

    // playOnStart --------------
    static void NightSnack_GetAudioPlayOnStart(uint32_t id, bool* outPlayOnStart) {
        if (!g_scriptInterface || !outPlayOnStart) { return; }
        *outPlayOnStart = g_scriptInterface->GetAudioPlayOnStart(id);
    }
    static void NightSnack_SetAudioPlayOnStart(uint32_t id, bool* playOnStart) {
        if (!g_scriptInterface || !playOnStart) { return; }
        g_scriptInterface->SetAudioPlayOnStart(id, *playOnStart);
	}



    // isPlaying --------------
    static void NightSnack_GetAudioIsPlaying(uint32_t id, bool* outIsPlayer) {
        if (!g_scriptInterface || !outIsPlayer) { return; }
        *outIsPlayer = g_scriptInterface->GetAudioIsPlaying(id);
    }
    static void NightSnack_SetAudioIsPlaying(uint32_t id, bool* isPlaying) {
        if (!g_scriptInterface || !isPlaying) { return; }
        g_scriptInterface->SetAudioIsPlaying(id, *isPlaying);
	}

#pragma endregion

    // --------------------------------------------------------------
    
#pragma region Scripting
    static void NightSnack_HasScript(uint32_t id, MonoString* name, bool* outExists) {
        if (!g_scriptInterface || !name || !outExists) { return; }
        char* cStr = mono_string_to_utf8(name);

        *outExists = g_scriptInterface->HasScript(id, cStr);
    }

    static void NightSnack_CreateScript(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }

        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->CreateScript(id, cStr);
    }

    static void NightSnack_DeleteScript(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return; }

        char* cStr = mono_string_to_utf8(name);
        g_scriptInterface->DeleteScript(id, cStr);
    }

    static MonoObject* NightSnack_GetScript(uint32_t id, MonoString* name) {
        if (!g_scriptInterface || !name) { return nullptr; }

        char* cStr = mono_string_to_utf8(name);
        return g_scriptInterface->GetScript(id, cStr);
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

    //GetComponent
    static uint32_t NightSnack_GetComponentInChildren(uint32_t id, MonoString* componentType) {
        if (!g_scriptInterface) { return 0; }
        char* cStr = mono_string_to_utf8(componentType);
        return g_scriptInterface->GetComponentInChildren(id, std::string(cStr));
    }
    static uint32_t NightSnack_GetComponentInParent(uint32_t id, MonoString* componentType) {
        if (!g_scriptInterface) { return 0; }
        char* cStr = mono_string_to_utf8(componentType);
        return g_scriptInterface->GetComponentInParent(id, std::string(cStr));
    }

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

    static uint32_t NightSnack_GetPlayerID() {
        if (!g_scriptInterface) { return 0; }
        return g_scriptInterface->GetPlayerID();
    }


    static void NightSnack_LoadLevel(MonoString* lvl)
    {
        if (!g_scriptInterface) { return; }
        char* cStr = mono_string_to_utf8(lvl);
        g_scriptInterface->LoadLevel(std::string(cStr));
    }

    static void NightSnack_RestartLevel()
    {
        if (!g_scriptInterface) { return; }
        g_scriptInterface->RestartLevel();
    }



    // --------------------------------------------------------------


}	// extern "C"

namespace NightSnack::Scripting {

    void RegisterMonoInternalCalls(ScriptInterface* scriptInterface) {
        //std::cout << "[Mono] Registering internal calls..." << std::endl;

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

        mono_add_internal_call("ManagedScripts.Engine::GetUIBasePos_Internal", (void*)NightSnack_GetUIBasePos);
        mono_add_internal_call("ManagedScripts.Engine::SetUIBasePos_Internal", (void*)NightSnack_SetUIBasePos);

        mono_add_internal_call("ManagedScripts.Engine::GetUIBaseScale_Internal", (void*)NightSnack_GetUIBaseScale);
        mono_add_internal_call("ManagedScripts.Engine::SetUIBaseScale_Internal", (void*)NightSnack_SetUIBaseScale);

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

#pragma region Animator
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorCurrentClip_Internal", (void*)NightSnack_GetAnimatorCurrentAnimationClip);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorCurrentClip_Internal", (void*)NightSnack_SetAnimatorCurrentAnimationClip);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorSpriteSheet_Internal", (void*)NightSnack_GetAnimatorSpriteSheetName);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorSpriteSheet_Internal", (void*)NightSnack_SetAnimatorSpriteSheetName);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorCurrentFrameIndex_Internal", (void*)NightSnack_GetAnimatorCurrentFrameIndex);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorCurrentFrameIndex_Internal", (void*)NightSnack_SetAnimatorCurrentFrameIndex);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorCurrentTime_Internal", (void*)NightSnack_GetAnimatorCurrentTime);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorCurrentTime_Internal", (void*)NightSnack_SetAnimatorCurrentTime);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorCurrentPlaybackSpeed_Internal", (void*)NightSnack_GetAnimatorPlaybackSpeed);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorCurrentPlaybackSpeed_Internal", (void*)NightSnack_SetAnimatorPlaybackSpeed);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorIsPlaying_Internal", (void*)NightSnack_GetAnimatorIsPlaying);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorIsPlaying_Internal", (void*)NightSnack_SetAnimatorIsPlaying);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorIsLooping_Internal", (void*)NightSnack_GetAnimatorIsLooping);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorIsLooping_Internal", (void*)NightSnack_SetAnimatorIsLooping);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorLastFacingRight_Internal", (void*)NightSnack_GetAnimatorLastFacingRight);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorLastFacingRight_Internal", (void*)NightSnack_SetAnimatorLastFacingRight);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorUVTopLeft_Internal", (void*)NightSnack_GetAnimatorUVTopLeft);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorUVTopLeft_Internal", (void*)NightSnack_SetAnimatorUVTopLeft);
        
        mono_add_internal_call("ManagedScripts.Engine::GetAnimatorUVBottomRight_Internal", (void*)NightSnack_GetAnimatorUVBottomRight);
        mono_add_internal_call("ManagedScripts.Engine::SetAnimatorUVBottomRight_Internal", (void*)NightSnack_SetAnimatorUVBottomRight);

#pragma endregion

#pragma region Fonts
        //fontType
        mono_add_internal_call("ManagedScripts.Engine::GetFontType_Internal", (void*)NightSnack_GetFontType);
        mono_add_internal_call("ManagedScripts.Engine::SetFontType_Internal", (void*)NightSnack_SetFontType);

        mono_add_internal_call("ManagedScripts.Engine::GetFontText_Internal", (void*)NightSnack_GetFontText);
        mono_add_internal_call("ManagedScripts.Engine::SetFontText_Internal", (void*)NightSnack_SetFontText);

        mono_add_internal_call("ManagedScripts.Engine::GetFontSize_Internal", (void*)NightSnack_GetFontSize);
        mono_add_internal_call("ManagedScripts.Engine::SetFontSize_Internal", (void*)NightSnack_SetFontSize);

        mono_add_internal_call("ManagedScripts.Engine::GetFontColor_Internal", (void*)NightSnack_GetFontTextColor);
        mono_add_internal_call("ManagedScripts.Engine::SetFontColor_Internal", (void*)NightSnack_SetFontTextColor);

        mono_add_internal_call("ManagedScripts.Engine::GetFontOffsetPosition_Internal", (void*)NightSnack_GetFontOffsetPosition);
        mono_add_internal_call("ManagedScripts.Engine::SetFontOffsetPosition_Internal", (void*)NightSnack_FontOffsetPosition);

#pragma endregion

#pragma region Camera

        mono_add_internal_call("ManagedScripts.Engine::GetCameraIsMain_Internal", (void*)NightSnack_GetCameraIsMain);
        mono_add_internal_call("ManagedScripts.Engine::SetCameraIsMain_Internal", (void*)NightSnack_SetCameraIsMain);

        mono_add_internal_call("ManagedScripts.Engine::GetCameraZoom_Internal", (void*)NightSnack_GetCameraZoom);
        mono_add_internal_call("ManagedScripts.Engine::SetCameraZoom_Internal", (void*)NightSnack_SetCameraZoom);

        mono_add_internal_call("ManagedScripts.Engine::GetCameraWidth_Internal", (void*)NightSnack_GetCameraWidth);
        mono_add_internal_call("ManagedScripts.Engine::SetCameraWidth_Internal", (void*)NightSnack_SetCameraWidth);

        mono_add_internal_call("ManagedScripts.Engine::GetCameraHeight_Internal", (void*)NightSnack_GetCameraHeight);
        mono_add_internal_call("ManagedScripts.Engine::SetCameraHeight_Internal", (void*)NightSnack_SetCameraHeight);

#pragma endregion

#pragma region Audio
        mono_add_internal_call("ManagedScripts.Engine::GetAudioKey_Internal", (void*)NightSnack_GetAudioKey);
        mono_add_internal_call("ManagedScripts.Engine::SetAudioKey_Internal", (void*)NightSnack_SetAudioKey);

        mono_add_internal_call("ManagedScripts.Engine::GetAudioVolume_Internal", (void*)NightSnack_GetAudioVolume);
        mono_add_internal_call("ManagedScripts.Engine::SetAudioVolume_Internal", (void*)NightSnack_SetAudioVolume);

        mono_add_internal_call("ManagedScripts.Engine::GetAudioLoop_Internal", (void*)NightSnack_GetAudioLoop);
        mono_add_internal_call("ManagedScripts.Engine::SetAudioLoop_Internal", (void*)NightSnack_SetAudioLoop);

        mono_add_internal_call("ManagedScripts.Engine::GetAudioPlayOnStart_Internal", (void*)NightSnack_GetAudioPlayOnStart);
        mono_add_internal_call("ManagedScripts.Engine::SetAudioPlayOnStart_Internal", (void*)NightSnack_SetAudioPlayOnStart);

        mono_add_internal_call("ManagedScripts.Engine::GetAudioIsPlaying_Internal", (void*)NightSnack_GetAudioIsPlaying);
        mono_add_internal_call("ManagedScripts.Engine::SetAudioIsPlaying_Internal", (void*)NightSnack_SetAudioIsPlaying);
#pragma endregion


#pragma region Scripting
        mono_add_internal_call("ManagedScripts.Engine::HasScript_Internal", (void*)NightSnack_HasScript);
        mono_add_internal_call("ManagedScripts.Engine::CreateScript_Internal", (void*)NightSnack_CreateScript);
        mono_add_internal_call("ManagedScripts.Engine::DeleteScript_Internal", (void*)NightSnack_DeleteScript);
        mono_add_internal_call("ManagedScripts.Engine::GetScript_Internal", (void*)NightSnack_GetScript);
#pragma endregion




#pragma region Key/Inputs
        mono_add_internal_call("ManagedScripts.Engine::IsKeyPressed_Internal", (void*)NightSnack_IsKeyPressed);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyDown_Internal", (void*)NightSnack_IsKeyDown);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyReleased_Internal", (void*)NightSnack_IsKeyReleased);
#pragma endregion

        mono_add_internal_call("ManagedScripts.Engine::GetComponentInChildren_Internal", (void*)NightSnack_GetComponentInChildren);
        mono_add_internal_call("ManagedScripts.Engine::GetComponentInParent_Internal", (void*)NightSnack_GetComponentInParent);

        mono_add_internal_call("ManagedScripts.Engine::Log_Internal", (void*)NightSnack_Log);
        mono_add_internal_call("ManagedScripts.Engine::PlaySound_Internal", (void*)NightSnack_PlaySound);
        mono_add_internal_call("ManagedScripts.Engine::GetDeltaTime_Internal", (void*)NightSnack_GetDeltaTime);

        mono_add_internal_call("ManagedScripts.Engine::CreateEntityArchetype_Internal", (void*)NightSnack_CreateEntityArchetype);
        mono_add_internal_call("ManagedScripts.Engine::GetPlayerID_Internal", (void*)NightSnack_GetPlayerID);

        mono_add_internal_call("ManagedScripts.Engine::LoadLevel_Internal", (void*)NightSnack_LoadLevel);
        mono_add_internal_call("ManagedScripts.Engine::RestartLevel_Internal", (void*)NightSnack_RestartLevel);

        //std::cout << "[Mono] Internal calls registered successfully" << std::endl;
    }

} // NightSnack::Scripting
