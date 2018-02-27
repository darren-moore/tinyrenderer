#include <complex>
#include <vector>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255, 0,   0,   255);
const TGAColor GREEN = TGAColor(0, 255, 0, 255);
const TGAColor BLUE = TGAColor(0, 0, 255, 255);
const int WIDTH = 200;
const int HEIGHT = 200;

Model *model = NULL;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);

int main(int argc, char** argv) {
	TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
	if(argc == 2){
		model = new Model(argv[1]);
	}
	else{
		model = new Model("obj/african_head.obj");
	}

	Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
	Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150,1), Vec2i(70,180)};
	Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};
	triangle(t0[0], t0[1], t0[2], image, RED);
	triangle(t1[0], t1[1], t1[2], image, GREEN);
	triangle(t2[0], t2[1], t2[2], image, BLUE);
	
	// Model rendering
	/*
	for (int i=0; i<model->nfaces(); i++){
		std::vector<int> face = model->face(i);
		for (int j=0; j<3; j++){
			Vec3f v0 = model->vert(face[j]);
			Vec3f v1 = model->vert(face[(j+1)%3]);
			int x0 = (v0.x+1) * WIDTH/2;
			int y0 = (v0.y+1) * HEIGHT/2;
			int x1 = (v1.x+1) * WIDTH/2;
			int y1 = (v1.y+1) * HEIGHT/2;
			line(x0, y0, x1, y1, image, white);
		
		}	
	}
	*/

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
	// Sort verts by y coord
	// t0 smallest
	
	if(t0.y > t1.y){
		std::swap(t0, t1);
	}
	if(t0.y > t2.y){
		std::swap(t0, t2);
	}
	if(t1.y > t2.y){
		std::swap(t1, t2);
	}

	
	// We use Bressenham's algo on two lines at once.
	// First up to t1, then to t2.

	for(int y = t0.y; y <= t1.y; y++){
		float t_t1 = (y-t0.y) / (float)(t1.y-t0.y);
		float t_t2 = (y-t0.y) / (float)(t2.y-t0.y);
		int x_t1 = t0.x*(1.-t_t1) + t1.x*t_t1; 
		int x_t2 = t0.x*(1.-t_t2) + t2.x*t_t2;	
		int xLeft = x_t2;
		int xRight = x_t1;
		
		// Need to fill in the right direction!
		if(t1.x < t0.x){
			std::swap(xLeft,xRight);
		}
		// Fill in w/ horiz line
		for (int xWalk = xLeft; xWalk <= xRight; xWalk++){
			image.set(xWalk, y, color);
		}
	}

	for(int y = t1.y; y <= t2.y; y++){
		float t_t1 = (y-t1.y) / (float)(t2.y-t1.y);
		float t_t2 = (y-t0.y) / (float)(t2.y-t0.y);
		int x_t1 = t1.x*(1.-t_t1) + t2.x*t_t1;
		int x_t2 = t0.x*(1.-t_t2) + t2.x*t_t2;	
		int xLeft = x_t2;
		int xRight = x_t1;
		if(t1.x < t0.x){
			std::swap(xLeft,xRight);
		}
		// Fill in w/ horiz line
		for (int xWalk = xLeft; xWalk <= xRight; xWalk++){
			image.set(xWalk, y, color);
		}
	}


}

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color){
	// We use Bressenham's algo
	bool vertChart = false;
	
	int x0 = p0.x;
	int y0 = p0.y;
	int x1 = p1.x;
	int y1 = p1.y;
	
	// Use vertical chart?
	if(std::abs(x0 - x1) < std::abs(y0 - y1)){
		std::swap(x0, y0);
		std::swap(x1, y1);
		vertChart = true;
	}
	
	// We shall draw left to right
	if(x0 > x1){
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	// Finally, draw it!
	for(int x = x0; x <= x1; x++){
		float t = (x-x0) / (float)(x1-x0);
		int y = y0*(1.-t) + y1*t;
		if(vertChart){
			image.set(y, x, color);
		}
		else{
			image.set(x, y, color);
		
		}
	}

}
