#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>
#include <SFML/Graphics.hpp>

// UNCOMMENT FOR ALTERNATIVE RENDERING LOOP
//#include <thread>
//#include <vector>

class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 800;
    int samples_per_pixel = 10;
    int max_depth = 10;

    double vfov = 90;
    point3 lookfrom = point3(0, 0, -1);
    point3 lookat = point3(0, 0, 0);
    vec3 vup = vec3(0, 1, 0);

    double defocus_angle = 0;
    double focus_dist = 10;

    void render(const hittable& world)
    {
        initialize();

        // Declare sf::Image before usage
        sf::Image backgroundImage;
        backgroundImage.create(image_width, image_height, sf::Color::Black);

        std::cout << image_width << "px by " << image_height << "px\n";

        sf::RenderWindow window(sf::VideoMode(800, 450), "Ray Tracer");

        // Create a vertex array to hold the pixels
        sf::VertexArray pixels(sf::Points, image_width * image_height);

        std::cout << "\rRendering in real-time..." << std::endl;

        // Add these lines for the update frequency
        const int update_frequency = 10;  // Update window every 10 scanlines
        int update_counter = 0;

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }

                sf::Color sfml_color = to_sfml_color(pixel_color, samples_per_pixel);
                backgroundImage.setPixel(i, j, sfml_color);

                // Set the color of the corresponding pixel in the vertex array
                pixels[i + j * image_width].position = sf::Vector2f(i, j);
                pixels[i + j * image_width].color = sfml_color;
            }

            // Increment the update counter
            update_counter++;

            if (update_counter >= update_frequency) {
                // Clear the window and draw the vertex array
                window.clear();
                window.draw(pixels);
                window.display();

                // Reset the counter
                update_counter = 0;
            }
        }

        // After the loop, ensure to update the window with any remaining pixels
        window.clear();
        window.draw(pixels);
        window.display();

        std::clog << "\rDone.                 \n";

        // Main loop
        while (window.isOpen())
        {
            // Handle events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
        }
    }



private:
    int     image_height;   // Rendered image height
    point3  center;         // Camera center
    point3  pixel00_loc;    // Location of pixel 0, 0
    vec3    pixel_delta_u;  // Offset to pixel to the right
    vec3    pixel_delta_v;  // Offset to pixel below
    vec3    u, v, w;
    vec3    defocus_disk_u;
    vec3    defocus_disk_v;

    void initialize()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // Declare sf::Image before usage
        sf::Image backgroundImage;
        backgroundImage.create(image_width, image_height, sf::Color::Black);

        center = lookfrom;

        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left =
            center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const 
    {
        // Get a randomly sampled camera ray for the pixel at location i,j.
        // originates from defocus disk

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    point3 defocus_disk_sample() const
    {
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 pixel_sample_square() const {
        // Returns a random point in the square surrounding a pixel at the origin.
        auto u = (static_cast<double>(rand()) / RAND_MAX);
        auto v = (static_cast<double>(rand()) / RAND_MAX);
        return (u * pixel_delta_u) + (v * pixel_delta_v);
    }


    color ray_color(const ray& r, int depth, const hittable& world) const
    {
        hit_record rec;

        if (depth <= 0)
        {
            return color(0,0,0);
        }

        if (world.hit(r, interval(0.001, infinity), rec)) 
        {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);

        // Declare sf::Image before usage
        //sf::Image backgroundImage;
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};


#endif // !CAMERA_H


//REPLACE RENDER FUNCTION IF YOU WANT FASTER RENDERING
//CANNOT SEE IN REAL TIME
/*
void render(const hittable& world)
    {
        initialize();

        // Declare sf::Image before usage
        sf::Image backgroundImage;
        backgroundImage.create(image_width, image_height, sf::Color::Black);

        std::cout << image_width << "px by " << image_height << "px\n";

        // create the window
        std::cout << "\rWindow will open when calculation have completed " << std::endl;

        std::vector<std::thread> threads;
        const int num_threads = std::thread::hardware_concurrency();

        for (int t = 0; t < num_threads; ++t)
        {
            threads.emplace_back([this, &world, t, num_threads, &backgroundImage]() {
                for (int j = t; j < image_height; j += num_threads)
                {
                    std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                    for (int i = 0; i < image_width; ++i)
                    {
                        color pixel_color(0, 0, 0);
                        for (int sample = 0; sample < samples_per_pixel; ++sample) {
                            ray r = get_ray(i, j);
                            pixel_color += ray_color(r, max_depth, world);
                        }

                        sf::Color sfml_color = to_sfml_color(pixel_color, samples_per_pixel);
                        backgroundImage.setPixel(i, j, sfml_color);
                    }
                }
                });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        std::clog << "\rDone.                 \n";

        sf::RenderWindow window(sf::VideoMode(800, 450), "Ray Tracer");

        // Create a texture and sprite to display the image
        sf::Texture backgroundTexture;
        backgroundTexture.loadFromImage(backgroundImage);
        sf::Sprite backgroundSprite(backgroundTexture);

        // Main loop
        while (window.isOpen())
        {
            // Handle events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // Clear the window
            window.clear();

            // Draw the background sprite
            window.draw(backgroundSprite);

            // Display the contents of the window
            window.display();
        }
    }
*/
