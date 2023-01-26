#include <stdbool.h>
#include "obstacles.h"

Obstacle initalize_obstacle(Obstacle obstacle, float x_pos, float y_pos, float velo_x,
		float velo_y, int width, int height) {
	obstacle.x_pos = x_pos;
	obstacle.y_pos = y_pos;
	obstacle.y_vel = velo_y;
	obstacle.x_vel = velo_x;
	obstacle.width = width;
	obstacle.height = height;
	obstacle.status = true;
	return obstacle;
}

Obstacle update_obstacle_pos(Obstacle obstacle, float velo_y) {
	obstacle.y_pos += velo_y;
	return obstacle;

}
