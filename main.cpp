#include <iostream>
#include <fstream>
#include <values.h>

#include "sphere.h"
#include "rect.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "box.h"

vec3 color(const ray& r, hitable* world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        //vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return /*emitted +*/ attenuation*color(scattered, world, depth+1);
        else
            return vec3(0, 0, 0);//emitted;
    }
    else {
        //return vec3(0, 0, 0);
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}

hitable *scene() {
    hitable **list = new hitable*[9];
    int i = 0;
    texture  *perl_tex = new noise_texture(0.1);
    material *perl_mat = new lambertian(perl_tex);
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *met = new metal(vec3(0.7, 0.6, 0.5), 0.0);
    material *glass = new dielectric(1.5);
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    list[i++] = new xz_rect(0, 555, 0, 555, 0, green);
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), glass), -18), vec3(130,0,65));
    list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), red), 15), vec3(265,0,295));
    list[i++] = new sphere(vec3(480, 120, 90), 110, perl_mat);
    list[i++] = new sphere(vec3(85, 65, 330), 60, met);
    return new hitable_list(list,i);
}

int main() {
    std::ofstream file;
    file.open("../aeckert-hw7.ppm");
    int nx = 960;
    int ny = 720;
    int ns = 1000;
    file << "P3\n" << nx << " " << ny << "\n255\n";

    hitable *world = scene();

    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;

    camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny),
               aperture, dist_to_focus, 0.0, 1.0);

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