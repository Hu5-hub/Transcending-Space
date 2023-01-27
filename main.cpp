// #define COW_NO_SOUND
#define JIM_IS_JIM
#include "include.cpp"
#include "diego.cpp"

void app_hud() {
    char *filename = "codebase/316818__steaq__432-hz.wav";
    char buffer[128] = {};
    int num_chars = 0;
    real interval_time_in_minutes = 10.0;
    real timestamp = util_timestamp_in_milliseconds();
    int num_badges = 0;
    sound_play_sound(filename);
    COW1._gui_hide_and_disable = true;
    config.tweaks_soup_draw_with_rounded_corners_for_all_line_primitives = false;

    while (cow_begin_frame()) {
        gui_printf("> %s", buffer);
        gui_readout("interval_time_in_minutes", &interval_time_in_minutes);

        static char keys[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
        for_(i, _COUNT_OF(keys)) {
            if (globals.key_pressed[keys[i]]) {
                buffer[num_chars++] = keys[i];
            }
        }
        if (globals.key_pressed[COW_KEY_BACKSPACE] && num_chars > 0) {
            buffer[--num_chars] = '\0';
        }
        if (globals.key_pressed[COW_KEY_ENTER]) {
            // sound_play_sound(filename);
            if (num_chars > 0) {
                sscanf(buffer, "%lf", &interval_time_in_minutes);
                num_chars = 0;
                memset(buffer, 0, _COUNT_OF(buffer));
                num_badges = 0;
            }
            timestamp = util_timestamp_in_milliseconds();
        }
        real minutes_per_interval = (util_timestamp_in_milliseconds() - timestamp) / (1000 * 60);
        real f = minutes_per_interval / interval_time_in_minutes;
        gui_readout("f", &f);
        if (f > 1.0) {
            sound_play_sound(filename);
            timestamp = util_timestamp_in_milliseconds();
            ++num_badges;
        }

        {
            eso_begin(globals.Identity, SOUP_LINES, 64);
            eso_color(color_plasma(f));
            eso_vertex(-1.0, 0.0);
            eso_vertex(LERP(f, -1.0, 1.0), 0.0);
            eso_end();
        }
        {
            eso_begin(globals.Identity, SOUP_POINTS, 32);
            eso_color(color_plasma(f));
            for (int i = 0; i < num_badges; ++i) {
                eso_vertex(-1.0 + (i + 1) * .05, 1.0 - 0.05);
            }
            eso_end();
        }
    }
}

void app_space_fish() {
    real data[3] = {};
    real *dynamixel_angle = data;
    real *asm5601_angle = data + 1;

    real fish_theta = 0.;

    // todo save camera to camera.txt super easily
    Camera3D camera = { 10.0 };
    while (cow_begin_frame()) {
        camera_move(&camera);
        mat4 P, V, PV; {
            P = camera_get_P(&camera);
            V = camera_get_V(&camera);
            PV = P * V;
        }

        gui_slider("dynamixel_angle", dynamixel_angle, RAD(-180), RAD(180), true);
        gui_slider("asm5601_angle", asm5601_angle, RAD(-180), RAD(180), true);

        int kelly_i = 0;
        auto _Q = [&](vec3 size, vec3 origin_datum, mat4 M2) {
            // // origin_datum
            // (0, 0, 0) center
            // (0, -1, 0) center of bottom face
            // (1, 1, 0) center of upper right edge
            // (1, 1, 1) ... corner
            mat4 M = M4_Translation(-cwiseProduct(origin_datum, .5 * size)) * M4_Scaling(.5 * size);
            library.meshes.box.draw(P, V, M2 * M, color_kelly(kelly_i++));
            // library.soups.box.draw(P * V * M2 * M, color_kelly(kelly_i++));
        };
        auto Q = [&](real size_x, real size_y, real size_z, real datum_x, real datum_y, real datum_z, mat4 M2 = globals.Identity) {
            _Q({ size_x, size_y, size_z }, { datum_x, datum_y, datum_z }, M2);
        };
        Q(10.0, 0.1, 10.0, 0.0, -1.0, 0.0);
        Q(1.0, 3.0, 0.1, 0.0, -1.0, 0.0, M4_Translation(1.0, 1.0) * M4_RotationAboutZAxis(-*dynamixel_angle));
        Q(1.0, 3.0, 0.1, 0.0, -1.0, 0.0, M4_Translation(3.0, 1.0) * M4_RotationAboutZAxis(-*asm5601_angle));

        library.meshes.sphere.draw(P, V, M4_Translation(4.0 * sin(fish_theta), 4.0), monokai.red);

        fish_theta += .0167;;
    }

}


int main() {
    {
        {
            config.tweaks_record_raw_then_encode_everything_WARNING_USES_A_LOT_OF_DISK_SPACE = true;
            config.tweaks_soup_draw_with_rounded_corners_for_all_line_primitives = true;
        }

        APPS {
            // APP(eg_soup);
            // APP(eg_texture);
            // APP(app_diego);
            // APP(eg_kitchen_sink);
            // APP(app_hud);
            APP(app_space_fish);
        }
    }
    return 0;
}
