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
         for (int x = 0; x < width; x++)
         {
             float r = static_cast<float>(x) / static_cast<float>(width);
             float g = static_cast<float>(y) / static_cast<float>(height);
             float b = 0.2;
             int ir = static_cast<int>(255.99 * r);
             int ig = static_cast<int>(255.99 * g);
             int ib = static_cast<int>(255.99 * b);

             // Set the pixel color in the image
             backgroundImage.setPixel(x, height - 1 - y, sf::Color(ir, ig, ib));
         }
     }

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