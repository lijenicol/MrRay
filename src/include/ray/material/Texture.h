#ifndef MATERIAL_TEXTURE_H
#define MATERIAL_TEXTURE_H

#include "ray/namespace.h"
#include "ray/rtutils.h"

RAY_NAMESPACE_OPEN_SCOPE

struct hit_record;

class Texture {
	public:
		virtual Colour value(double u, double v, const hit_record& rec) const = 0;
};

class SolidColour : public Texture {
	public:
		SolidColour(Colour c) : colour_value(c) {}
		SolidColour(double r, double g, double b) : colour_value(Colour(r,g,b)) {}
		virtual Colour value(double u, double v, 
			   	  			 const hit_record& rec) const override {
			return colour_value;
		}
	private:
		Colour colour_value;
};

class NormalTexture : public Texture {
public:
	NormalTexture() {}
	virtual Colour value(double u, double v, 
					     const hit_record& rec) const override;
};

class CheckerTexture : public Texture {
	public:
		CheckerTexture(Colour c1, Colour c2) : 
			odd(std::make_shared<SolidColour>(c1)), even(std::make_shared<SolidColour>(c2)) {}
		CheckerTexture(std::shared_ptr<Texture> t1, std::shared_ptr<Texture> t2) : odd(t1), even(t2) {}

		virtual Colour value(double u, double v, 
			         	     const hit_record& rec) const override;
	private:
		std::shared_ptr<Texture> odd;
		std::shared_ptr<Texture> even;
};

class ImageTexture : public Texture {
	public:
		const static int bytes_per_pixel = 3;
		ImageTexture(const char* path);

		~ImageTexture() { delete data; }

		virtual Colour value(double u, double v, 
					    	 const hit_record& rec) const override;

	private:
		unsigned char* data;
		int width, height;
		int bytes_per_scanline;
};

RAY_NAMESPACE_CLOSE_SCOPE

#endif