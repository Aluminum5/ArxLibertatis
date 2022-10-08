/*
 * Copyright 2011-2022 Arx Libertatis Team (see the AUTHORS file)
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

#ifndef ARX_GRAPHICS_EFFECTS_FOG_H
#define ARX_GRAPHICS_EFFECTS_FOG_H

#include <vector>

#include "core/GameTime.h"
#include "core/TimeTypes.h"
#include "graphics/BaseGraphicsTypes.h"
#include "graphics/Color.h"
#include "math/Types.h"
#include "math/Angle.h"
#include "math/Vector.h"
#include "math/Quantizer.h"
#include "platform/Platform.h"
#include "util/HandleContainer.h"


struct EERIE_3DOBJ;

struct FOG_DEF {
	
	bool exist = true;
	Vec3f pos = Vec3f(0.f);
	Color3f rgb;
	Color3f rgbRandom;
	float size = 0.f;
	bool directional = false;
	float sizeDelta = 0.f;
	Vec3f move = Vec3f(0.f);
	Anglef angle;
	float speed = 1.f;
	float speedRandom = 2.f;
	float rotatespeed = 0.f;
	GameDuration duration;
	long blend = 0;
	float frequency = 0.f;
	math::Quantizer elapsed;
	bool visible = false;
	GameInstant creationTime;
	
	constexpr FOG_DEF() arx_noexcept_default
	
};

extern util::HandleVector<FogHandle, FOG_DEF> g_fogs;

void ARX_FOGS_Render();
FogHandle ARX_FOGS_GetFree();
void ARX_FOGS_Clear();

#endif // ARX_GRAPHICS_EFFECTS_FOG_H
