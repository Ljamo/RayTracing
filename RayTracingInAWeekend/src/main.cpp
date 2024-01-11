#include "common.h"

#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>
#include <SFML/Graphics.hpp>

// currently only returns black
color ray_color(const ray& r, const hittable& world)
{
    hit_record rec;

    // checks if ray intersects object and returns red if true
    if (world.hit(r, 0, infinity, rec))
    {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }

    // gradient background
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    // Image
    int width = 800;
    int height = 400;

    // World
    hittable_list world;

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Rendering
    std::cout << "P3\n" << width << " " << height << "\n255\n";



    // create the window
    sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");

    vec3 lower_left_corner(-2.0, -1.0, -1.0);
    vec3 horizontal(4.0, 0.0, 0.0);
    vec3 vertical(0.0, 2.0, 0.0);
    vec3 origin(0.0, 0.0, 0.0);


    sf::Image backgroundImage;
    backgroundImage.create(width, height, sf::Color::Black);

    for (int y = height - 1; y >= 0; y--)
    {
        std::clog << "\rScanlines remaining: " << (height - y) << ' ' << std::flush;
        for (int x = 0; x < width; x++)
        {
            float u = float(x) / float(width);
            float v = float(y) / float(height);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);


            // Calculate pixel color using vec3
            //color pixel_color(static_cast<double>(x) / (width - 1),
            //    static_cast<double>(y) / (height - 1),
            //    0.2);

            // Set the pixel color in the image

            vec3 pixel_color = ray_color(r, world);

            backgroundImage.setPixel(x, height - 1 - y, sf::Color(
                static_cast<sf::Uint8>(255.999 * pixel_color.x()),
                static_cast<sf::Uint8>(255.999 * pixel_color.y()),
                static_cast<sf::Uint8>(255.999 * pixel_color.z())
            ));

            //optional if you want to output the pixel colours (doesnt work)
            //write_color(std::cout, pixel_color);
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


    return 0;
}