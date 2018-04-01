#include <iostream>
#include <fstream>
#include <values.h>

#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

vec3 color(const ray& r, hitable* world, int depth) {
    hit_record rec;
    if(world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0, 0, 0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

hitable *scene() {
    hitable **list = new hitable*[4];
    texture *marble = new noise_texture();
    texture *checker = new checker_texture(
            new constant_texture(vec3(0.35, 0.35, 0.35)),
            new constant_texture(vec3(0.0, 0.52, 0.52))
    );

    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(checker));
    list[1] = new sphere(vec3(2, 1, -1), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    list[2] = new sphere(vec3(-1, 1, 0), 1.0, new lambertian(marble));
    list[3] = new sphere(vec3(3, 1, 2), 1.0, new dielectric(1.5));
    //list[4] = new sphere(vec3(7, 0.8, 1.4), 0.2, new lambertian(new constant_texture(vec3(1.0, 0.44, 0.54))));

    return new hitable_list(list,4);
}

int main() {
    std::ofstream file;
    file.open("../test.ppm");
    int nx = 1024;
    int ny = 768;
    int ns = 1000;
    file << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *world = scene();
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

    for(int j = ny-1; j >= 0; j--) {
        for(int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for(int s=0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            auto ir = int(255.99*col[0]);
            auto ig = int(255.99*col[1]);
            auto ib = int(255.99*col[2]);
            file << ir << " " << ig << " " << ib << "\n";
        }
    }

    file.close();

    return 0;
}