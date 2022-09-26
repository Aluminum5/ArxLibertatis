/*
 * Copyright 2014-2022 Arx Libertatis Team (see the AUTHORS file)
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

#ifndef ARX_GAME_MAGIC_SPELLS_SPELLSLVL05_H
#define ARX_GAME_MAGIC_SPELLS_SPELLSLVL05_H

#include <memory>

#include "game/magic/Spell.h"
#include "graphics/spells/Spells05.h"
#include "math/Quantizer.h"
#include "platform/Platform.h"


class RuneOfGuardingSpell final : public Spell {
	
public:
	
	RuneOfGuardingSpell();
	
	void Launch() override;
	void End() override;
	void Update() override;
	
	Vec3f getPosition() const override;
	
private:
	
	Vec3f m_pos;
	LightHandle m_light;
	
	TextureContainer * tex_p2;
	
};

class LevitateSpell final : public Spell {
	
public:
	
	LevitateSpell();
	
	void Launch() override;
	void End() override;
	void Update() override;
	
private:
	
	Vec3f m_pos;
	float m_baseRadius;
	RotatingCone cone1;
	RotatingCone cone2;
	FloatingStones m_stones;
	
	void createDustParticle();
	
};

class CurePoisonSpell final : public Spell {
	
public:
	
	CurePoisonSpell();
	
	void Launch() override;
	void End() override;
	void Update() override;
	
private:
	
	Vec3f m_pos;
	LightHandle m_light;
	ParticleSystem m_particles;
	
};

class RepelUndeadSpell final : public Spell {
	
public:
	
	RepelUndeadSpell();
	
	void Launch() override;
	void End() override;
	void Update() override;
	
private:
	
	LightHandle m_light;
	Vec3f m_pos;
	float m_yaw;
	TextureContainer * tex_p2;
	math::Quantizer m_quantizer;
	
};

class PoisonProjectileSpell final : public Spell {
	
public:
	
	~PoisonProjectileSpell() override;
	
	void Launch() override;
	void End() override;
	void Update() override;
	
private:
	
	LightHandle lLightId;
	std::vector<std::unique_ptr<CPoisonProjectile>> m_projectiles;
	
};

#endif // ARX_GAME_MAGIC_SPELLS_SPELLSLVL05_H
