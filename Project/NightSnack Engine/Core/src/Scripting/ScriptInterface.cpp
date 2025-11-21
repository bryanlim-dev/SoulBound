/******************************************************************************
 * File:        ScriptInterface.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file defines functions convert native component types to types visible
 * to C# Scripting API.
 * This is the furthest that the ECS Coordinator will be able to reach.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "ScriptInterface.h"

#include "./Systems/AudioSystem.h"
#include "./Systems/Application/Application.h"
#include "./Systems/Application/Input.h"
#include "../Systems/AllSystems.h"

#include <iostream>

namespace NightSnack::Scripting {

    // --------------------------------------------------------------
    // Transform operations
    // --------------------------------------------------------------
#pragma region Transform
    Trans ScriptInterface::GetTransform(Entity e) {

        Transform t = m_coord.GetComponent<Transform>(e);
        Trans result;

        result.pos = vec3(t.pos.x, t.pos.y, t.pos.z);
        result.rot = t.rot;
        result.scale = vec2(t.scale.x, t.scale.y);
        result.layer = t.layer;

        return result;
    }
    void ScriptInterface::SetTransform(Entity e, Trans& t) {

        Transform& trans = m_coord.ModifyComponent<Transform>(e);

        trans.pos = Vec3(t.pos.x, t.pos.y, t.pos.z);
        trans.rot = t.rot;
        trans.scale = Vec2(t.scale.x, t.scale.y);
        trans.layer = t.layer;
    }
    // Position ----
    vec3 ScriptInterface::GetPosition(Entity e) {
        Vec3 position = m_coord.GetComponent<Transform>(e).pos;
        vec3 result = vec3(position.x, position.y, position.z);
        return result;
    }
    void ScriptInterface::SetPosition(Entity e, vec3 pos) {
        Vec3& position = m_coord.ModifyComponent<Transform>(e).pos;
        position = Vec3(pos.x, pos.y, pos.z);
    }

    // Rotation ----
    float ScriptInterface::GetRotation(Entity e) {
        return m_coord.GetComponent<Transform>(e).rot;
    }
    void ScriptInterface::SetRotation(Entity e, float rotate) {
        m_coord.ModifyComponent<Transform>(e).rot = rotate;
    }

    // Scale -------
    vec2 ScriptInterface::GetScale(Entity e) {
        Vec2 scale = m_coord.GetComponent<Transform>(e).scale;
        return vec2(scale.x, scale.y);
    }
    void ScriptInterface::SetScale(Entity e, vec2 scal) {
        Vec2& scale = m_coord.ModifyComponent<Transform>(e).scale;
        scale = Vec2(scal.x, scal.y);
    }

    // Layer -------
    LayerType ScriptInterface::GetLayer(Entity e) {
        LayerType layer = m_coord.GetComponent<Transform>(e).layer;
        return layer;
    }
    void ScriptInterface::SetLayer(Entity e, LayerType lay) {
        LayerType& layer = m_coord.ModifyComponent<Transform>(e).layer;
        layer = lay;
    }
#pragma endregion

    // --------------------------------------------------------------
    // Rigidbody Inputs
    // --------------------------------------------------------------
#pragma region Rigidbody
    // --------------------------------------------------------------
    // Force Inputs
    // --------------------------------------------------------------
#pragma region Force
    Forc ScriptInterface::GetForce(Entity e) {

        Force t = m_coord.GetComponent<RigidBody>(e).force;
        Forc result;

        result.direction = vec2(t.direction.x, t.direction.y);
        result.magnitude = t.magnitude;
        result.isActive = t.isActive;
        result.lifetime = t.lifetime;

        return result;
    }
    void ScriptInterface::SetForce(Entity e, Forc& t) {

        Force& force = m_coord.ModifyComponent<RigidBody>(e).force;

        force.direction = Vec2(t.direction.x, t.direction.y);
        force.magnitude = t.magnitude;
        force.isActive = t.isActive;
        force.lifetime = t.lifetime;
    }

    // Direction ----
    vec2 ScriptInterface::GetForceDirection(Entity e) {
        Vec2 direction = m_coord.GetComponent<RigidBody>(e).force.direction;
        vec2 result = vec2(direction.x, direction.y);
        return result;
    }
    void ScriptInterface::SetForceDirection(Entity e, vec2 pos) {
        Vec2& direction = m_coord.ModifyComponent<RigidBody>(e).force.direction;
        direction = Vec3(pos.x, pos.y);
    }

    // Magnitude ----
    float ScriptInterface::GetForceMagnitude(Entity e)
    {
        float magnitude = m_coord.GetComponent<RigidBody>(e).force.magnitude;
        return magnitude;
    }
    void ScriptInterface::SetForceMagnitude(Entity e, float mag)
    {
        float& magnitude = m_coord.ModifyComponent<RigidBody>(e).force.magnitude;
        magnitude = mag;
    }

    // Lifetime ----
    float ScriptInterface::GetForceLifetime(Entity e)
    {
        float lifetime = m_coord.GetComponent<RigidBody>(e).force.lifetime;
        return lifetime;
    }
    void ScriptInterface::SetForceLifetime(Entity e, float life)
    {
        float& lifetime = m_coord.ModifyComponent<RigidBody>(e).force.lifetime;
        lifetime = life;
    }

    // Force Active ----
    bool ScriptInterface::GetForceActive(Entity e)
    {
        bool isActive = m_coord.GetComponent<RigidBody> (e).force.isActive;
        return isActive;
    }
    void ScriptInterface::SetForceActive(Entity e, bool active)
    {
        bool& isActive = m_coord.ModifyComponent<RigidBody>(e).force.isActive;
        isActive = active;
    }
#pragma endregion //Force

    RigidBod ScriptInterface::GetRigidBody(Entity e)
    {
        RigidBody rigidBody = m_coord.GetComponent<RigidBody>(e);
        RigidBod body;
        body.force = GetForce(e);
        body.vel = rigidBody.vel;
        body.accel = rigidBody.accel;
        body.invMass = rigidBody.invMass;
        body.invInertia = rigidBody.invInertia;
        body.restitution = rigidBody.restitution;
        body.gravitymult = rigidBody.gravitymult;
        body.useGravity = rigidBody.useGravity;
        body.useRotate = rigidBody.useRotate;
        

        return body;
    }
    void ScriptInterface::SetRigidBody(Entity e, RigidBod& bod)
    {
        RigidBody& rigidBody = m_coord.ModifyComponent<RigidBody>(e);
        SetForce(e, bod.force);

        rigidBody.vel = bod.vel;
        rigidBody.accel = bod.accel;
        rigidBody.invMass = bod.invMass;
        rigidBody.invInertia = bod.invInertia;
        rigidBody.restitution = bod.restitution;
        rigidBody.gravitymult = bod.gravitymult;
        rigidBody.useGravity = bod.useGravity;
        rigidBody.useRotate = bod.useRotate;
    }

    // Velocity ----
    vec2 ScriptInterface::GetVelocity(Entity e) {
        Vec2 vel = m_coord.GetComponent<RigidBody>(e).vel;
        vec2 result = vec2(vel.x, vel.y);
        return result;
    }
    void ScriptInterface::SetVelocity(Entity e, vec2 vel) {
        Vec2& velocity = m_coord.ModifyComponent<RigidBody>(e).vel;
        velocity = Vec2(vel.x, vel.y);
    }

    // Acceleration ----
    vec2 ScriptInterface::GetAcceleration(Entity e) {
        Vec2 acc = m_coord.GetComponent<RigidBody>(e).accel;
        vec2 result = vec2(acc.x, acc.y);
        return result;
    }
    void ScriptInterface::SetAcceleration(Entity e, vec2 accel) {
        Vec2& acceleration = m_coord.ModifyComponent<RigidBody>(e).accel;
        acceleration = Vec2(accel.x, accel.y);
    }

    // InvMass ----
    float ScriptInterface::GetInvMass(Entity e) {
        return m_coord.GetComponent<RigidBody>(e).invMass;
    }
    void ScriptInterface::SetInvMass(Entity e, float invmas) {
        m_coord.ModifyComponent<RigidBody>(e).invMass = invmas;
    }

    // InvInertia ----
    float ScriptInterface::GetInvInertia(Entity e) {
        return m_coord.GetComponent<RigidBody>(e).invInertia;
    }
    void ScriptInterface::SetInvInertia(Entity e, float invIner) {
        m_coord.ModifyComponent<RigidBody>(e).invInertia = invIner;
    }

    // Restitution ----
    float ScriptInterface::GetRestitution(Entity e) {
        return m_coord.GetComponent<RigidBody>(e).restitution;
    }
    void ScriptInterface::SetRestitution(Entity e, float restitution) {
        m_coord.ModifyComponent<RigidBody>(e).restitution = restitution;
    }

    // GravityMult ----
    float ScriptInterface::GetGravityMult(Entity e){
        return m_coord.GetComponent<RigidBody>(e).gravitymult;
    }
    void ScriptInterface::SetGravityMult(Entity e, float gravMul) {
        m_coord.ModifyComponent<RigidBody>(e).gravitymult = gravMul;
    }

    // UseGravity ----
    bool ScriptInterface::GetUseGravity(Entity e) {
        return m_coord.GetComponent<RigidBody>(e).useGravity;
    }
    void ScriptInterface::SetUseGravity(Entity e, bool useGravity) {
        m_coord.ModifyComponent<RigidBody>(e).useGravity = useGravity;
    }

    // UseRotate ----
    bool ScriptInterface::GetUseRotate(Entity e) {
        return m_coord.GetComponent<RigidBody>(e).useRotate;
    }
    void ScriptInterface::SetUseRotate(Entity e, bool useRotate) {
        m_coord.ModifyComponent<RigidBody>(e).useRotate = useRotate;
    }


#pragma endregion //Rigidbody

    // --------------------------------------------------------------
    // Collider Inputs
    // --------------------------------------------------------------
#pragma region Collider

    Collide ScriptInterface::GetCollider(Entity e) {

        Collider t = m_coord.GetComponent<Collider>(e);
        Collide result;

        result.shape = t.shape;
        result.type = t.type;
        result.isStatic = t.isStatic;
        result.width = t.width;
        result.height = t.height;
        result.radius = t.radius;

        return result;
    }

    void ScriptInterface::SetCollider(Entity e, Collide col)
    {
        Collider& t = m_coord.ModifyComponent<Collider>(e);
        t.shape = col.shape;
        t.type = col.type;
        t.isStatic = col.isStatic;
        t.width = col.width;
        t.height = col.height;
        t.radius = col.radius;
    }

    // ShapeType -------
    ShapeType ScriptInterface::GetColliderShape(Entity e) {
        ShapeType shape = m_coord.GetComponent<Collider>(e).shape;
        return shape;
    }
    void ScriptInterface::SetColliderShape(Entity e, ShapeType shap) {
        ShapeType& shape = m_coord.ModifyComponent<Collider>(e).shape;
        shape = shap;
    }

    // ColliderType -------
    ColliderType ScriptInterface::GetColliderType(Entity e) {
        ColliderType type = m_coord.GetComponent<Collider>(e).type;
        return type;
    }
    void ScriptInterface::SetColliderType(Entity e, ColliderType typ) {
        ColliderType& type = m_coord.ModifyComponent<Collider>(e).type;
        type = typ;
    }

    // isStatic ----
    bool ScriptInterface::GetColliderStatic(Entity e)
    {
        return m_coord.GetComponent<Collider>(e).isStatic;
    }
    void ScriptInterface::SetColliderStatic(Entity e, bool isStatic)
    {
        m_coord.ModifyComponent<Collider>(e).isStatic = isStatic;
    }

    // Height ----
    float ScriptInterface::GetColliderHeight(Entity e)
    {
        return m_coord.GetComponent<Collider>(e).height;
    }
    void ScriptInterface::SetColliderHeight(Entity e, float height)
    {
        m_coord.ModifyComponent<Collider>(e).height = height;
    }

    // Width ----
    float ScriptInterface::GetColliderWidth(Entity e)
    {
        return m_coord.GetComponent<Collider>(e).width;
    }
    void ScriptInterface::SetColliderWidth(Entity e, float width)
    {
        m_coord.ModifyComponent<Collider>(e).width = width;
    }

    // Radius ----
    float ScriptInterface::GetColliderRadius(Entity e)
    {
        return m_coord.GetComponent<Collider>(e).radius;
    }
    void ScriptInterface::SetColliderRadius(Entity e, float radius)
    {
        m_coord.ModifyComponent<Collider>(e).radius = radius;
    }

    // isGrounded ----
    bool ScriptInterface::GetColliderGrounded(Entity e)
    {
        return m_coord.GetComponent<Collider>(e).isGrounded;
    }
    void ScriptInterface::SetColliderGrounded(Entity e, bool grounded)
    {
        m_coord.ModifyComponent<Collider>(e).isGrounded = grounded;
    }

    // Colliding ----
    bool ScriptInterface::GetColliderColliding(Entity e)
    {
        return m_coord.GetComponent<Collider>(e).colliding;
    }
    void ScriptInterface::SetColliderColliding(Entity e, bool colliding)
    {
        m_coord.ModifyComponent<Collider>(e).colliding = colliding;
    }


#pragma endregion

    // --------------------------------------------------------------
    // Color Inputs
    // --------------------------------------------------------------
#pragma region Color
    vec4 ScriptInterface::GetColorRGBA(Entity e)
    {
        Color color = m_coord.GetComponent<Color>(e);
        return vec4{ color.r, color.b, color.g, color.a };
    }

    void ScriptInterface::SetColorRGBA(Entity e, vec4 rgba)
    {
        Color& color = m_coord.ModifyComponent<Color>(e);
        color.r = rgba.x;
        color.g = rgba.y;
        color.b = rgba.z;
        color.a = rgba.w;
    }

#pragma endregion

    // --------------------------------------------------------------
    // Renderable Inputs
    // --------------------------------------------------------------
#pragma region Renderable
    Renderabl ScriptInterface::GetRenderable(Entity e) {

        Renderable t = m_coord.GetComponent<Renderable>(e);
        Renderabl result;

        result.meshName = t.meshName;
        result.materialName = t.materialName;
        result.isWireframe = t.isWireframe;
        result.wasModified = t.wasModified;

        return result;
    }

    void ScriptInterface::SetRenderable(Entity e, Renderabl rend)
    {
        Renderable& t = m_coord.ModifyComponent<Renderable>(e);
        t.meshName = rend.meshName;
        t.materialName = rend.materialName;
        t.isWireframe = rend.isWireframe;
        //t.wasModified = rend.wasModified;
    }

    std::string ScriptInterface::GetMeshName(Entity e){
        return m_coord.GetComponent<Renderable>(e).meshName;
    }
    void ScriptInterface::SetMeshName(Entity e, std::string name){
        m_coord.ModifyComponent<Renderable>(e).meshName = name;
    }

    std::string ScriptInterface::GetMaterialName(Entity e){
        return m_coord.GetComponent<Renderable>(e).materialName;
    }
    void ScriptInterface::SetMaterialName(Entity e, std::string name){
        m_coord.GetSystem<GraphicsSystem>()->UpdateTextureOfMaterial(name, e);
    }

    bool ScriptInterface::GetIsWireframe(Entity e)
    {
        return m_coord.GetComponent<Renderable>(e).isWireframe;
    }

    void ScriptInterface::SetIsWireframe(Entity e, bool wire){
        m_coord.ModifyComponent<Renderable>(e).isWireframe = wire;
    }


#pragma endregion

    // --------------------------------------------------------------
    // Key Inputs
    // --------------------------------------------------------------
#pragma region Key/Buttons
    bool ScriptInterface::IsKeyPressed(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyPressed(key);
    }

    bool ScriptInterface::IsKeyDown(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyDown(key);
    }

    bool ScriptInterface::IsKeyReleased(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyReleased(key);
    }
#pragma endregion

    // --------------------------------------------------------------
    // Audio
    // --------------------------------------------------------------
    void ScriptInterface::PlaySound(const std::string& path) {
        //m_coord.GetSystem<AudioSystem>()->PlayAudio(soundPath);
        std::cout << "Playing sound: " << path << '\n';
    }

    // Time
    float ScriptInterface::GetDeltaTime() {
        return FixedDT;
    }

    // Logging
    void ScriptInterface::Log(const std::string& msg) {
        std::cout << "[Script] " << msg << '\n';
    }

    //Create Entity
    Entity ScriptInterface::CreateEntityArchetype(std::string archeTypeName, vec3 pos)
    {
        Entity entity= m_coord.GetSystem<EntityFactory>()->Create(archeTypeName);
        m_coord.ModifyComponent<Transform>(entity).pos = Vec3{ pos.x, pos.x, pos.z };
        return entity;
    }
    // --------------------------------------------------------------

}   // NightSnack::Scripting
