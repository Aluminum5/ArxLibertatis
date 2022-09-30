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

#include "graphics/data/MeshManipulation.h"

#include <stddef.h>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include "game/Entity.h"

#include "graphics/BaseGraphicsTypes.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/Vertex.h"
#include "graphics/data/Mesh.h"
#include "graphics/data/TextureContainer.h"

#include "io/resource/PakReader.h"
#include "io/log/Logger.h"

#include "math/Types.h"
#include "math/Vector.h"

#include "platform/Platform.h"

#include "scene/Object.h"

void EERIE_MESH_TWEAK_Skin(EERIE_3DOBJ * obj, const res::path & s1, const res::path & s2) {
	
	LogDebug("Tweak Skin " << s1 << " " << s2);
	
	if(obj == nullptr || s1.empty() || s2.empty()) {
		LogError << "Tweak Skin got NULL Pointer";
		return;
	}
	
	LogDebug("Tweak Skin " << s1 << " " << s2);
	
	res::path skintochange = "graph/obj3d/textures" / s1;
	
	res::path skinname = "graph/obj3d/textures" / s2;
	TextureContainer * tex = TextureContainer::Load(skinname);
	if(!tex) {
		return;
	}
	
	if(obj->originaltextures.empty()) {
		obj->originaltextures.reserve(obj->texturecontainer.size());
		for(TextureContainer * texture : obj->texturecontainer) {
			obj->originaltextures.emplace_back(texture ? texture->m_texName : std::string_view());
		}
	}
	
	arx_assert(obj->originaltextures.size() == obj->texturecontainer.size());
	
	bool found = false;
	
	for(size_t i = 0; i < obj->texturecontainer.size(); i++) {
		if(obj->originaltextures[i] == skintochange) {
			obj->texturecontainer[i] = tex;
			found = true;
		}
	}
	
	if(found) {
		return;
	}
	
	for(TextureContainer * & texture : obj->texturecontainer) {
		if(texture->m_texName == skintochange) {
			texture = tex;
		}
	}
	
}

bool IsInSelection(const EERIE_3DOBJ * obj, size_t vert, ObjSelection tw) {
	
	if(!obj || tw == ObjSelection()) {
		return false;
	}
	
	const EERIE_SELECTIONS & sel = obj->selections[tw.handleData()];
	
	return std::find(sel.selected.begin(), sel.selected.end(), vert) != sel.selected.end();
}

static size_t getEquivalentVertex(const EERIE_3DOBJ & obj, Vec3f vertex) {
	
	for(size_t i = 0; i < obj.vertexlist.size(); i++) {
		if(obj.vertexlist[i].v == vertex) {
			return i;
		}
	}
	
	return size_t(-1);
}

static size_t ObjectAddVertex(EERIE_3DOBJ * obj, const EERIE_VERTEX * vert) {
	
	for(size_t i = 0; i < obj->vertexlist.size(); i++) {
		if(obj->vertexlist[i].v.x == vert->v.x
		   && obj->vertexlist[i].v.y == vert->v.y
		   && obj->vertexlist[i].v.z == vert->v.z
		) {
			return i;
		}
	}

	obj->vertexlist.push_back(*vert);
	return obj->vertexlist.size() - 1;
}

static ActionPoint GetActionPoint(const EERIE_3DOBJ * obj, const char * name) {
	
	if(!obj)
		return ActionPoint();
	
	for(const EERIE_ACTIONLIST & act : obj->actionlist) {
		if(act.name == name) {
			return act.idx;
		}
	}

	return ActionPoint();
}

