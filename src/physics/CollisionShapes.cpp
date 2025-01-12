/*
 * Copyright 2011-2019 Arx Libertatis Team (see the AUTHORS file)
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
// Copyright (c) 1999-2001 ARKANE Studios SA. All rights reserved

#include "physics/CollisionShapes.h"

#include "game/Entity.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Math.h"


void EERIE_COLLISION_Cylinder_Create(Entity * io)
{
	if(!io)
		return;

	EERIE_3DOBJ * obj = io->obj;

	if(!obj)
		return;

	if(obj->vertexlist.empty()) {
		io->physics.cyl.height = 0.f;
		return;
	}
	
	const EERIE_VERTEX & origin = obj->vertexlist[obj->origin];
	
	io->physics.cyl.origin = origin.v;
	
	float d = 0.f;
	float height = 0.f;
	for(const EERIE_VERTEX & vertex : obj->vertexlist) {
		if(&vertex != &origin && glm::abs(io->physics.cyl.origin.y - vertex.v.y) < 20.f) {
			d = std::max(d, glm::distance(io->physics.cyl.origin, vertex.v));
		}
		height = std::max(height, io->physics.cyl.origin.y - vertex.v.y);
	}

	if(d == 0.f || height == 0.f) {
		io->physics.cyl.height = 0.f;
		return;
	}
	
	io->original_radius = d * 1.2f;
	io->original_height = -height;
	io->physics.cyl.origin = io->pos;
	
	if(io->original_height > -40) {
		float v = (-io->original_height) * (1.0f / 40);
		io->original_radius *= (0.5f + v * 0.5f);
	}
	
	io->original_height = glm::clamp(io->original_height, -165.f, -40.f);
	
	if(io->original_radius > 40.f)
		io->original_radius = 40.f;

	io->physics.cyl.radius = io->original_radius * io->scale;
	io->physics.cyl.height = io->original_height * io->scale;
}
