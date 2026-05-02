/*
MultiCraft - ESP Rendering
Ported from OtterClient
*/

#include "esp.h"
#include "client/client.h"
#include "client/clientenvironment.h"
#include "client/localplayer.h"
#include "client/clientmap.h"
#include "client/camera.h"
#include "client/content_cao.h"
#include "nodedef.h"
#include "settings.h"
#include "util/basic_macros.h"
#include <unordered_map>

ESPRenderer::ESPRenderer(Client *client) : m_client(client)
{
}

void ESPRenderer::draw()
{
	if (!g_settings->getBool("enable_esp"))
		return;

	ClientEnvironment &env = m_client->getEnv();
	Camera *camera = m_client->getCamera();
	LocalPlayer *player = env.getLocalPlayer();

	if (!player || !camera)
		return;

	auto driver = m_client->getSceneManager()->getVideoDriver();

	// Get settings
	bool draw_entities = g_settings->getBool("esp_entities");
	bool draw_players = g_settings->getBool("esp_players");
	bool draw_tracers = g_settings->getBool("esp_tracers");

	if (!draw_entities && !draw_players)
		return;

	// Setup material for ESP rendering
	video::SMaterial material;
	material.Lighting = false;
	material.ZBuffer = false;
	material.ZWriteEnable = irr::video::EZW_OFF;
	driver->setMaterial(material);

	// Get entities
	std::unordered_map<u16, ClientActiveObject*> allObjects;
	env.getAllActiveObjects(allObjects);

	v3f camera_offset = intToFloat(camera->getOffset(), BS);
	v3f eye_pos = camera->getPosition() + 1000.0f * camera->getDirection() - camera_offset;

	for (auto &ao_it : allObjects) {
		ClientActiveObject *cao = ao_it.second;
		if (cao->isLocalPlayer())
			continue;

		GenericCAO *obj = dynamic_cast<GenericCAO*>(cao);
		if (!obj)
			continue;

		bool is_player = obj->isPlayer();
		bool should_draw = is_player ? draw_players : draw_entities;
		if (!should_draw)
			continue;

		// Get bounding box
		aabb3f box;
		if (!obj->getSelectionBox(&box))
			continue;

		v3f pos = obj->getPosition() - camera_offset;
		box.MinEdge += pos;
		box.MaxEdge += pos;

		// Set color
		video::SColor color = is_player
			? video::SColor(255, 255, 0, 0)  // Red for players
			: video::SColor(255, 0, 255, 0); // Green for entities

		// Draw bounding box
		driver->draw3DBox(box, color);

		// Draw tracer line
		if (draw_tracers) {
			driver->draw3DLine(eye_pos, box.getCenter(), color);
		}
	}
}