static long ObjectAddFace(EERIE_3DOBJ * obj, const EERIE_FACE * face, const EERIE_3DOBJ * srcobj) {
	
	// Check Already existing faces
	for(size_t i = 0; i < obj->facelist.size(); i++) {
		if(obj->vertexlist[obj->facelist[i].vid[0]].v.x == srcobj->vertexlist[face->vid[0]].v.x
		   && obj->vertexlist[obj->facelist[i].vid[1]].v.x == srcobj->vertexlist[face->vid[1]].v.x
		   && obj->vertexlist[obj->facelist[i].vid[2]].v.x == srcobj->vertexlist[face->vid[2]].v.x
		   && obj->vertexlist[obj->facelist[i].vid[0]].v.y == srcobj->vertexlist[face->vid[0]].v.y
		   && obj->vertexlist[obj->facelist[i].vid[1]].v.y == srcobj->vertexlist[face->vid[1]].v.y
		   && obj->vertexlist[obj->facelist[i].vid[2]].v.y == srcobj->vertexlist[face->vid[2]].v.y
		   && obj->vertexlist[obj->facelist[i].vid[0]].v.z == srcobj->vertexlist[face->vid[0]].v.z
		   && obj->vertexlist[obj->facelist[i].vid[1]].v.z == srcobj->vertexlist[face->vid[1]].v.z
		   && obj->vertexlist[obj->facelist[i].vid[2]].v.z == srcobj->vertexlist[face->vid[2]].v.z
		) {
			return -1;
		}
	}

	size_t f0 = ObjectAddVertex(obj, &srcobj->vertexlist[face->vid[0]]);
	size_t f1 = ObjectAddVertex(obj, &srcobj->vertexlist[face->vid[1]]);
	size_t f2 = ObjectAddVertex(obj, &srcobj->vertexlist[face->vid[2]]);
	
	obj->facelist.push_back(*face);

	obj->facelist.back().vid[0] = static_cast<unsigned short>(f0);
	obj->facelist.back().vid[1] = static_cast<unsigned short>(f1);
	obj->facelist.back().vid[2] = static_cast<unsigned short>(f2);
	obj->facelist.back().texid = 0;
	
	for(size_t i = 0; i < obj->texturecontainer.size(); i++) {
		if(face->texid >= 0 && size_t(face->texid) < srcobj->texturecontainer.size()
		   && obj->texturecontainer[i] == srcobj->texturecontainer[face->texid]) {
			obj->facelist.back().texid = short(i);
			break;
		}
	}

	return obj->facelist.size() - 1;
}

static void ObjectAddAction(EERIE_3DOBJ * obj, std::string_view name, const EERIE_VERTEX * vert) {
	
	size_t newvert = ObjectAddVertex(obj, vert);
	
	for(const EERIE_ACTIONLIST & action : obj->actionlist) {
		if(action.name == name) {
			return;
		}
	}
	
	obj->actionlist.emplace_back();
	
	EERIE_ACTIONLIST & action = obj->actionlist.back();
	
	action.name = name;
	action.idx = ActionPoint(newvert);
}

long ObjectAddMap(EERIE_3DOBJ * obj, TextureContainer * tc) {
	
	if(tc == nullptr)
		return -1;

	for(size_t i = 0; i < obj->texturecontainer.size(); i++) {
		if(obj->texturecontainer[i] == tc)
			return i;
	}

	obj->texturecontainer.push_back(tc);

	return obj->texturecontainer.size() - 1;
}

static void AddVertexToGroup(EERIE_3DOBJ * obj, size_t group, const EERIE_VERTEX * vert) {

	for(size_t i = 0; i < obj->vertexlist.size(); i++) {
		if(obj->vertexlist[i].v.x == vert->v.x
		   && obj->vertexlist[i].v.y == vert->v.y
		   && obj->vertexlist[i].v.z == vert->v.z
		) {
			AddVertexIdxToGroup(obj, group, i);
		}
	}
}

void AddVertexIdxToGroup(EERIE_3DOBJ * obj, size_t group, size_t val) {
	
	auto & indices = obj->grouplist[group].indexes;
	if(std::find(indices.begin(), indices.end(), val) == indices.end()) {
		indices.push_back(val);
	}
	
}

static void copySelection(const EERIE_3DOBJ & source, ObjSelection sourceSelection,
                          EERIE_3DOBJ & dest, ObjSelection destSelection) {
	
	for(size_t index : source.selections[sourceSelection.handleData()].selected) {
		size_t t = getEquivalentVertex(dest, source.vertexlist[index].v);
		if(t != size_t(-1)) {
			auto & selection = dest.selections[destSelection.handleData()].selected;
			if(std::find(selection.begin(), selection.end(), t) == selection.end()) {
				selection.push_back(t);
			}
		}
	}
	
}

