#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

#include "color.h"
#include "hittable.h"

#include <iostream>
#include <SFML/Graphics.hpp>

class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 800;
    int samples_per_pixel = 10;
    int max_depth = 10;

    void render(const hittable& world)
    {
        initialize();

        // Declare sf::Image before usage
        sf::Image backgroundImage;
        backgroundImage.create(image_width, image_height, sf::Color::Black);

        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
        
        // create the window
        sf::RenderWindow window(sf::VideoMode(800, 450), "Ray Tracer");


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
            }

        }
        std::clog << "\rDone.                 \n";

        

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

private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below

    void initialize()
    {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // Declare sf::Image before usage
        sf::Image backgroundImage;
        backgroundImage.create(image_width, image_height, sf::Color::Black);

        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left =
            center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const {
        // Get a randomly sampled camera ray for the pixel at location i,j.

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square();

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
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
            vec3 direction = rec.normal + random_unit_vector();
            return 0.7 * ray_color(ray(rec.p, direction), depth-1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);

        // Declare sf::Image before usage
        //sf::Image backgroundImage;
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};


#endif // !CAMERA_H
