/*
 * Copyright 2011-2021 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */
/* Based on:
===========================================================================
ARX FATALIS GPL Source Code
Copyright (C) 1999-2010 Arkane Studios SA, a ZeniMax Media company.

This file is part of the Arx Fatalis GPL Source Code ('Arx Fatalis Source Code'). 

Arx Fatalis Source Code is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Arx Fatalis Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Arx Fatalis Source Code.  If not, see 
<http://www.gnu.org/licenses/>.

In addition, the Arx Fatalis Source Code is also subject to certain additional terms. You should have received a copy of these 
additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Arx 
Fatalis Source Code. If not, please request a copy in writing from Arkane Studios at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing Arkane Studios, c/o 
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
// Code: Cyril Meynier
//
// Copyright (c) 1999-2000 ARKANE Studios SA. All rights reserved

#ifndef ARX_GRAPHICS_PARTICLE_PARTICLEEFFECTS_H
#define ARX_GRAPHICS_PARTICLE_PARTICLEEFFECTS_H

#include <stddef.h>
#include <type_traits>
#include <vector>

#include "core/TimeTypes.h"
#include "game/GameTypes.h"
#include "graphics/Color.h"
#include "graphics/Vertex.h"
#include "math/Types.h"
#include "math/Vector.h"
#include "math/Angle.h"
#include "util/Flags.h"


struct EERIEPOLY;
struct Sphere;
class TextureContainer;
class Entity;
struct EERIE_3DOBJ;

enum ParticlesTypeFlag {
	FIRE_TO_SMOKE       = 1 << 0,
	ROTATING            = 1 << 1,
	FADE_IN_AND_OUT     = 1 << 2,
	DISSIPATING         = 1 << 4,
	GRAVITY             = 1 << 5,
	SUBSTRACT           = 1 << 6,
	DELAY_FOLLOW_SOURCE = 1 << 11,
	PARTICLE_ANIMATED   = 1 << 13,
	SPLAT_GROUND        = 1 << 15,
	SPLAT_WATER         = 1 << 16,
	PARTICLE_SUB2       = 1 << 17,
	PARTICLE_GOLDRAIN   = 1 << 18,
	PARTICLE_NOZBUFFER  = 1 << 19,
	PARTICLE_ZDEC       = 1 << 20,
	PARTICLE_2D         = 1 << 21,
};

DECLARE_FLAGS(ParticlesTypeFlag, ParticlesTypeFlags)
DECLARE_FLAGS_OPERATORS(ParticlesTypeFlags)

struct alignas(16) PARTICLE_DEF {
	
	Vec3f ov;
	float size;
	Vec3f move;
	float sizeDelta;
	Color3f rgb;
	ParticlesTypeFlags m_flags;
	ShortGameDuration elapsed;
	ShortGameDuration duration;
	TextureContainer * tc;
	Vec3f * source;
	EntityHandle sourceionum;
	float m_rotation;
	bool exist;
	
	PARTICLE_DEF()
		: ov(0.f)
		, size(0.f)
		, move(0.f)
		, sizeDelta(0.f)
		, rgb(Color3f::black)
		, m_flags(0)
		, elapsed(0)
		, duration(0)
		, tc(nullptr)
		, source(nullptr)
		, m_rotation(0.f)
		, exist(false)
	{ }
	
};

static_assert(std::is_trivially_copyable_v<PARTICLE_DEF>);

//-----------------------------------------------------------------------------

extern long NewSpell;

void MagFX(const Vec3f & pos, float size);
void RestoreAllLightsInitialStatus();
void TreatBackgroundActions();

void Add3DBoom(const Vec3f & position);
void AddRandomSmoke(const Entity & io, long amount);

void ManageTorch();

void MakePlayerAppearsFX(const Entity & io);
void MakeCoolFx(const Vec3f & pos);

PARTICLE_DEF * createParticle(bool allocateWhilePaused = false);
long getParticleCount();

void ARX_PARTICLES_ClearAll();
void ARX_PARTICLES_Update();
void ARX_PARTICLES_Spawn_Blood(const Vec3f & pos, float dmgs, EntityHandle source);
void ARX_PARTICLES_Spawn_Blood2(const Vec3f & pos, float dmgs, Color col, Entity * io);
void ARX_PARTICLES_Spawn_Lava_Burn(Vec3f pos, Entity * io = nullptr);
void ARX_PARTICLES_Add_Smoke(const Vec3f & pos, long flags, long amount, const Color3f & rgb = Color3f(0.3f, 0.3f, 0.34f)); // flag 1 = randomize pos

void ARX_PARTICLES_Spawn_Splat(const Vec3f & pos, float dmgs, Color col);
void ARX_PARTICLES_SpawnWaterSplash(const Vec3f & pos);

void createFireParticles(Vec3f pos, int perPos, ShortGameDuration delay);

void createObjFireParticles(const EERIE_3DOBJ * obj, int particlePositions, int perPos,
                            ShortGameDuration delay);

void LaunchFireballBoom(const Vec3f & poss, float level, Vec3f * direction = nullptr,
                        const Color3f * rgb = nullptr);
void spawnFireHitParticle(const Vec3f & poss, long type);
void spawn2DFireParticle(const Vec2f & pos, float scale);

void SpawnFireballTail(const Vec3f & poss, const Vec3f & vecto, float level, long flags);

#endif // ARX_GRAPHICS_PARTICLE_PARTICLEEFFECTS_H
