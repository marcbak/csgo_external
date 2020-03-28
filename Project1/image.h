#pragma once
#include <d3dx9math.h>
#include "LocalPlayerInfo.h"
namespace Image
{
	bool screen_transform(const LocalPlayerInfo& local_player, const D3DXVECTOR3& point, D3DXVECTOR3& screen)
	{
		const D3DXMATRIX& w2sMatrix = local_player.get_view_matrix();
		screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
		screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
		screen.z = 0.0f;

		float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

		if (w < 0.001f) {
			screen.x *= 100000;
			screen.y *= 100000;
			return true;
		}

		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;

		return false;
	}

	bool world_to_screen(const LocalPlayerInfo& local_player, const D3DXVECTOR3& origin, D3DXVECTOR3& screen, int w, int h)
	{
		if (!screen_transform(local_player, origin, screen)) {
			screen.x = (w / 2.0f) + (screen.x * w) / 2;
			screen.y = (h / 2.0f) - (screen.y * h) / 2;

			return true;
		}
		return false;
	}

	D3DXVECTOR3 calculate_angle(const D3DXVECTOR3& source, const D3DXVECTOR3& destination)
	{
		constexpr float pi = 3.141592653589793;
		D3DXVECTOR3 angles{};
		D3DXVECTOR3 delta = source - destination;

		// przeciwprostok¹tna
		const float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		angles.x = asin(delta.z / hyp) * (180.0f / pi);
		angles.y = atan(delta.y / delta.x) * (180.0f / pi);

		if (angles.y < 0) {
			angles.y += 360;
		}
			
		if (delta.x >= 0.0) {
			angles.y += 180.0;
		}
		
		angles.z = 0.0f;

		return angles;
	}

}