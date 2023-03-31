#define COW_NO_STYLE_GUIDE
#include "include.cpp"

bool is_vec3_equal(vec3 a, vec3 b) {
  if ((a[0] == b[0]) && (a[1] == b[1]) && (a[2] == b[2])) {
    return true;
  } 
  return false; 
}

struct boid {
    vec3 position;
    vec3 velocity;
};

int num_boids = 800;
boid *boids = (boid *) calloc(num_boids, sizeof(boid));


vec3 boisRules(boid b) {
    vec3 cohesion_pos = {};
    vec3 separation_pos = {};
    vec3 vel = {};
    real cohesion_speed = 0.00008;
    real alignment_speed = 0.00005;
    real distance_threshold = 2;

    for (int i = 0; i < num_boids; ++i) {
        // ignore if b = boids[i]
        if (!is_vec3_equal(b.position, boids[i].position) && !is_vec3_equal(b.velocity, boids[i].velocity)) {
            // cohesion
            cohesion_pos += boids[i].position;

            // sepration
            if (norm(b.position - boids[i].position) <= distance_threshold) {
                separation_pos -= (boids[i].position - b.position);
            }

            // alignment
            vel += boids[i].velocity;
        }
    }

    // cohesion: compute the average position of boids
    cohesion_pos /= (num_boids - 1);
    vec3 new_Cohesion_Pos = (cohesion_pos - b.position) * cohesion_speed;

    // alignment
    vel = vel / (num_boids - 1);
    vec3 newVel = (vel - b.velocity) * alignment_speed;
    
    return new_Cohesion_Pos + separation_pos + newVel;
}

void transcending_space() {
    Camera3D camera = {50.0, RAD(45)};
    sound_loop_music("codebase/butterfly.wav");

    int primitives[] = {SOUP_POINTS, SOUP_LINES, SOUP_LINE_STRIP, SOUP_LINE_LOOP};
    int primitive_index = 0;

    real t = 0;
    int color_index = 0;
    

    // initialize the positions and velocity
    for (int i = 0; i < num_boids; ++i) {
        boids[i].position = V3(random_real(-20.0, 20.0), random_real(-15.0, 15.0), random_real(-9.0, 18.0));
        boids[i].velocity = V3(random_real(-0.001, 0.001), random_real(-0.001, 0.001), random_real(-0.001, 0.001));
    }

    while(cow_begin_frame()) {
        camera_move(&camera);
        mat4 PV = camera_get_PV(&camera);
        gui_slider("primitive_index", &primitive_index, 0, 2, 'j', 'k', true);
        gui_slider("color_index", &color_index, 0, 1, 'n', 'm', true);

        eso_begin(PV, primitives[primitive_index]);
        //eso_color(monokai.white);
        for (int i = 0; i < num_boids; ++i) {
            t = real(i) / (num_boids - 1);
            if (color_index == 0) {
                eso_color(monokai.white);
            } else {
                eso_color(color_plasma(t));
            }
            eso_vertex(boids[i].position);
            vec3 vec = boisRules(boids[i]);

            //update
            boids[i].velocity += vec;
            boids[i].position += boids[i].velocity;
        }
        eso_end();
    }
    free(boids);

}



int main() {
    APPS {
        APP(transcending_space);
    }
    return 0;
}

