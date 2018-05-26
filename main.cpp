#include <complex>
#include <vector>
#include <cmath>
#include <cstdlib>

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

	// Model rendering
	for (int i=0; i<model->nfaces(); i++){
		//printf("%d\n",i);
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		for (int j=0; j<3; j++){
			Vec3f fv = model->vert(face[j]); // face vert
			screen_coords[j] = Vec2i((fv.x+1) * WIDTH/2, (fv.y+1) * HEIGHT/2);
		}
		triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand()%255, rand()%255, rand()%255,255));	
	}
	

	

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}
/*
Vec3f barycentric(Vec2i *pts, Vec2i P){
	// Get barycentric coords of point P on triangle given by pts
	// (1-u-v, u, v)
	Vec3f a = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]);
	Vec3f b = Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);
	
	// Solve linear system with cross prod.
	Vec3f u = cross(a, b);
	// Get (u, v, 1)

	// if u[2] < 1, degenerate case, else normalize & return
	if(std::abs(u.z) < 1)
		return Vec3f(-1, 1, 1);
	else
		return Vec3f(1. - (u.x + u.y)/u.z, u.x/u.z, u.x/u.z);

}

void triangle2(Vec2i *pts, TGAImage &image, TGAColor color){
	Vec2i boxMin(image.get_width() - 1, image.get_height() - 1);
	Vec2i boxMax(0,0);
	Vec2i clamp(image.get_width)

}
*/
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color){
	// Sort verts by y coord
	// t0 smallest
	
	if(t0.y > t1.y)
		std::swap(t0, t1);
	if(t0.y > t2.y)
		std::swap(t0, t2);
	if(t1.y > t2.y)
		std::swap(t1, t2);
	
	// We use Bressenham's algo on two lines at once.
	// First up to t1, then to t2.

	for(int y = t0.y; y <= t1.y; y++){
		float t_t1 = (y-t0.y) / (float)(t1.y-t0.y + .01);
		float t_t2 = (y-t0.y) / (float)(t2.y-t0.y + .01);
		int x_t1 = t0.x*(1.-t_t1) + t1.x*t_t1; 
		int x_t2 = t0.x*(1.-t_t2) + t2.x*t_t2;	
		int xLeft = x_t2;
		int xRight = x_t1;
		
		// Need to fill in the right direction!
		if(xRight < xLeft)
			std::swap(xLeft,xRight);
		// Fill in w/ horiz line
		for (int xWalk = xLeft; xWalk <= xRight; xWalk++){
			image.set(xWalk, y, color);
		}
	}

	for(int y = t1.y; y <= t2.y; y++){
		float t_t1 = (y-t1.y) / (float)(t2.y-t1.y + .01);
		float t_t2 = (y-t0.y) / (float)(t2.y-t0.y + .01);
		int x_t1 = t1.x*(1.-t_t1) + t2.x*t_t1;
		int x_t2 = t0.x*(1.-t_t2) + t2.x*t_t2;	
		int xLeft = x_t2;
		int xRight = x_t1;
		if(xRight < xLeft)
			std::swap(xLeft,xRight);
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
