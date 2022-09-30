/*
 * Copyright 2013-2021 Arx Libertatis Team (see the AUTHORS file)
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

#ifndef ARX_MATH_VECTOR_H
#define ARX_MATH_VECTOR_H

#include <cmath>

#include "math/GtxFunctions.h"
#include "math/Types.h"

// TODO replace calls with their direct glm equivalents!

template <class V>
bool closerThan(const V & a, const V & b, typename V::value_type d) {
	return arx::distance2(a, b) < (d * d);
}

template <class V>
bool fartherThan(const V & a, const V & b, typename V::value_type d) {
	return arx::distance2(a, b) > (d * d);
}

template <class V>
bool isallfinite(const V & vec) {
	
	for(size_t i = 0; i < size_t(vec.length()); i++) {
		if(!(std::isfinite)(vec[i])) {
			return false;
		}
	}
	
	return true;
}

inline Vec2f getXZ(Vec3f pos) {
	return Vec2f(pos.x, pos.z);
}

#endif // ARX_MATH_VECTOR_H
