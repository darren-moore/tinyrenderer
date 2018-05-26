#include <complex>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor WHITE = TGAColor(255, 255, 255, 255);
const TGAColor RED   = TGAColor(255, 0,   0,   255);
const TGAColor GREEN = TGAColor(0, 255, 0, 255);
const TGAColor BLUE = TGAColor(0, 0, 255, 255);
const int WIDTH = 1000;
const int HEIGHT = 1000;

Model *model = NULL;

void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color);
void triangle(Vec3f *pts, float *zBuffer, TGAImage &image, TGAColor color);


int main(int argc, char** argv) {
	TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
	
	// init zBuffer
	float *zBuffer = new float[WIDTH*HEIGHT];
	for(int i = 0; i < WIDTH*HEIGHT; i++){
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	if(argc == 2){
		model = new Model(argv[1]);
	}
	else{
		model = new Model("obj/african_head.obj");
	}
	
	Vec3f light = Vec3f(0, 0, -1);
	light.normalize();
	// Model rendering
	for (int i=0; i<model->nfaces(); i++){
		//printf("%d\n",i);
		std::vector<int> face = model->face(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		for (int j=0; j<3; j++){
			Vec3f fv = model->vert(face[j]); // face vert
			screen_coords[j] = Vec3f(int((fv.x+1.) * WIDTH/2.), int((fv.y+1.) * HEIGHT/2.), fv.z);
			world_coords[j] = fv;
		}
		Vec3f normal = cross(world_coords[2]-world_coords[0],world_coords[1]-world_coords[0]);
		normal.normalize();
		float intensity = normal*light;
		TGAColor faceCol = TGAColor(intensity*255, intensity*255, intensity*255, 255);
		if(intensity > 0)
			triangle(screen_coords, zBuffer, image, faceCol);
	}
	

	

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}

Vec3f barycentric(Vec3f *pts, Vec3f P){
	// Get barycentric coords of point P on triangle given by pts
	// (1-u-v, u, v)
	Vec3f a = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x);
	Vec3f b = Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y);

	// Solve linear system with cross prod.
	Vec3f u = cross(a,b);
	// Get (u, v, 1)
	//printf("%f,%f,%f\n",u.x,u.y,u.z);
	// if u[2] < 1, degenerate case, else normalize & return
	if(std::abs(u.z) > 1e-2)
		return Vec3f(1.f - (u.x + u.y)/u.z, u.y/u.z, u.x/u.z);

	return Vec3f(-1, 1, 1);
}

void triangle(Vec3f *pts, float *zBuffer, TGAImage &image, TGAColor color){

	Vec2f boxMin(image.get_width() - 1, image.get_height() - 1);
	Vec2f boxMax(0,0);
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	
	// Figure the box:
	for(int i=0; i<3; i++){
		for(int j=0; j<2; j++){
			boxMin[j] = std::max(0.f, std::min(boxMin[j], pts[i][j]));
			boxMax[j] = std::min(clamp[j], std::max(boxMax[j], pts[i][j]));
		}
	}
	
	// Walk the box and paint
	Vec3f p;
	for(p.x=boxMin.x; p.x<=boxMax.x; p.x++){
		for(p.y=boxMin.y; p.y<=boxMax.y; p.y++){
			Vec3f bary = barycentric(pts, p);
			if(bary.x<0||bary.y<0||bary.z<0) continue;
			// Gather z-value of p
			p.z = 0;
			for(int i=0; i<3; i++)
				p.z += pts[i].z*bary[i];
			if(zBuffer[int(p.x+p.y*WIDTH)] <= p.z){
				zBuffer[int(p.x+p.y*WIDTH)] = p.z;
				image.set(p.x,p.y,color);
			}
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
