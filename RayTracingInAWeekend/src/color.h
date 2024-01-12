#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <SFML/Graphics/Color.hpp>

using color = vec3;

inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

sf::Color to_sfml_color(const color& pixel_color, int samples_per_pixel)
{
    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    auto r = pixel_color.x() * scale;
    auto g = pixel_color.y() * scale;
    auto b = pixel_color.z() * scale;

    // Apply the linear to gamma transform.
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Write the translated [0,255] value of each color component.
    static const interval intensity(0.000, 0.999);

    return sf::Color(
        static_cast<sf::Uint8>(256 * intensity.clamp(r)),
        static_cast<sf::Uint8>(256 * intensity.clamp(g)),
        static_cast<sf::Uint8>(256 * intensity.clamp(b))
    );
}


#endif
