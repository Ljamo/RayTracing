#include "color.h"
#include "vec3.h"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    int width = 800;
    int height = 400;
    std::cout << "P3\n" << width << " " << height << "\n255\n";



    // create the window
     sf::RenderWindow window(sf::VideoMode(width, height), "Ray Tracer");


     sf::Image backgroundImage;
     backgroundImage.create(width, height, sf::Color::Black);

     for (int y = height - 1; y >= 0; y--)
     {
         std::clog << "\rScanlines remaining: " << (height - y) << ' ' << std::flush;
         for (int x = 0; x < width; x++)
         {
             // Calculate pixel color using vec3
             color pixel_color(static_cast<double>(x) / (width - 1),
                 static_cast<double>(y) / (height - 1),
                 0.2);

             // Set the pixel color in the image
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