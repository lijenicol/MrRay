#pragma once
#include "../rtutils.h"
#include "../rt_stb_image.h"

class Texture {
	public:
		virtual Colour value(double u, double v, const hit_record& rec) const = 0;
};

class SolidColour : public Texture {
	public:
		SolidColour(Colour c) : colour_value(c) {}
		SolidColour(double r, double g, double b) : colour_value(Colour(r,g,b)) {}
		virtual Colour value(double u, double v, const hit_record& rec) const override {
			return colour_value;
		}
	private:
		Colour colour_value;
};

class NormalTexture : public Texture {
public:
	NormalTexture() {}
	virtual Colour value(double u, double v, const hit_record& rec) const override {
		return (rec.normal + Vec3(1, 1, 1)) / 2.0;
	}
};

class CheckerTexture : public Texture {
	public:
		CheckerTexture(Colour c1, Colour c2) : 
			odd(std::make_shared<SolidColour>(c1)), even(std::make_shared<SolidColour>(c2)) {}
		CheckerTexture(std::shared_ptr<Texture> t1, std::shared_ptr<Texture> t2) : odd(t1), even(t2) {}

		virtual Colour value(double u, double v, const hit_record& rec) const override {
			// Taking the sign of the sines gives us the checker texture
			double frequency = 8.0;
			double sines = sin(frequency*u*(2*pi)) * sin(frequency *v*(2*pi));
			if (sines < 0)
				return odd->value(u, v, rec);
			return even->value(u, v, rec);
		}
	private:
		std::shared_ptr<Texture> odd;
		std::shared_ptr<Texture> even;
};

class ImageTexture : public Texture {
	public:
		const static int bytes_per_pixel = 3;
		ImageTexture(const char* path) {
			int components_per_pixel = bytes_per_pixel;

			// Load image through stb_image 
			std::cerr << "Loading Image: " << path << "\n";
			data = stbi_load(path, &width, &height, &components_per_pixel, components_per_pixel);

			// Handle incorrect path
			if (!data) {
				std::cerr << "ERROR: Could not load path: " << path << ".\n";
				width = height = 0;
			}

			bytes_per_scanline = components_per_pixel * width;
		}

		~ImageTexture() { delete data; }

		virtual Colour value(double u, double v, const hit_record& rec) const override {
			// Return solid cyan if no image
			if(data == nullptr)
				return Colour(0.5, 0, 0.5);
			
			// Get pixel coordinates
			u = clamp(u, 0, 1);
			v = 1.0 - clamp(v, 0, 1);

			int i = static_cast<int>(u * width);
			int j = static_cast<int>(v * height);

			// Keeps i & j below the width and height (due to 0 index)
			if (i >= width) i = width - 1;
			if (j >= height) j = height - 1;

			// Output pixel colour
			const double colour_scale = 1.0 / 255.0;
			unsigned char* pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
			return Colour(colour_scale * pixel[0], colour_scale * pixel[1], colour_scale * pixel[2]);
		}

	private:
		unsigned char* data;
		int width, height;
		int bytes_per_scanline;
};