static EERIE_3DOBJ * CreateIntermediaryMesh(const EERIE_3DOBJ * obj1, const EERIE_3DOBJ * obj2, long tw) {
	
	ObjSelection tw1 = ObjSelection();
	ObjSelection tw2 = ObjSelection();
	ObjSelection iw1 = ObjSelection();
	ObjSelection jw1 = ObjSelection();
	ObjSelection sel_head1 = ObjSelection();
	ObjSelection sel_head2 = ObjSelection();
	ObjSelection sel_torso1 = ObjSelection();
	ObjSelection sel_torso2 = ObjSelection();
	ObjSelection sel_legs1 = ObjSelection();
	ObjSelection sel_legs2 = ObjSelection();

	// First we retreive selection groups indexes
	for(size_t i = 0; i < obj1->selections.size(); i++) { // TODO iterator
		ObjSelection sel = ObjSelection(i);
		
		if(obj1->selections[i].name == "head") {
			sel_head1 = sel;
		} else if(obj1->selections[i].name == "chest") {
			sel_torso1 = sel;
		} else if(obj1->selections[i].name == "leggings") {
			sel_legs1 = sel;
		}
	}

	for(size_t i = 0; i < obj2->selections.size(); i++) { // TODO iterator
		ObjSelection sel = ObjSelection(i);
		
		if(obj2->selections[i].name == "head") {
			sel_head2 = sel;
		} else if(obj2->selections[i].name == "chest") {
			sel_torso2 = sel;
		} else if(obj2->selections[i].name == "leggings") {
			sel_legs2 = sel;
		}
	}

	if(sel_head1 == ObjSelection()) return nullptr;

	if(sel_head2 == ObjSelection()) return nullptr;

	if(sel_torso1 == ObjSelection()) return nullptr;

	if(sel_torso2 == ObjSelection()) return nullptr;

	if(sel_legs1 == ObjSelection()) return nullptr;

	if(sel_legs2 == ObjSelection()) return nullptr;

	if(tw == TWEAK_HEAD) {
		tw1 = sel_head1;
		tw2 = sel_head2;
		iw1 = sel_torso1;
		jw1 = sel_legs1;
	}

	if(tw == TWEAK_TORSO) {
		tw1 = sel_torso1;
		tw2 = sel_torso2;
		iw1 = sel_head1;
		jw1 = sel_legs1;
	}

	if(tw == TWEAK_LEGS) {
		tw1 = sel_legs1;
		tw2 = sel_legs2;
		iw1 = sel_torso1;
		jw1 = sel_head1;
	}

	if(tw1 == ObjSelection() || tw2 == ObjSelection())
		return nullptr;

	// Now Retreives Tweak Action Points
	{
		ActionPoint idx_head1 = GetActionPoint(obj1, "head2chest");
		if(idx_head1 == ActionPoint())
			return nullptr;

		ActionPoint idx_head2 = GetActionPoint(obj2, "head2chest");
		if(idx_head2 == ActionPoint())
			return nullptr;

		ActionPoint idx_torso1 = GetActionPoint(obj1, "chest2leggings");
		if(idx_torso1 == ActionPoint())
			return nullptr;

		ActionPoint idx_torso2 = GetActionPoint(obj2, "chest2leggings");
		if(idx_torso2 == ActionPoint())
			return nullptr;
	}
	
	// Work will contain the Tweaked object
	EERIE_3DOBJ * work = new EERIE_3DOBJ;
	
	// Linked objects are linked to this object.
	if(obj1->linked.size() > obj2->linked.size()) {
		work->linked = obj1->linked;
	} else {
		work->linked = obj2->linked;
	}
	
	// Is the origin of object in obj1 or obj2 ? Retreives it for work object
	if(IsInSelection(obj1, obj1->origin, tw1)) {
		work->origin = ObjectAddVertex(work, &obj2->vertexlist[obj2->origin]);
	} else {
		work->origin = ObjectAddVertex(work, &obj1->vertexlist[obj1->origin]);
	}
	
	// Recreate Action Points included in work object.for Obj1
	for(size_t i = 0; i < obj1->actionlist.size(); i++) {
		const EERIE_ACTIONLIST & action = obj1->actionlist[i];
		if(   IsInSelection(obj1, action.idx.handleData(), iw1)
		   || IsInSelection(obj1, action.idx.handleData(), jw1)
		   || action.name == "head2chest"
		   || action.name == "chest2leggings"
		) {
			ObjectAddAction(work, action.name, &obj1->vertexlist[action.idx.handleData()]);
		}
	}

	// Do the same for Obj2
	for(size_t i = 0; i < obj2->actionlist.size(); i++) {
		const EERIE_ACTIONLIST & action = obj2->actionlist[i];

		if(   IsInSelection(obj2, action.idx.handleData(), tw2)
		   || action.name == "head2chest"
		   || action.name == "chest2leggings"
		) {
			ObjectAddAction(work, action.name, &obj2->vertexlist[action.idx.handleData()]);
		}
	}

	// Recreate Vertex using Obj1 Vertexes
	for(size_t i = 0; i < obj1->vertexlist.size(); i++) {
		if(IsInSelection(obj1, i, iw1) || IsInSelection(obj1, i, jw1)) {
			ObjectAddVertex(work, &obj1->vertexlist[i]);
		}
	}

	// The same for Obj2
	for(size_t i = 0; i < obj2->vertexlist.size(); i++) {
		if(IsInSelection(obj2, i, tw2)) {
			ObjectAddVertex(work, &obj2->vertexlist[i]);
		}
	}


	// Look in Faces for forgotten Vertexes... AND
	// Re-Create TextureContainers Infos
	// We look for texturecontainers included in the future tweaked object
	TextureContainer * tc = nullptr;

	for(size_t i = 0; i < obj1->facelist.size(); i++) {
		const EERIE_FACE & face = obj1->facelist[i];

		if(   (IsInSelection(obj1, face.vid[0], iw1) || IsInSelection(obj1, face.vid[0], jw1))
		   && (IsInSelection(obj1, face.vid[1], iw1) || IsInSelection(obj1, face.vid[1], jw1))
		   && (IsInSelection(obj1, face.vid[2], iw1) || IsInSelection(obj1, face.vid[2], jw1))
		) {
			if(face.texid != -1) {
				if(tc != obj1->texturecontainer[face.texid]) {
					tc = obj1->texturecontainer[face.texid];
					ObjectAddMap(work, tc);
				}
			}

			ObjectAddFace(work, &face, obj1);
		}
	}

	for(size_t i = 0; i < obj2->facelist.size(); i++) {
		const EERIE_FACE & face = obj2->facelist[i];

		if(   IsInSelection(obj2, face.vid[0], tw2)
		   || IsInSelection(obj2, face.vid[1], tw2)
		   || IsInSelection(obj2, face.vid[2], tw2)
		) {

			if(face.texid != -1) {
				if(tc != obj2->texturecontainer[face.texid]) {
					tc = obj2->texturecontainer[face.texid];
					ObjectAddMap(work, tc);
				}
			}

			ObjectAddFace(work, &face, obj2);
		}
	}
	
	// Recreate Groups
	work->grouplist.resize(std::max(obj1->grouplist.size(), obj2->grouplist.size()));
	
	for(size_t k = 0; k < obj1->grouplist.size(); k++) {
		const VertexGroup & grp = obj1->grouplist[k];
		work->grouplist[k].name = grp.name;
		size_t v = getEquivalentVertex(*work, obj1->vertexlist[grp.origin].v);
		if(v != size_t(-1)) {
			work->grouplist[k].m_blobShadowSize = grp.m_blobShadowSize;
			if(IsInSelection(obj1, grp.origin, iw1) || IsInSelection(obj1, grp.origin, jw1)) {
				work->grouplist[k].origin = v;
			}
		}
	}
	
	for(size_t k = 0; k < obj2->grouplist.size(); k++) {
		if(k >= obj1->grouplist.size()) {
			work->grouplist[k].name = obj2->grouplist[k].name;
		}
		size_t v = getEquivalentVertex(*work, obj2->vertexlist[obj2->grouplist[k].origin].v);
		if(v != size_t(-1)) {
			work->grouplist[k].m_blobShadowSize = obj2->grouplist[k].m_blobShadowSize;
			if(IsInSelection(obj2, obj2->grouplist[k].origin, tw2)) {
				work->grouplist[k].origin = v;
			}
		}
	}
	
	// Recreate Selection Groups (only the 3 selections needed to reiterate mesh tweaking)
	work->selections.resize(3);
	work->selections[0].name = "head";
	work->selections[1].name = "chest";
	work->selections[2].name = "leggings";
	if(tw == TWEAK_HEAD) {
		copySelection(*obj2, sel_head2, *work, ObjSelection(0));
	} else {
		copySelection(*obj1, sel_head1, *work, ObjSelection(0));
	}
	if(tw == TWEAK_TORSO) {
		copySelection(*obj2, sel_torso2, *work, ObjSelection(1));
	} else {
		copySelection(*obj1, sel_torso1, *work, ObjSelection(1));
	}
	if(tw == TWEAK_LEGS) {
		copySelection(*obj2, sel_legs2, *work, ObjSelection(2));
	} else {
		copySelection(*obj1, sel_legs1, *work, ObjSelection(2));
	}
	
	// Now recreates other selections...
	for(size_t i = 0; i < obj1->selections.size(); i++) {
		
		if(EERIE_OBJECT_GetSelection(work, obj1->selections[i].name) != ObjSelection()) {
			continue;
		}
		
		size_t num = work->selections.size();
		work->selections.resize(num + 1);
		work->selections[num].name = obj1->selections[i].name;
		
		copySelection(*obj1, ObjSelection(i), *work, ObjSelection(num));
		
		ObjSelection ii = EERIE_OBJECT_GetSelection(obj2, obj1->selections[i].name);
		if(ii != ObjSelection()) {
			copySelection(*obj2, ii, *work, ObjSelection(num));
		}
		
	}
	
	for(size_t i = 0; i < obj2->selections.size(); i++) {
		if(EERIE_OBJECT_GetSelection(work, obj2->selections[i].name) == ObjSelection()) {
			size_t num = work->selections.size();
			work->selections.resize(num + 1);
			work->selections[num].name = obj2->selections[i].name;
			copySelection(*obj2, ObjSelection(i), *work, ObjSelection(num));
		}
	}
	
	// Recreate Animation-groups vertex
	for(size_t i = 0; i < obj1->grouplist.size(); i++) {
		for(size_t j = 0; j < obj1->grouplist[i].indexes.size(); j++) {
			AddVertexToGroup(work, i, &obj1->vertexlist[obj1->grouplist[i].indexes[j]]);
		}
	}

	for(size_t i = 0; i < obj2->grouplist.size(); i++) {
		for(size_t j = 0; j < obj2->grouplist[i].indexes.size(); j++) {
			AddVertexToGroup(work, i, &obj2->vertexlist[obj2->grouplist[i].indexes[j]]);
		}
	}
	
	work->vertexWorldPositions.resize(work->vertexlist.size());
	work->vertexClipPositions.resize(work->vertexlist.size());
	work->vertexColors.resize(work->vertexlist.size());
	
	return work;
}

