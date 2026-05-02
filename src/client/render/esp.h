/*
MultiCraft - ESP Rendering
Ported from OtterClient
*/

#pragma once

#include <irrlicht.h>

class Client;

class ESPRenderer {
public:
	ESPRenderer(Client *client);
	void draw();

private:
	Client *m_client;
};
