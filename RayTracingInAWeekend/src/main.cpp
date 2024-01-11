#include "color.h"
#include "vec3.h"
#include "ray.h"

#include <iostream>
#include <SFML/Graphics.hpp>

double hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    // Quadratic equation discriminant
    // b^2 - 4ac
    auto discriminant = half_b * half_b - a * c;

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (-half_b - sqrt(discriminant)) / a;
    }

    // Returns true if discriminant is greater than 0
    return (discriminant >= 0);
}

// currently only returns black
color ray_color(const ray& r)
{
    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);
    // checks if ray intersects object and returns red if true
    if (t > 0.0)
    {
        vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);

    }


    // gradient background
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
}

int main()
{
    int width = 800;
    int height = 400;



    //  Rendering
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

            vec3 pixel_color = ray_color(r);

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