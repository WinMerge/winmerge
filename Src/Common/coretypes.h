#ifndef CORETYPES_H
#define CORETYPES_H

#if !defined(COM_MODEL)
#include "idltypes.h"
#endif

class FRect : public FRECT
{
public:
	FRect(float l = 0.f, float t = 0.f, float r = 0.f, float b = 0.f)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}
	FRect(const FRECT& src)
	{
		Copy(src);
	}
	void Empty() { left = top = right = bottom = 0.f; }
	FRect& Copy(const FRECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
		return (*this);
	}
	void Normalize() 
	{
		float t;
		if (Width() < 0.f)
		{
			t = left;
			left = right;
			right = t;
		}
		if (Height() < 0.f)
		{
			t = top;
			top = bottom;
			bottom = t;
		}
	}
	float Width() { return (right>left? right-left : left-right); }
	float Height() { return (top>bottom? top-bottom : bottom-top); }
};

class FPoint : public FPOINT
{
public:
	FPoint(float ptx = 0.f, float pty = 0.f)
	{
		x = ptx;
		y = pty;
	}
	FPoint(const FPOINT& src)
	{
		Copy(src);
	}
	FPoint& Copy(const FPOINT& src)
	{
		x = src.x;
		y = src.y;
		return (*this);
	}
	void Empty() { x = y = 0.f; }
};


class FSize : public FSIZE
{
public:
	FSize(float w = 0.f, float h = 0.f)
	{
		width = w;
		height = h;
	}
	FSize(const FSIZE& src)
	{
		Copy(src);
	}
	FSize& Copy(const FSIZE& src)
	{
		width = src.width;
		height = src.height;
		return (*this);
	}
	void Empty() { width = height = 0.f; }
};



class FArc : public FARC
{
public:
	FArc()
	{
		Empty();
	}
	FArc(float x, float y, float r, float a1, float a2)
	{
		radius = r;
		start_angle = a1;
		end_angle = a2;
		center.x = x;
		center.y = y;
	}
	FArc(const FARC& src)
	{
		Copy(src);
	}
	FArc& Copy(const FARC& src)
	{
		radius = src.radius;
		start_angle = src.start_angle;
		end_angle = src.end_angle;
		center.x = src.center.x;
		center.y = src.center.y; 
		return (*this);
	}
	void Empty() 
	{ 
		radius = 0.f;
		start_angle = 0.f;
		end_angle = 0.f;
		center.x = 0.f;
		center.y = 0.f; 
	}
};




#endif