void EERIE_MESH_TWEAK_Do(Entity * io, TweakType tw, const res::path & path) {
	
	res::path ftl_file = ("game" / path).set_ext("ftl");

	if ((!g_resources->getFile(ftl_file)) && (!g_resources->getFile(path))) return;

	if (io == nullptr) return;

	if (io->obj == nullptr) return;

	if(path.empty() && tw == TWEAK_REMOVE) {
		
		if(io->tweaky) {
			delete io->obj;
			io->obj = io->tweaky;
			EERIE_Object_Precompute_Fast_Access(io->obj);
			io->tweaky = nullptr;
		}
		
		return;
	}
	
	if(!(tw & (TWEAK_HEAD | TWEAK_TORSO | TWEAK_LEGS))){
		return;
	}
	
	EERIE_3DOBJ * tobj = loadObject(path);
	if(!tobj) {
		return;
	}
	
	EERIE_3DOBJ * result = nullptr;
	if(tw == (TWEAK_HEAD | TWEAK_TORSO | TWEAK_LEGS)) {
		result = tobj; // Replace the entire mesh
	} else {
		
		if(tw & TWEAK_HEAD) {
			result = CreateIntermediaryMesh(io->obj, tobj, TWEAK_HEAD);
		} else {
			result = io->obj;
		}
		
		if(result && (tw & TWEAK_TORSO)) {
			EERIE_3DOBJ * result2 = CreateIntermediaryMesh(result, tobj, TWEAK_TORSO);
			if(result != io->obj) {
				delete result;
			}
			result = result2;
		}
		
		if(result && (tw & TWEAK_LEGS)) {
			EERIE_3DOBJ * result2 = CreateIntermediaryMesh(result, tobj, TWEAK_LEGS);
			if(result != io->obj) {
				delete result;
			}
			result = result2;
		}
		
		delete tobj;
		
		arx_assert(result != io->obj);
		
		if(!result) {
			return;
		}
		
		EERIE_Object_Precompute_Fast_Access(result);
		
		EERIE_CreateCedricData(result);
		
		// TODO also do this for the other branch?
		io->animBlend.lastanimtime = 0;
		io->animBlend.m_active = false;
	}
	
	if(!io->tweaky) {
		io->tweaky = io->obj;
	} else if(io->tweaky != io->obj) {
		delete io->obj;
	}
	
	io->obj = result;
	
}
